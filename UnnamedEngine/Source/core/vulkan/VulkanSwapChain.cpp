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

#include "VulkanSwapChain.h"

#include <limits>
#include <algorithm>

#include "Vulkan.h"
#include "../render/RenderPass.h"
#include "../render/Framebuffer.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The VulkanSwapChain class
 *****************************************************************************/

VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, Settings& settings) {
	this->device = device;

	//Obtain the swap chain support details for the device being used
	VulkanDeviceSwapChainSupportDetails swapChainSupportDetails = VulkanDevice::querySwapChainSupport(device->getPhysical());

	//Choose a surface format, present mode and extent
	VkSurfaceFormatKHR swapSurfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
	VkPresentModeKHR   presentMode       = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
	extent                               = chooseSwapExtent(swapChainSupportDetails.capabilities, settings);

	//Assign the VSync setting based on what is being used
	Window::getCurrentInstance()->getSettings().videoVSync = (presentMode == VK_PRESENT_MODE_FIFO_KHR);

	//Obtain the queue family indices
	VulkanDeviceQueueFamilies queueFamilies = device->getQueueFamilies();

	//Have to decide number of images to have in swap chain
	//Recommended to use one more than the minimum to have another image ready so don't have to wait for driver to complete previous one
	uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

	//Ensure this number does not exceed maximum (0 indicates no maximum)
	if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount)
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;

	//Creation info for swap chain instance
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = Vulkan::getWindowSurface();
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = swapSurfaceFormat.format;
	createInfo.imageColorSpace  = swapSurfaceFormat.colorSpace;
	createInfo.imageExtent      = extent;
	createInfo.imageArrayLayers = 1; //Number of layers each image consists of (always 1 unless VR/stereoscopic 3D)
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamilyIndex, queueFamilies.presentFamilyIndex };

	if (queueFamilies.graphicsFamilyIndex != queueFamilies.presentFamilyIndex) {
		//Use concurrent mode - not as fast but saves changing ownership (most hardware will have same queue families anyway)
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //Optional
		createInfo.pQueueFamilyIndices   = nullptr; //Optional
	}

	//Ensure no transform is applied to images in the swap chain
	createInfo.preTransform   = swapChainSupportDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //Ignore alpha channel for compositing with other surfaces
	createInfo.presentMode    = presentMode;
	createInfo.clipped        = VK_TRUE; //Don't care about pixels obscured by another window
	createInfo.oldSwapchain   = VK_NULL_HANDLE;

	//Attempt to create the swap chain
	if (vkCreateSwapchainKHR(device->getLogical(), &createInfo, nullptr, &instance) != VK_SUCCESS)
		Logger::log("Failed to create swap chain", "VulkanSwapchain", LogType::Error);

	//Assign the chosen format and extent
	this->surfaceFormat = swapSurfaceFormat.format;
	this->extent = extent;

	//Obtain the images in the created swap chain
	vkGetSwapchainImagesKHR(device->getLogical(), instance, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device->getLogical(), instance, &imageCount, images.data());

	//Create the image views
	imageViews.resize(images.size());

	for (unsigned int i = 0; i < images.size(); ++i)
		//Create the image view
		imageViews[i] = Vulkan::createImageView(images[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);

	//Obtain the number of samples being used
	numSamples = settings.videoSamples;
	//Now setup the colour buffer if necessary
	if (numSamples > 0) {
		Vulkan::createImage(extent.width, extent.height, 1, 1, static_cast<VkSampleCountFlagBits>(numSamples), surfaceFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colourImage, colourImageMemory);
		colourImageView = Vulkan::createImageView(colourImage, VK_IMAGE_VIEW_TYPE_2D, surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);

		Vulkan::transitionImageLayout(colourImage, surfaceFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);
	}

	//Now setup the depth buffer
	depthFormat = Vulkan::findDepthFormat();
	Vulkan::createImage(extent.width, extent.height, 1, 1, static_cast<VkSampleCountFlagBits>(numSamples == 0 ? 1 : numSamples), depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = Vulkan::createImageView(depthImage, VK_IMAGE_VIEW_TYPE_2D, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);
	Vulkan::transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);

	//Assign the default attachment descriptions

	//Setup the colour attachment info
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format = surfaceFormat;
	colourAttachment.samples = static_cast<VkSampleCountFlagBits>(numSamples == 0 ? 1 : numSamples);
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear before rendering
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //After rendering store so can display
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout = numSamples > 0 ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Setup the depth attachment info
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = depthFormat;
	depthAttachment.samples = static_cast<VkSampleCountFlagBits>(numSamples == 0 ? 1 : numSamples);
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Assign the attachments
	defaultAttachmentDescriptions = { colourAttachment, depthAttachment };

	//Check whether using MSAA
	if (numSamples > 0) {
		//Also need to resolve colour attachment (which should be in VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) before displaying
		VkAttachmentDescription colourAttachmentResolve = {};
		colourAttachmentResolve.format = surfaceFormat;
		colourAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colourAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colourAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colourAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colourAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colourAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colourAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		defaultAttachmentDescriptions.push_back(colourAttachmentResolve);
	}
}

VulkanSwapChain::~VulkanSwapChain() {
	//Destory default framebuffers
	for (Framebuffer* framebuffer : defaultFramebuffers)
		delete framebuffer;

	//Destroy image views and the swap chain
	for (auto& imageView : imageViews)
		vkDestroyImageView(device->getLogical(), imageView, nullptr);
	vkDestroySwapchainKHR(device->getLogical(), instance, nullptr);

	if (numSamples > 0) {
		vkDestroyImageView(device->getLogical(), colourImageView, nullptr);
		vkDestroyImage(Vulkan::getDevice()->getLogical(), colourImage, nullptr);
		vkFreeMemory(Vulkan::getDevice()->getLogical(), colourImageMemory, nullptr);
	}

	vkDestroyImageView(device->getLogical(), depthImageView, nullptr);
    vkDestroyImage(Vulkan::getDevice()->getLogical(), depthImage, nullptr);
    vkFreeMemory(Vulkan::getDevice()->getLogical(), depthImageMemory, nullptr);
}

void VulkanSwapChain::setupDefaultFramebuffers(RenderPass* defaultRenderPass) {
	//Create one framebuffer per swap chain image
	for (unsigned int i = 0; i < images.size(); ++i) {
		//Required framebuffer attachements
		std::vector<VkImageView> framebufferAttachments;

		//Check for MSAA
		if (numSamples > 0) {
			framebufferAttachments.push_back(getColourImageView());
			framebufferAttachments.push_back(getDepthImageView());
			framebufferAttachments.push_back(getImageView(i));
		} else {
			framebufferAttachments.push_back(getImageView(i));
			framebufferAttachments.push_back(getDepthImageView());
		}

		//Create and add the framebuffer
		defaultFramebuffers.push_back(new Framebuffer(defaultRenderPass->getVkInstance(), extent.width, extent.height, framebufferAttachments, false));
	}
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	//Check if no preferred format
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		//Return the default
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	else {
		//Need to find the preferred format that is available
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}
	}

	//Otherwise just use the first available
	return availableFormats[0];
}

bool VulkanSwapChain::isPresentModeAvailable(VkPresentModeKHR presentMode, const std::vector<VkPresentModeKHR>& availablePresentModes) {
	//Go through the available present modes
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == presentMode)
			//Mode is available
			return true;
	}

	//Mode is not available
	return false;
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	//Obtain the requested mode
	VkPresentModeKHR requestedMode;
	if (Window::getCurrentInstance()->getSettings().videoVSync == 0)
		requestedMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	else if (Window::getCurrentInstance()->getSettings().videoVSync == 2)
		requestedMode = VK_PRESENT_MODE_MAILBOX_KHR;
	else
		requestedMode = VK_PRESENT_MODE_FIFO_KHR;

	//Note: VK_PRESENT_MODE_FIFO_KHR and therefore VSync should always be available

	//Check if the requested mode is available
	if (isPresentModeAvailable(requestedMode, availablePresentModes))
		return requestedMode;
	else {
		//Present mode was not available, attempt to find alternative
		if (requestedMode == VK_PRESENT_MODE_FIFO_KHR)
			Logger::log("VK_PRESENT_MODE_FIFO_KHR is not supported when it should be", "Vulkan", LogType::Error);
		else if (requestedMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			//Try triple buffering instead (since also doesn't limit frame rate)
			if (isPresentModeAvailable(VK_PRESENT_MODE_MAILBOX_KHR, availablePresentModes)) {
				Logger::log("VK_PRESENT_MODE_IMMEDIATE_KHR is not supported so using VK_PRESENT_MODE_MAILBOX_KHR instead", "Vulkan", LogType::Information);
				return VK_PRESENT_MODE_MAILBOX_KHR;
			} else {
				Logger::log("VK_PRESENT_MODE_IMMEDIATE_KHR is not supported so using VK_PRESENT_MODE_FIFO_KHR instead", "Vulkan", LogType::Information);
				return VK_PRESENT_MODE_FIFO_KHR;
			}
		} else if (requestedMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			//Fall back to double buffering
			Logger::log("VK_PRESENT_MODE_MAILBOX_KHR is not supported so using VK_PRESENT_MODE_FIFO_KHR instead", "Vulkan", LogType::Information);
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	return requestedMode;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Settings& settings) {
	//If size is set to this maximum value then surface size will be determined by the extent of a swap chain targeting the surface
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		//Keep the same as the surface
		return capabilities.currentExtent;
	else {
		//Make as close to window size as possible, then surface will match this
		VkExtent2D actualExtent = { static_cast<uint32_t>(settings.windowWidth), static_cast<uint32_t>(settings.windowHeight) };

		//Clamp value between maximum and minimum supported by implementation
		actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width,  actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
