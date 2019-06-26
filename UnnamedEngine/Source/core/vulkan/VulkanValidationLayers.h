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

#ifndef CORE_VULKAN_VULKANVALIDATIONLAYERS_H_
#define CORE_VULKAN_VULKANVALIDATIONLAYERS_H_

#include <vector>
#include "../Window.h"

/*****************************************************************************
 * The VulkanValidationLayers class
 *****************************************************************************/

class VulkanValidationLayers {
private:
	/* Stores all the requested validation layers to use */
	static std::vector<const char*> validationLayers;

	/* Debug messenger */
	static VkDebugUtilsMessengerEXT debugMessenger;

	/* Method to create a debug messenger */
	static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	/* Method to destroy a debug messenger */
	static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	/* Callback method for debug messeges */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		    VkDebugUtilsMessageTypeFlagsEXT messageType,
		    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		    void* pUserData);
public:
	/* Returns whether the validation layers required are supported */
	static bool checkSupport();

	/* Method to create a debug messenger for displaying output for validation layers */
	static void createDebugMessenger();

	/* Method to destroy the debug messenger */
	static void destroyDebugMessenger();

	/* Returns a list of the required validation layers */
	static std::vector<const char*>& getLayers() { return validationLayers; }
};

#endif /* CORE_VULKAN_VULKANVALIDATIONLAYERS_H_ */
