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

#include "FBO.h"

#include "RenderPass.h"

 /*****************************************************************************
  * The FramebufferAttachment class
  *****************************************************************************/

FramebufferAttachment::FramebufferAttachment(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout finalLayout) : Texture(), format(format), finalLayout(finalLayout) {
	//Setup the texture for Vulkan
	setupVk(width, height, format, usage, aspectMask);
}

FramebufferAttachment::~FramebufferAttachment() {

}

VkAttachmentDescription FramebufferAttachment::getVkAttachmentDescription() {
	//Create and return the structure
	VkAttachmentDescription description = {};
	description.format = format;
	description.samples = VK_SAMPLE_COUNT_1_BIT;
	description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //Might want to allow this to change to VK_ATTACHMENT_STORE_OP_DONT_CARE when not needed e.g. depth
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = finalLayout;

	return description;
}

 /*****************************************************************************
  * The FBO class
  *****************************************************************************/

FBO::FBO(uint32_t width, uint32_t height, std::vector<FramebufferAttachment*> attachments) : width(width), height(height), attachments(attachments) {
	//Go through each attachment and add its description
	for (unsigned int i = 0; i < attachments.size(); ++i)
		vulkanAttachmentDescriptions.push_back(attachments[i]->getVkAttachmentDescription());
}

FBO::~FBO() {
	//Delete the framebuffer
	delete framebuffer;
	//Delete all of the attachments
	for (FramebufferAttachment* attachment : attachments)
		delete attachment;
}

void FBO::setup(RenderPass* renderPass) {
	//The framebuffer attachments' image views
	std::vector<VkImageView> framebufferAttachments;

	//Go through each attachment
	for (unsigned int i = 0; i < attachments.size(); ++i)
		//Add the attachment and its description to the lists
		framebufferAttachments.push_back(attachments[i]->getVkImageView());

	//Create the framebuffer
	framebuffer = new Framebuffer(renderPass->getVkInstance(), width, height, framebufferAttachments);
}