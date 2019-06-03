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

/*****************************************************************************
 * The Vulkan class
 *****************************************************************************/

VkInstance               Vulkan::instance;
VkDebugUtilsMessengerEXT Vulkan::debugMessenger;
VkSurfaceKHR             Vulkan::windowSurface;
VulkanDevice*            Vulkan::device;
VulkanSwapChain*         Vulkan::swapChain;

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

	//If have reached this point, initialisation successful
	return true;
}

void Vulkan::destroy() {
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
