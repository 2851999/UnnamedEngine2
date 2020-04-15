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

#include "VulkanDevice.h"

/*****************************************************************************
 * The VulkanSwapChain class handles the image swap chain for rendering using
 * Vulkan
 *****************************************************************************/

class VulkanSwapChain {
private:
	/* The swap chain instance */
	VkSwapchainKHR instance;

	/* The images, image views and frame buffers in this swap chain */
	std::vector<VkImage>       images;
	std::vector<VkImageView>   imageViews;

	/* States number of MSAA samples being used */
	unsigned int numSamples;

	/* Resources for the colour buffer (used for MSAA) */
	VkImage        colourImage;
	VkDeviceMemory colourImageMemory;
	VkImageView    colourImageView;

	/* Resources for the depth buffer */
	VkImage        depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView    depthImageView;

	/* The format and extent used for this swap chain */
	VkFormat   format;
	VkExtent2D extent;

	/* The device this swap chain is for */
	VulkanDevice* device;

	/* Methods used to choose a surface format, present mode and extent based on what is supported */
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static bool               isPresentModeAvailable (VkPresentModeKHR presentMode, const std::vector<VkPresentModeKHR>&   availablePresentModes);
	static VkPresentModeKHR   chooseSwapPresentMode  (const std::vector<VkPresentModeKHR>&   availablePresentModes);
	static VkExtent2D         chooseSwapExtent       (const VkSurfaceCapabilitiesKHR&        capabilities, Settings& settings);
public:
	/* Constructor */
	VulkanSwapChain(VulkanDevice* device, Settings& settings);

	/* Destructor */
	virtual ~VulkanSwapChain();

	/* Getters */
	inline VkSwapchainKHR& getInstance() { return instance; }
	inline VkImageView& getImageView(unsigned int index) { return imageViews[index]; }
	inline unsigned int getNumSamples() { return numSamples; }
	inline VkImageView& getColourImageView() { return colourImageView; }
	inline VkImageView& getDepthImageView() { return depthImageView; }
	inline VkFormat getFormat() { return format; }
	inline VkExtent2D& getExtent() { return extent; }
	inline VulkanDevice* getDevice() { return device; }
	inline unsigned int getImageCount() { return images.size(); }
};

