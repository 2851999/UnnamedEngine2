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

#include "VulkanRenderPass.h"

#include "Vulkan.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The VulkanRenderPass class
 *****************************************************************************/

VulkanRenderPass::VulkanRenderPass(VulkanSwapChain* swapChain) {
	this->device = swapChain->getDevice();

	//Setup the render pass

	//Setup the colour attachment info
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format         = swapChain->getFormat();
	colourAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear before rendering
	colourAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE; //After rendering store so can display
	colourAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colourAttachmentRef = {};
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Setup the depth attachment info
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format         = Vulkan::findDepthFormat();
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount    = 1;
	subpass.pColorAttachments       = &colourAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass    = 0;
	dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colourAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments    = attachments.data();
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies   = &dependency;

	//Attempt to create the render pass
	if (vkCreateRenderPass(device->getLogical(), &renderPassInfo, nullptr, &instance) != VK_SUCCESS)
		Logger::log("Failed to create render pass", "VulkanRenderPass", LogType::Error);

	//Now need to create framebuffers, one for each swap chain image
	swapChainFramebuffers.resize(swapChain->getImageCount());

	//Obtain the image extent
	VkExtent2D& extent = swapChain->getExtent();

	for (unsigned int i = 0; i < swapChain->getImageCount(); ++i) {
		//Setup the creation info for the framebuffer
		std::array<VkImageView, 2> framebufferAttachments = {
			swapChain->getImageView(i),
			swapChain->getDepthImageView() //Only single subpass will be running at same time (due to semaphores) so can use the same depth image
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass      = instance;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(framebufferAttachments.size());
		framebufferCreateInfo.pAttachments    = framebufferAttachments.data();
		framebufferCreateInfo.width           = extent.width;
		framebufferCreateInfo.height          = extent.height;
		framebufferCreateInfo.layers          = 1;

		if (vkCreateFramebuffer(device->getLogical(), &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			Logger::log("Failed to create framebuffer", "VulkanRenderPass", LogType::Error);

	}
}

VulkanRenderPass::~VulkanRenderPass() {
	//Destroy the framebuffers
	for (auto framebuffer : swapChainFramebuffers)
    	vkDestroyFramebuffer(device->getLogical(), framebuffer, nullptr);

	//Destroy the render pass
	vkDestroyRenderPass(device->getLogical(), instance, nullptr);
}
