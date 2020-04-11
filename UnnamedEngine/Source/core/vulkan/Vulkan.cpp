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

#include "../render/Mesh.h"
#include "../../utils/Logging.h"
#include "../render/DescriptorSet.h"

#include <limits>
#include <array>

/*****************************************************************************
 * The Vulkan class
 *****************************************************************************/

VkInstance                                   Vulkan::instance;
VkSurfaceKHR                                 Vulkan::windowSurface;
VulkanDevice*                                Vulkan::device;
VulkanSwapChain*                             Vulkan::swapChain;
VulkanRenderPass*                            Vulkan::renderPass;
VkCommandPool                                Vulkan::commandPool;
std::vector<VkCommandBuffer>                 Vulkan::commandBuffers;
std::vector<VkSemaphore>                     Vulkan::imageAvailableSemaphores;
std::vector<VkSemaphore>                     Vulkan::renderFinishedSemaphores;
std::vector<VkFence>                         Vulkan::inFlightFences;
unsigned int                                 Vulkan::currentFrame = 0;
VulkanGraphicsPipeline*                      Vulkan::currentGraphicsPipeline = NULL;
std::vector<Vulkan::DescriptorSetUpdateInfo> Vulkan::descriptorSetUpdateQueue;
std::vector<Vulkan::UBOUpdateInfo>			 Vulkan::uboUpdateQueue;

bool Vulkan::initialise(Window* window) {
	//Initialise Vulkan
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled && ! VulkanValidationLayers::checkSupport()) {
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
	VulkanValidationLayers::createDebugMessenger();
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

	//Use MSAA?
	if (window->getSettings().videoSamples > 0) {
		//Obtain the number of MSAA samples to use based on the requested value and what is supported
		VkSampleCountFlagBits sampleCount = getMaxUsableSampleCount(window->getSettings().videoSamples);
		//Assign the value based on what was obtained
		window->getSettings().videoSamples = sampleCount;
	}

	//Create the command pool
	createCommandPool();

	//Create the swap chain
	swapChain = new VulkanSwapChain(device, window->getSettings());

	//Create the render pass
	renderPass = new VulkanRenderPass(swapChain);

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

	destroyCommandPool();

	delete renderPass;
	delete swapChain;
	delete device;

	destroyWindowSurface();
	VulkanValidationLayers::destroyDebugMessenger();
	destroyInstance();
}

bool Vulkan::createInstance() {
	//Fill out required information
	VkApplicationInfo appInfo = {};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = Window::getCurrentInstance()->getSettings().windowTitle.c_str();
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
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled) {
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
}

void Vulkan::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = mipLevels;
    imageInfo.arrayLayers   = arrayLayers;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = numSamples;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags         = flags;

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

VkImageView Vulkan::createImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layerCount) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image    = image;
	viewInfo.viewType = viewType;
	viewInfo.format   = format;
	viewInfo.subresourceRange.aspectMask     = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = layerCount;

	VkImageView imageView;
	if (vkCreateImageView(device->getLogical(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		Logger::log("Failed to create image view", "Vulkan", LogType::Error);

	return imageView;
}

void Vulkan::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount, VkCommandBuffer commandBuffer) {
	bool createCommandBuffer = (commandBuffer == VK_NULL_HANDLE);
	if (createCommandBuffer)
		commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout; //Can use VK_IMAGE_LAYOUT_UNDEFINED if don't care about existing contents of image
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;

	//Assign correct aspect mask for depth(possibly with stencil)/texture image
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format))
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	} else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = layerCount;

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
	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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

	if (createCommandBuffer)
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

void Vulkan::startDraw() {
	//Aquire the next swap chain image
	vkAcquireNextImageKHR(device->getLogical(), swapChain->getInstance(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentFrame); //Image index is next in chain

	//Wait for all fences (VK_TRUE)
	vkWaitForFences(device->getLogical(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device->getLogical(), 1, &inFlightFences[currentFrame]); //Unlike semaphores have to reset after use

	//------------------------------------------------------------------------------------------------------
	vkResetCommandBuffer(commandBuffers[currentFrame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

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

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 }; //1.0 is far view plane, 0.0 is near view plane

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Vulkan::stopDraw() {
	//Update descriptor sets and UBOs
	updateDescriptorSetQueue();
	updateUBOQueue();


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
	VkResult result = vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
	if (result != VK_SUCCESS) //vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS
		Logger::log("Failed to submit draw command buffer, result " + utils_string::str(result), "Vulkan", LogType::Error);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain->getInstance() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains    = swapChains;
	presentInfo.pImageIndices  = &currentFrame;
	presentInfo.pResults       = nullptr;

	result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
	if (result != VK_SUCCESS)
		Logger::log("Failed to present image from queue " + utils_string::str(result), "Vulkan", LogType::Error);

	//vkAcquireNextImageKHR semaphore signalled will be the one with this index (so must increase before it is called again)
	currentFrame = (currentFrame + 1) % swapChain->getImageCount();
}

void Vulkan::bindGraphicsPipeline(VulkanGraphicsPipeline* pipeline) {
	currentGraphicsPipeline = pipeline;

	//Bind the pipeline
	vkCmdBindPipeline(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getInstance());
}

bool Vulkan::updateDescriptorSetFrame(DescriptorSetUpdateInfo& info) {
	//Update the set for the current frame
	info.set->updateVk(info.nextUpdateFrame);

	//Reduce the number of updates left by 1
	info.updatesLeft--;
	info.nextUpdateFrame = (info.nextUpdateFrame + 1) % swapChain->getImageCount();

	return info.updatesLeft <= 0;
}

void Vulkan::updateDescriptorSet(DescriptorSet* set) {
	//Setup the structure for the queue
	DescriptorSetUpdateInfo info;
	info.set             = set;
	info.nextUpdateFrame = getNextFrame();
	info.updatesLeft     = getSwapChain()->getImageCount();

	//Add the set to the update queue
	descriptorSetUpdateQueue.push_back(info);
}

void Vulkan::updateUBO(UBO* ubo, void* data, unsigned int offset, unsigned int size) {
	//Setup the structure for the queue
	UBOUpdateInfo info;
	info.ubo             = ubo;
	info.data            = data;
	info.offset          = offset;
	info.size            = size;
	info.nextUpdateFrame = getNextFrame();
	info.updatesLeft     = getSwapChain()->getImageCount();

	//Add the set to the update queue
	uboUpdateQueue.push_back(info);
}

void Vulkan::updateDescriptorSetQueue() {
	//Indices of data to be removed
	unsigned int removeEnd = 0;
	//Go through the descriptor sets
	for (unsigned int i = 0; i < descriptorSetUpdateQueue.size(); ++i) {
		//Update the current set, and prepare to remove it if finished updating
		if (updateDescriptorSetFrame(descriptorSetUpdateQueue[i]))
			removeEnd++;
	}
	//Remove all finished updates from the queue
	if (removeEnd > 0)
		descriptorSetUpdateQueue.erase(descriptorSetUpdateQueue.begin(), descriptorSetUpdateQueue.begin() + removeEnd);
}

bool Vulkan::updateUBOFrame(UBOUpdateInfo& info) {
	//Update the set for the current frame
	info.ubo->updateFrame(info.data, info.offset, info.size);

	//Reduce the number of updates left by 1
	info.updatesLeft--;
	info.nextUpdateFrame = (info.nextUpdateFrame + 1) % swapChain->getImageCount();

	return info.updatesLeft <= 0;
}

void Vulkan::updateUBOQueue() {
	//Indices of data to be removed
	unsigned int removeEnd = 0;
	//Go through the descriptor sets
	for (unsigned int i = 0; i < uboUpdateQueue.size(); ++i) {
		//Update the current set, and prepare to remove it if finished updating
		if (updateUBOFrame(uboUpdateQueue[i]))
			removeEnd++;
	}
	//Remove all finished updates from the queue
	if (removeEnd > 0)
		uboUpdateQueue.erase(uboUpdateQueue.begin(), uboUpdateQueue.begin() + removeEnd);
}

VkSampleCountFlagBits Vulkan::getMaxUsableSampleCount(unsigned int targetSamples) {
	//Obtain the Vulkan representation of the target number of samples
	VkSampleCountFlagBits targetSampleBits;

	//Obtain the supported values
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(device->getPhysical(), &physicalDeviceProperties);
	VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	//Find the closest supported
	while (targetSamples > 1) {
		targetSampleBits = static_cast<VkSampleCountFlagBits>(targetSamples); //Should be same integer value

		if (counts & targetSampleBits)
			//Supported so stop
			break;
		else
			targetSamples /= 2;
	}

	//Return the final value
	return targetSampleBits;
}

VkFormat Vulkan::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->getPhysical(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}
	Logger::log("Failed to find suitable format", "Vulkan 'findSupportedFormat'", LogType::Error);
	return VK_FORMAT_UNDEFINED;
}

void Vulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //Only used by graphics queue

	if (vkCreateBuffer(device->getLogical(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		Logger::log("Failed to create buffer", "VulkanBuffer", LogType::Error);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device->getLogical(), buffer, &memoryRequirements);

	//Allocate memory to this buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getLogical(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		Logger::log("Failed to allocate buffer memory", "VulkanBuffer", LogType::Error);

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
