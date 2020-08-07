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

#pragma once

#include "../Window.h"

/* Structure used to contain information about queue families for a physical device */
struct VulkanDeviceQueueFamilies {
	/* Present and graphics queue family indices and a boolean stating whether they have been assigned */
	uint32_t presentFamilyIndex;
	bool     presentFamilyFound = false;

	uint32_t graphicsFamilyIndex;
	bool     graphicsFamilyFound = false;

	/* Methods to assign the above indices */
	void asssignPresentFamily(uint32_t presentFamily) {
		this->presentFamilyIndex = presentFamily;
		presentFamilyFound       = true;
	}

	void assignGraphicsFamily(uint32_t graphicsFamily) {
		this->graphicsFamilyIndex = graphicsFamily;
		graphicsFamilyFound       = true;
	}

	/* Returns whether all of the required queue families have been assigned */
	bool isComplete() {
		return presentFamilyFound && graphicsFamilyFound;
	}
};

/* Structure used to contain information about a devices swap chain support */
struct VulkanDeviceSwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	//Supported surface formats
	std::vector<VkSurfaceFormatKHR> formats;
	//Supported present modes
	std::vector<VkPresentModeKHR> presentModes;
};

/*****************************************************************************
 * The VulkanDevice class used to handle a physical and logical device for
 * Vulkan
 *****************************************************************************/

class VulkanDevice {
private:
	/* The physical and logical devices used by Vulkan */
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; //Destroyed with Vulkan instance
	VkDevice         logicalDevice;                   //Destroyed with instance of this class

	/* The queue families supported/used by this device */
	VulkanDeviceQueueFamilies queueFamilies;

	/* Present and graphics queues created with the device */
	VkQueue graphicsQueue;
	VkQueue presentQueue;
public:
	/* Constructor */
	VulkanDevice(VkPhysicalDevice& physicalDevice);

	/* Destructor */
	virtual ~VulkanDevice();

	/* Utility methods to find out more about this device */
	std::string listLimits();
	std::string listSupportedExtensions();

	/* Setters and getters */
	VkPhysicalDevice& getPhysical() { return physicalDevice; }
	VkDevice&         getLogical()  { return logicalDevice;  }
	VulkanDeviceQueueFamilies& getQueueFamilies() { return queueFamilies; }
	VkQueue& getGraphicsQueue()     { return graphicsQueue; }
	VkQueue& getPresentQueue()      { return presentQueue;  }

	/* Finds a suitable physical device and creates and returns a VulkanDevice instance using it */
	static VulkanDevice* create();

	/* Checks the queue family and swap chain support of a device and returns whether a device is suitable for the engine */
	static bool isDeviceSuitable(VkPhysicalDevice device);

	/* Method to find and return queue families for a physical device */
	static VulkanDeviceQueueFamilies findQueueFamilies(VkPhysicalDevice device);

	/* Method to check swap chain support and return data useful for its setup */
	static VulkanDeviceSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
};

