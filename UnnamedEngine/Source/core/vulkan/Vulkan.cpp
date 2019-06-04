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

std::vector<float> Vulkan::vertices = {
	-0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,     1.0f, 1.0f, 1.0f
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
	vertexBuffer->setup();
	indexBuffer  = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices, GL_STATIC_DRAW);
	indexBuffer->setup();

	//Create the graphics pipeline
	graphicsPipeline = new VulkanGraphicsPipeline(swapChain, vertexBuffer, renderPass);

	//Create the command buffers
	createCommandBuffers();

	//Create the synchronisation objects
	createSyncObjects();

	//If have reached this point, initialisation successful
	return true;
}

void Vulkan::destroy() {
	//Wait for a suitable time
	vkDeviceWaitIdle(device->getLogical());

	destroySyncObjects();

	delete graphicsPipeline;

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
	poolCreateInfo.flags            = 0; //Optional VK_COMMAND_POOL_CREATE_TRANSIENT_BIT - if buffers will be updated many times

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
			throw std::runtime_error("Failed to begin recording command buffer");

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
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit

		//vkCmdDraw(commandBuffers[i], vertexBuffer->getNumVertices(), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(6), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			Logger::log("Failed to record command buffer", "Vulkan", LogType::Error);
	}
}

void Vulkan::createSyncObjects() {
	//Create the semaphores and fences
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; //Start signalled as default is not and would cause 'vkWaitForFences called for fence 0x11 which has not been submitted on a Queue or during acquire next image'

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		if (vkCreateSemaphore(device->getLogical(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->getLogical(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device->getLogical(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			Logger::log("Failed to create synchronisation objects for a frame", "Vulkan", LogType::Error);
		}
	}
}

void Vulkan::destroySyncObjects() {
	//Destroy the semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(device->getLogical(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device->getLogical(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device->getLogical(), inFlightFences[i], nullptr);
	}
}

void Vulkan::drawFrame() {
	//Wait for all fences (VK_TRUE)
	vkWaitForFences(device->getLogical(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device->getLogical(), 1, &inFlightFences[currentFrame]); //Unlike semaphores have to reset after use

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device->getLogical(), swapChain->getInstance(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffers[imageIndex];

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
	presentInfo.pImageIndices  = &imageIndex;
	presentInfo.pResults       = nullptr;

	vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
