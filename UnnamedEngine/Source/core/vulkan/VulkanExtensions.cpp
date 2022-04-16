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

#include "VulkanExtensions.h"

#include "Vulkan.h"

#include "../Window.h"

#include <set>

/*****************************************************************************
 * The VulkanExtensions class
 *****************************************************************************/

std::vector<const char*> VulkanExtensions::requiredExtensions;

std::vector<const char*> VulkanExtensions::requiredDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> VulkanExtensions::requiredRaytracingExtensions = {
	//Ray tracing extensions
	VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
	VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
	//Required by VK_KHR_acceleration_structure
	VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	//Required by VK_KHR_ray_tracing_pipeline
	VK_KHR_SPIRV_1_4_EXTENSION_NAME,
	//Required by VK_KHR_spirv_1_4,
	VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
};

void VulkanExtensions::addRequired() {
	//Obtain the extensions required by GLFW
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Assign the extensions
	requiredExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (Window::getCurrentInstance()->getSettings().videoRaytracing)
		requiredDeviceExtensions.insert(std::end(requiredDeviceExtensions), std::begin(requiredRaytracingExtensions), std::end(requiredRaytracingExtensions));

	//Check if validation layers are also required
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled)
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void VulkanExtensions::addRequiredExtension(const char* extension) {
	requiredExtensions.push_back(extension);
}

bool VulkanExtensions::checkSupport(VkPhysicalDevice device) {
	//Get the available extensions for the device
	uint32_t deviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);

	std::vector<VkExtensionProperties> availbleDeviceExtensions(deviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, availbleDeviceExtensions.data());

	//Create set of required extensions and remove ones that are available (check list)
	std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

	for (const auto& deviceExtension : availbleDeviceExtensions)
		requiredExtensions.erase(deviceExtension.extensionName);

	return requiredExtensions.empty();
}
