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

#include "VulkanDevice.h"

#include "Vulkan.h"
#include "VulkanExtensions.h"
#include "VulkanValidationLayers.h"
#include "../../utils/Logging.h"

#include <set>

/*****************************************************************************
 * The VulkanDevice class
 *****************************************************************************/

VulkanDevice::VulkanDevice(VkPhysicalDevice& physicalDevice) {
	this->physicalDevice = physicalDevice;

	//Obtain the device queue families
	this->queueFamilies = findQueueFamilies(physicalDevice);

	//Create the logical device
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilies.graphicsFamilyIndex, queueFamilies.presentFamilyIndex }; //Likely these are identical, so only one one in set

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		//Setup the queue create info
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	//Request anisotropic filtering (support must be checked in 'isDeviceSuitable')
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	//Obtain the required validation layers and device extensions
	std::vector<const char*>& validationLayers = VulkanValidationLayers::getLayers();
	std::vector<const char*>& deviceExtensions = VulkanExtensions::getDeviceExtentions();

	//Info for logical device creation
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos       = queueCreateInfos.data();
	createInfo.pEnabledFeatures 	   = &deviceFeatures;
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	//Not required in recent Vulkan implementations (as no distinction between instance and device validation layers)
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled) {
		createInfo.enabledLayerCount   = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else
		createInfo.enabledLayerCount   = 0;
	//---------------------------------------------------------------------------------------------------------------

	//Create the logical device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
		Logger::log("Failed to create a logical device", "VulkanDevice", LogType::Error);

	//Obtain the device queues requested
	vkGetDeviceQueue(logicalDevice, queueFamilies.graphicsFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilies.presentFamilyIndex,  0, &presentQueue );
}

VulkanDevice::~VulkanDevice() {
	//Device queues also cleaned up when device is destroyed

	//Destroy the device
	vkDestroyDevice(logicalDevice, nullptr);
}

VulkanDevice* VulkanDevice::create() {
	//The device instance to return
	VulkanDevice* deviceInstance = NULL;

	//Obtain a list of physical devices
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(Vulkan::getInstance(), &physicalDeviceCount, nullptr);

	//Check there are devices available
	if (physicalDeviceCount > 0) {
		VkPhysicalDevice chosenPhysicalDevice = VK_NULL_HANDLE;

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(Vulkan::getInstance(), &physicalDeviceCount, physicalDevices.data());

		//Go through the devices and pick the first suitable one
		for (const auto& physicalDevice : physicalDevices) {
			if (isDeviceSuitable(physicalDevice)) {
				chosenPhysicalDevice = physicalDevice;
				break;
			}
		}

		//Check a physical device was found
		if (chosenPhysicalDevice != VK_NULL_HANDLE)
			//Create the device
			deviceInstance = new VulkanDevice(chosenPhysicalDevice);
		else
			Logger::log("Failed to find a suitable physical device", "VulkanDevice", LogType::Error);
	} else
		Logger::log("Failed to find any physical devices", "VulkanDevice", LogType::Error);

	//Return the device instance
	return deviceInstance;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) {
	//Obtain the device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//Obtain supported queue families
	VulkanDeviceQueueFamilies queueFamilies = findQueueFamilies(device);

	//Check extensions are supported
	bool extensionsSupported = VulkanExtensions::checkSupport(device);

	//Check swap chain support of the device
	bool swapChainSupportAdequate = false;

	if (extensionsSupported) {
		VulkanDeviceSwapChainSupportDetails swapChainSupportDetails = querySwapChainSupport(device);
		swapChainSupportAdequate = (! swapChainSupportDetails.formats.empty()) && (! swapChainSupportDetails.presentModes.empty());
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	//Return the result
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && queueFamilies.isComplete() && extensionsSupported && swapChainSupportAdequate && supportedFeatures.samplerAnisotropy;
}

VulkanDeviceQueueFamilies VulkanDevice::findQueueFamilies(VkPhysicalDevice device) {
	VulkanDeviceQueueFamilies queueFamilies;

	//Obtain queue families supported by the device
	uint32_t availableQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &availableQueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> availableQueueFamilies(availableQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &availableQueueFamilyCount, availableQueueFamilies.data());

	//Go through the available queue families for this device
	for (uint32_t i = 0; i < availableQueueFamilyCount; ++i) {
		//Check for present support (rendering to a surface)
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Vulkan::getWindowSurface(), &presentSupport);

		//Check and assign queue family with present/graphics support
		if (availableQueueFamilies[i].queueCount > 0 && presentSupport)
			queueFamilies.asssignPresentFamily(i);
		if (availableQueueFamilies[i].queueCount > 0 && (availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			queueFamilies.assignGraphicsFamily(i);

		//Stop if all required families have been found
		if (queueFamilies.isComplete())
			break;
	}

	//Return the found info
	return queueFamilies;
}

VulkanDeviceSwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) {
	VulkanDeviceSwapChainSupportDetails details;

	//The window surface being used
	VkSurfaceKHR windowSurface = Vulkan::getWindowSurface();

	//Obtain the surface capabilities of the device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface, &details.capabilities);

	//Obtain the supported surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, nullptr);
	if (formatCount > 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, details.formats.data());
	}

	//Obtain the supported present modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, nullptr);
	if (presentModeCount > 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, details.presentModes.data());
	}

	//Return the support details
	return details;
}
