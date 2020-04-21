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

class Framebuffer;
class RenderPass;

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

	/* The formats and extent used for this swap chain */
	VkFormat   surfaceFormat;
	VkFormat   depthFormat;
	VkExtent2D extent;

	/* The device this swap chain is for */
	VulkanDevice* device;

	/* Default framebuffers for rendering to the swap chain using the
	   engines default RenderPass (requires one per swap chain image) */
	std::vector<Framebuffer*> defaultFramebuffers;

	/* Default attachment descriptions for rendering to the default framebuffer */
	std::vector<VkAttachmentDescription> defaultAttachmentDescriptions;

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

	/* Method to setup the default framebuffers given the default render pass */
	void setupDefaultFramebuffers(RenderPass* defaultRenderPass);

	/* Returns the attachments required for rendering to the default framebuffer */
	inline std::vector<VkAttachmentDescription>& getDefaultAttachmentDescriptions() { return defaultAttachmentDescriptions; }

	/* Getters */
	inline VkSwapchainKHR& getInstance() { return instance; }
	inline VkImageView& getImageView(unsigned int index) { return imageViews[index]; }
	inline unsigned int getNumSamples() { return numSamples; }
	inline VkImageView& getColourImageView() { return colourImageView; }
	inline VkImageView& getDepthImageView() { return depthImageView; }
	inline VkFormat getSurfaceFormat() { return surfaceFormat; }
	inline VkFormat getDepthFormat() { return depthFormat; }
	inline VkExtent2D& getExtent() { return extent; }
	inline VulkanDevice* getDevice() { return device; }
	inline unsigned int getImageCount() { return images.size(); }
	inline Framebuffer* getDefaultFramebuffer(unsigned int currentFrame) { return defaultFramebuffers[currentFrame]; }
};

