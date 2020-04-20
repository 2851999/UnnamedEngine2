/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "RenderPass.h"

#include <array>

#include "../../utils/Logging.h"

 /*****************************************************************************
  * The RenderPass class
  *****************************************************************************/

RenderPass::RenderPass() {
	//Create the textures
	colourTexture = new Texture();
	colourTexture->setupVk(Vulkan::getSwapChain()->getExtent().width, Vulkan::getSwapChain()->getExtent().height, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	depthTexture = new Texture();
	depthTexture->setupVk(Vulkan::getSwapChain()->getExtent().width, Vulkan::getSwapChain()->getExtent().height, Vulkan::getSwapChain()->getDepthFormat(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

	//Setup the colour attachment info
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format = Vulkan::getSwapChain()->getSurfaceFormat();
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colourAttachmentRef = {};
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Setup the depth attachment info
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = Vulkan::getSwapChain()->getDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	//Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//The attachment descriptions
	std::vector<VkAttachmentDescription> attachments = { colourAttachment, depthAttachment };

	//Create the render pass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(Vulkan::getDevice()->getLogical(), &renderPassInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
		Logger::log("Failed to create render pass", "RenderPass", LogType::Error);

	//The framebuffer attachments
	std::vector<VkImageView> framebufferAttachments = { colourTexture->getVkImageView(), depthTexture->getVkImageView() };

	//Create the framebuffer
	VkFramebufferCreateInfo framebufferCreateInfo = {};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.renderPass = vulkanInstance;
	framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(framebufferAttachments.size());
	framebufferCreateInfo.pAttachments = framebufferAttachments.data();
	framebufferCreateInfo.width = Vulkan::getSwapChain()->getExtent().width;
	framebufferCreateInfo.height = Vulkan::getSwapChain()->getExtent().height;
	framebufferCreateInfo.layers = 1;

	if (vkCreateFramebuffer(Vulkan::getDevice()->getLogical(), &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS)
		Logger::log("Failed to create framebuffer", "nRenderPass", LogType::Error);
}

RenderPass::~RenderPass() {
	//Destroy created Vulkan objects
	if (vulkanInstance != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(Vulkan::getDevice()->getLogical(), framebuffer, nullptr);
		vkDestroyRenderPass(Vulkan::getDevice()->getLogical(), vulkanInstance, nullptr);
	}
	//delete colourTexture;
	//delete depthTexture;
}

void RenderPass::begin() {
	Vulkan::setCurrentRenderPass(vulkanInstance);

	//Begin the render pass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vulkanInstance;
	renderPassInfo.framebuffer = framebuffer;

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = Vulkan::getSwapChain()->getExtent();

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 }; //1.0 is far view plane, 0.0 is near view plane

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(Vulkan::getCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end() {
	//End the render pass
	vkCmdEndRenderPass(Vulkan::getCurrentCommandBuffer());
}
