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

#include "VulkanValidationLayers.h"

#include <cstdint>
#include "Vulkan.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The VulkanValidationLayers class
 *****************************************************************************/

std::vector<const char*> VulkanValidationLayers::validationLayers = {
		"VK_LAYER_KHRONOS_validation"
		//VK_LAYER_LUNARG_standard_validation
		//"VK_LAYER_LUNARG_api_dump"
};

VkDebugUtilsMessengerEXT VulkanValidationLayers::debugMessenger;

bool VulkanValidationLayers::checkSupport() {
	//Obtain the supported layers
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//Go through the required layers and ensure all of the required layers are supported
	for (const char* layerName : validationLayers) {
		//States whether the layer was found in the list of available layers
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				//Have found the required layer in the list of supported layers
				layerFound = true;
				break;
			}
		}

		if (! layerFound)
			return false; //Found a layer that isn't supported
	}

	return true;
}

void VulkanValidationLayers::createDebugMessenger() {
	//Only bother if validation layers are enabled
	if (! Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled)
		return;

	//Setup the information required
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	//Attempt to create the messenger
	if (createDebugUtilsMessengerEXT(Vulkan::getInstance(), &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		Logger::log("Failed to create a debug messenger", "VulkanValidationLayers", LogType::Error);
}

void VulkanValidationLayers::destroyDebugMessenger() {
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled)
		destroyDebugUtilsMessengerEXT(Vulkan::getInstance(), debugMessenger, nullptr);
}

VkResult VulkanValidationLayers::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	//vkCreateDebugUtilsMessengerEXT is from extension, so not loaded by default
}

void VulkanValidationLayers::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanValidationLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	LogType logType;
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		logType = LogType::Debug;
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		logType = LogType::Information;
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		logType = LogType::Warning;
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		logType = LogType::Error;
	//Output the message
	Logger::log(pCallbackData->pMessage, "Vulkan (ValidationLayer)", logType);

	return VK_FALSE;
}

