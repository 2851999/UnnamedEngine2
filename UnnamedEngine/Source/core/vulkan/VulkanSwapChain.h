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

#ifndef CORE_VULKAN_VULKANSWAPCHAIN_H_
#define CORE_VULKAN_VULKANSWAPCHAIN_H_

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
	std::vector<VkFramebuffer> framebuffers;

	/* The format and extent used for this swap chain */
	VkFormat   format;
	VkExtent2D extent;

	/* The device this swap chain is for */
	VulkanDevice* device;

	/* Methods used to choose a surface format, present mode and extent based on what is supported */
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR   chooseSwapPresentMode  (const std::vector<VkPresentModeKHR>&   availablePresentModes);
	static VkExtent2D         chooseSwapExtent       (const VkSurfaceCapabilitiesKHR&        capabilities, Settings& settings);
public:
	/* Constructor */
	VulkanSwapChain(VulkanDevice* device, Settings& settings);

	/* Destructor */
	virtual ~VulkanSwapChain();
};

#endif /* CORE_VULKAN_VULKANSWAPCHAIN_H_ */
