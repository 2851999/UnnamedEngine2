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

#ifndef CORE_VULKAN_VULKAN_H_
#define CORE_VULKAN_VULKAN_H_

#include "../Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

/*****************************************************************************
 * The Vulkan class manages resources required for Vulkan
 *****************************************************************************/

class Vulkan {
private:
	/* The actual instance handled by this class */
	static VkInstance instance;

	/* Debug messenger */
	static VkDebugUtilsMessengerEXT debugMessenger;

	/* The surface used for rendering to the window */
	static VkSurfaceKHR windowSurface;

	/* The device instance */
	static VulkanDevice* device;

	/* The swap chain */
	static VulkanSwapChain* swapChain;

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
	/* States whether validation layers are enabled or not */
	static bool ENABLE_VALIDATION_LAYERS;

	/* Method to initialise everything required for Vulkan - returns if this was successful */
	static bool initialise(Window* window);

	/* Method to destroy everything required for Vulkan */
	static void destroy();

	/* Method to create the Vulkan instance (Returns whether the creation was successful */
	static bool createInstance();

	/* Method to destroy the Vulkan instance */
	static void destroyInstance();

	/* Method to create a debug messenger for displaying output for validation layers */
	static void createDebugMessenger();

	/* Method to destroy the debug messenger */
	static void destroyDebugMessenger();

	/* Method to create the window surface (Returns whether the creation was successful) */
	static bool createWindowSurface(Window* window);

	/* Method to destroy the window surface */
	static void destroyWindowSurface();

	/* Method to obtain the Vulkan instance */
	static inline VkInstance& getInstance() { return instance; }

	/* Method to obtain the window surface */
	static inline VkSurfaceKHR& getWindowSurface() { return windowSurface; }
};


#endif /* CORE_VULKAN_VULKAN_H_ */
