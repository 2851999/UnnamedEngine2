/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#include "Vulkan.h"

#include "VulkanExtensions.h"
#include "VulkanValidationLayers.h"

#include "../../utils/Logging.h"

#include <stb/stb_image.h>
#include <limits>

/*****************************************************************************
 * The Vulkan class
 *****************************************************************************/

VkInstance                   Vulkan::instance;
VkDebugUtilsMessengerEXT     Vulkan::debugMessenger;
VkSurfaceKHR                 Vulkan::windowSurface;
VulkanDevice*                Vulkan::device;
VulkanSwapChain*             Vulkan::swapChain;
VulkanRenderPass*            Vulkan::renderPass;
VBO<float>*                  Vulkan::vertexBuffer;
VBO<unsigned int>*           Vulkan::indexBuffer;
VulkanGraphicsPipeline*      Vulkan::graphicsPipeline;
VkCommandPool                Vulkan::commandPool;
std::vector<VkCommandBuffer> Vulkan::commandBuffers;
std::vector<VkSemaphore>     Vulkan::imageAvailableSemaphores;
std::vector<VkSemaphore>     Vulkan::renderFinishedSemaphores;
std::vector<VkFence>         Vulkan::inFlightFences;
unsigned int                 Vulkan::currentFrame = 0;
VkDescriptorPool             Vulkan::descriptorPool;
VkDescriptorSetLayout        Vulkan::descriptorSetLayout;
std::vector<VkDescriptorSet> Vulkan::descriptorSets;
std::vector<VulkanBuffer*>   Vulkan::uniformBuffers;
Vulkan::UBOData              Vulkan::uboData;
VkImage                      Vulkan::textureImage;
VkDeviceMemory               Vulkan::textureImageMemory;
VkImageView                  Vulkan::textureImageView;
VkSampler                    Vulkan::textureSampler;

std::vector<float> Vulkan::vertices = {
	-0.5f, -0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
	 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	 0.5f,  0.5f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
	-0.5f,  0.5f,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f
};

std::vector<unsigned int> Vulkan::indices = {
		0, 1, 2, 2, 3, 0
};

bool Vulkan::ENABLE_VALIDATION_LAYERS = true;

bool Vulkan::initialise(Window* window) {
	//Initialise Vulkan
	if (ENABLE_VALIDATION_LAYERS && ! VulkanValidationLayers::checkSupport()) {
		Logger::log("Required validation layers are not supported", "Vulkan", LogType::Error);
		return false;
	}
	//Add the required extensions
	VulkanExtensions::addRequired();
	//Create the Vulkan instance
	if (! createInstance()) {
		Logger::log("Vulkan instance could not be created", "Vulkan", LogType::Error);
		return false;
	}
	//Create the debug messenger (If validation layers are enabled)
	createDebugMessenger();
	//Create the window surface
	if (! createWindowSurface(window)) {
		Logger::log("Window surface instance could not be created", "Vulkan", LogType::Error);
		return false;
	}

	//Create the device
	device = VulkanDevice::create();

	//Check whether the device creation was successful
	if (! device) {
		Logger::log("Failed to create a VulkanDevice", "Vulkan", LogType::Error);
		return false;
	}

	//Create the swap chain
	swapChain = new VulkanSwapChain(device, window->getSettings());

	//Create the render pass
	renderPass = new VulkanRenderPass(swapChain);

	//Create the command pool
	createCommandPool();

	//Create the vertex and index buffers
	vertexBuffer = new VBO<float>(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
	vertexBuffer->addAttribute(0, 2);
	vertexBuffer->addAttribute(1, 3);
	vertexBuffer->addAttribute(2, 2);
	vertexBuffer->setup();
	indexBuffer  = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices, GL_STATIC_DRAW);
	indexBuffer->setup();

	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	//Create the descriptor pool
	createDescriptorPool();

	createUniformBuffers();

	createDescriptorSetLayout();
	createDescriptorSets();

	//Create the graphics pipeline
	graphicsPipeline = new VulkanGraphicsPipeline(swapChain, vertexBuffer, renderPass, descriptorSetLayout);

	//Create the command buffers
	createCommandBuffers();

	//Create the synchronisation objects
	createSyncObjects();

	//If have reached this point, initialisation successful
	return true;
}

void Vulkan::destroy() {
	//Wait for a suitable time
	waitDeviceIdle();

	destroySyncObjects();

	vkDestroySampler(device->getLogical(), textureSampler, nullptr);
	vkDestroyImageView(device->getLogical(), textureImageView, nullptr);

    vkDestroyImage(device->getLogical(), textureImage, nullptr);
    vkFreeMemory(device->getLogical(), textureImageMemory, nullptr);

	delete graphicsPipeline;

	destroyDescriptorSetLayout();
	destroyUniformBuffers();

	destroyDescriptorPool();

	delete vertexBuffer;
	delete indexBuffer;

	destroyCommandPool();

	delete renderPass;
	delete swapChain;
	delete device;

	destroyWindowSurface();
	destroyDebugMessenger();
	destroyInstance();
}

bool Vulkan::createInstance() {
	//Fill out required information
	VkApplicationInfo appInfo = {};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "Unnamed Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Obtain the required extensions
	std::vector<const char*>& requiredExtensions = VulkanExtensions::getExtentions();

	createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	//Obtain the required validation layers (if they are enabled)
	if (Vulkan::ENABLE_VALIDATION_LAYERS) {
		std::vector<const char*>& requiredValidationLayers = VulkanValidationLayers::getLayers();  //requiredValidationLayers goes out of scope if not using reference causing undefined behaviour

		createInfo.enabledLayerCount   = static_cast<uint32_t>(requiredValidationLayers.size());
		createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
	} else
		createInfo.enabledLayerCount = 0;

	//Attempt to create the instance
	return vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS;
}

void Vulkan::destroyInstance() {
	vkDestroyInstance(instance, nullptr);
}

void Vulkan::createDebugMessenger() {
	//Only bother if validation layers are enabled
	if (! Vulkan::ENABLE_VALIDATION_LAYERS)
		return;

	//Setup the information required
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	//Attempt to create the messenger
	if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		Logger::log("Failed to create a debug messenger", "Vulkan", LogType::Error);
}

void Vulkan::destroyDebugMessenger() {
	if (ENABLE_VALIDATION_LAYERS)
		destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
}

bool Vulkan::createWindowSurface(Window* window) {
	//Attempt to create the window surface
	return glfwCreateWindowSurface(instance, window->getInstance(), nullptr, &windowSurface) == VK_SUCCESS;
}

void Vulkan::destroyWindowSurface() {
	vkDestroySurfaceKHR(instance, windowSurface, nullptr);
}

void Vulkan::createCommandPool() {
	//Obtain the queue family indices from the device
	VulkanDeviceQueueFamilies& queueFamilies = device->getQueueFamilies();

	//Setup the command pool creation data
	VkCommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.queueFamilyIndex = queueFamilies.graphicsFamilyIndex;
	poolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //Optional VK_COMMAND_POOL_CREATE_TRANSIENT_BIT - if buffers will be updated many times

	//Attempt to create the command pool
	if (vkCreateCommandPool(device->getLogical(), &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
		Logger::log("Failed to create command pool", "Vulkan", LogType::Error);
}

void Vulkan::destroyCommandPool() {
	vkDestroyCommandPool(device->getLogical(), commandPool, nullptr);
}

void Vulkan::createCommandBuffers() {
	//Obtain the list of framebuffers in the swap chain
	std::vector<VkFramebuffer>& swapChainFramebuffers = renderPass->getSwapChainFramebuffers();

	//Need to allocate the same number of command buffers as framebuffers
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool        = commandPool;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

	//Attempt to allocate the command buffers
	if (vkAllocateCommandBuffers(device->getLogical(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		Logger::log("Failed to allocate command buffers", "Vulkan", LogType::Error);

	//Go through the command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			Logger::log("Failed to begin recording command buffer", "Vulkan", LogType::Error);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = renderPass->getInstance();
		renderPassInfo.framebuffer = swapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain->getExtent();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues    = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getInstance());

		VkBuffer vertexBuffers[] = { vertexBuffer->getVkBuffer()->getInstance() };
		VkDeviceSize offsets = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, &offsets);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getLayout(), 0, 1, &descriptorSets[i], 0, nullptr);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit

		//vkCmdDraw(commandBuffers[i], vertexBuffer->getNumVertices(), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(6), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			Logger::log("Failed to record command buffer", "Vulkan", LogType::Error);
	}
}

void Vulkan::createTextureImage() {
	int textureWidth, textureHeight, numChannels;
	stbi_uc* pixels = stbi_load("resources/textures/texture.jpg", &textureWidth, &textureHeight, &numChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = textureWidth * textureHeight * 4; //numChannels doesn't work???

	if (! pixels)
		Logger::log("Failed to load texture image", "Vulkan", LogType::Error);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device->getLogical(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device->getLogical(), stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	//First need to transition texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL then copy from staging buffer to the texture image
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); //VK_IMAGE_LAYOUT_UNDEFINED is initial layout (from createImage)
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));

	//Prepare image for shader access
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device->getLogical(), stagingBuffer, nullptr);
    vkFreeMemory(device->getLogical(), stagingBufferMemory, nullptr);
}

void Vulkan::createTextureImageView() {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image      = textureImage;
	viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format     = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	if (vkCreateImageView(device->getLogical(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
	    Logger::log("Failed to create texture image view", "Vulkan", LogType::Error);
}

void Vulkan::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE; //Useful for PCF shadows
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	//NOTE: Sampler not attached to image (can use again in TextureParameters?)

	if (vkCreateSampler(device->getLogical(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Vulkan", LogType::Error);
}

void Vulkan::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->getLogical(), &imageInfo, nullptr, &image) != VK_SUCCESS)
    	Logger::log("Failed to create image", "Vulkan", LogType::Error);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->getLogical(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getLogical(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		Logger::log("Failed to allocate image memory", "Vulkan", LogType::Error);

	vkBindImageMemory(device->getLogical(), image, imageMemory, 0);
}

void Vulkan::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout; //Can use VK_IMAGE_LAYOUT_UNDEFINED if don't care about existing contents of image
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image                           = image;
	barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

	    barrier.srcAccessMask = 0;
	    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	    sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

	    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	    sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else
	    Logger::log("Unsupported layout transition", "Vulkan", LogType::Error);

	vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}

void Vulkan::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset      = 0;
	region.bufferRowLength   = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
	);

	endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer Vulkan::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool        = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device->getLogical(), &allocInfo, &commandBuffer);

	//Start recording to the command buffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT not necessary here since only using once and want to wait for copy to finish

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Vulkan::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	//Stop recording to the buffer
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->getGraphicsQueue());

	vkFreeCommandBuffers(device->getLogical(), commandPool, 1, &commandBuffer);
}

void Vulkan::createSyncObjects() {
	//Number of images in swap chain
	unsigned int swapChainImageCount = swapChain->getImageCount();
	//Create the semaphores and fences
	imageAvailableSemaphores.resize(swapChainImageCount);
	renderFinishedSemaphores.resize(swapChainImageCount);
	inFlightFences.resize(swapChainImageCount);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; //Start signalled as default is not and would cause 'vkWaitForFences called for fence 0x11 which has not been submitted on a Queue or during acquire next image'

	for (unsigned int i = 0; i < swapChainImageCount; ++i) {
		if (vkCreateSemaphore(device->getLogical(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->getLogical(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device->getLogical(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			Logger::log("Failed to create synchronisation objects for a frame", "Vulkan", LogType::Error);
		}
	}
}

void Vulkan::destroySyncObjects() {
	//Destroy the semaphores and fences
	for (size_t i = 0; i < inFlightFences.size(); ++i) {
		vkDestroySemaphore(device->getLogical(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device->getLogical(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device->getLogical(), inFlightFences[i], nullptr);
	}
}

void Vulkan::createDescriptorPool() {
	//Assign the creation info
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain->getImageCount());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain->getImageCount()); //Have one for each swap chain image

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = static_cast<uint32_t>(swapChain->getImageCount());
	poolInfo.flags         = 0;

	//Attempt to create the pool
	if (vkCreateDescriptorPool(device->getLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		Logger::log("Failed to create descriptor pool", "Vulkan", LogType::Error);
}

void Vulkan::destroyDescriptorPool() {
	vkDestroyDescriptorPool(device->getLogical(), descriptorPool, nullptr);
}

void Vulkan::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(swapChain->getImageCount(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain->getImageCount());
	allocInfo.pSetLayouts        = layouts.data();

	descriptorSets.resize(swapChain->getImageCount());
	if (vkAllocateDescriptorSets(device->getLogical(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		Logger::log("Failed to allocate descriptor sets", "Vulkan", LogType::Error);

	//Allows writing of each UBO
	for (unsigned int i = 0; i < swapChain->getImageCount(); ++i) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i]->getInstance();
		bufferInfo.offset = 0;
		bufferInfo.range  = sizeof(UBOData); //Can use VK_WHOLE_SIZE

	    VkDescriptorImageInfo imageInfo = {};
	    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	    imageInfo.imageView   = textureImageView;
	    imageInfo.sampler     = textureSampler;

	    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet           = descriptorSets[i];
		descriptorWrites[0].dstBinding       = 0;
		descriptorWrites[0].dstArrayElement  = 0;
		descriptorWrites[0].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount  = 1;
		descriptorWrites[0].pBufferInfo      = &bufferInfo;
		descriptorWrites[0].pImageInfo       = nullptr; //Optional
		descriptorWrites[0].pTexelBufferView = nullptr; //Optional

		descriptorWrites[1].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet           = descriptorSets[i];
		descriptorWrites[1].dstBinding       = 1;
		descriptorWrites[1].dstArrayElement  = 0;
		descriptorWrites[1].descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount  = 1;
		descriptorWrites[1].pBufferInfo      = nullptr;
		descriptorWrites[1].pImageInfo       = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Vulkan::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding            = 0;
	uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount    = 1;
	uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT; //VK_SHADER_STAGE_ALL_GRAPHICS
	uboLayoutBinding.pImmutableSamplers = nullptr; //Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding            = 1;
	samplerLayoutBinding.descriptorCount    = 1;
	samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings    = bindings.data();

	if (vkCreateDescriptorSetLayout(device->getLogical(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		Logger::log("Failed to create descriptor set layout", "Vulkan", LogType::Error);
}

void Vulkan::destroyDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(device->getLogical(), descriptorSetLayout, nullptr);
}

void Vulkan::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UBOData);
	uniformBuffers.resize(swapChain->getImageCount());

	for (unsigned int i = 0; i < swapChain->getImageCount(); ++i)
		uniformBuffers[i] = new VulkanBuffer(bufferSize, device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
}

void Vulkan::destroyUniformBuffers() {
	for (unsigned int i = 0; i < uniformBuffers.size(); ++i)
		delete uniformBuffers[i];
	uniformBuffers.clear();
}

void Vulkan::updateUniformBuffer() {
	uboData.mvpMatrix = Matrix4f().initOrthographic(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);

	uniformBuffers[currentFrame]->copyData(&uboData, 0, sizeof(UBOData));
}

void Vulkan::startDraw() {
	//Aquire the next swap chain image
	vkAcquireNextImageKHR(device->getLogical(), swapChain->getInstance(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentFrame); //Image index is next in chain

	//Wait for all fences (VK_TRUE)
	vkWaitForFences(device->getLogical(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device->getLogical(), 1, &inFlightFences[currentFrame]); //Unlike semaphores have to reset after use

	//------------------------------------------------------------------------------------------------------
	vkResetCommandBuffer(commandBuffers[currentFrame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	updateUniformBuffer();

	//Obtain the list of framebuffers in the swap chain
	std::vector<VkFramebuffer>& swapChainFramebuffers = renderPass->getSwapChainFramebuffers();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
		Logger::log("Failed to begin recording command buffer", "Vulkan", LogType::Error);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass  = renderPass->getInstance();
	renderPassInfo.framebuffer = swapChainFramebuffers[currentFrame];

	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapChain->getExtent();

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues    = &clearColor;

	vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getInstance());
	vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getLayout(), 0, 1, &descriptorSets[currentFrame], 0, nullptr);
}

void Vulkan::stopDraw() {
	vkCmdEndRenderPass(commandBuffers[currentFrame]);

	if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS)
		Logger::log("Failed to record command buffer", "Vulkan", LogType::Error);
	//-------------------------------------------------------------------------------------------

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	//Fence added here to be signalled when command buffer finishes executing
	if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) //vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS
		Logger::log("Failed to submit draw command buffer", "Vulkan", LogType::Error);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain->getInstance() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains    = swapChains;
	presentInfo.pImageIndices  = &currentFrame;
	presentInfo.pResults       = nullptr;

	vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

	//vkAcquireNextImageKHR semaphore signalled will be the one with this index (so must increase before it is called again)
	currentFrame = (currentFrame + 1) % swapChain->getImageCount();
}

void Vulkan::drawFrame() {
	startDraw();

	VkBuffer vertexBuffers[] = { vertexBuffer->getVkBuffer()->getInstance() };
	VkDeviceSize offsets = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, &offsets);
	vkCmdBindIndexBuffer(commandBuffers[currentFrame], indexBuffer->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit

	//vkCmdDraw(commandBuffers[i], vertexBuffer->getNumVertices(), 1, 0, 0);
	vkCmdDrawIndexed(commandBuffers[currentFrame], static_cast<uint32_t>(6), 1, 0, 0, 0);

	stopDraw();
}

void Vulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //Only used by graphics queue

	if (vkCreateBuffer(device->getLogical(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		Logger::log("Failed to create vertex buffer", "VulkanBuffer", LogType::Error);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device->getLogical(), buffer, &memoryRequirements);

	//Allocate memory to this buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getLogical(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		Logger::log("Failed to allocate vertex buffer memory", "VulkanBuffer", LogType::Error);

	//Associate the memory with the buffer
	vkBindBufferMemory(device->getLogical(), buffer, bufferMemory, 0);
}

void Vulkan::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool, VkQueue& graphicsQueue) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size      = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

uint32_t Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(device->getPhysical(), &memoryProperties);

	//Should also check what heap memory comes from since this can also affect performance - not done here

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) //Check if corresponding bit for current memory type is 1
				&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	Logger::log("Failed to find suitable memory type", "VulkanBuffer", LogType::Error);
	return 0;
}

VkResult Vulkan::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	//vkCreateDebugUtilsMessengerEXT is from extension, so not loaded by default
}

void Vulkan::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	    VkDebugUtilsMessageTypeFlagsEXT messageType,
	    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void* pUserData) {
	//Output the message
	Logger::log(pCallbackData->pMessage, "Vulkan (ValidationLayer)", LogType::Warning);

	return VK_FALSE;
}
