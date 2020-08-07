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

#include "Framebuffer.h"

#include "../../utils/Logging.h"

 /*****************************************************************************
  * The Framebuffer class
  *****************************************************************************/

Framebuffer::Framebuffer(VkRenderPass renderPass, uint32_t width, uint32_t height, std::vector<VkImageView> attachments, bool cubemap) {
	//Create the framebuffer
	VkFramebufferCreateInfo framebufferCreateInfo = {};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.renderPass = renderPass;
	framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferCreateInfo.pAttachments = attachments.data();
	framebufferCreateInfo.width = width;
	framebufferCreateInfo.height = height;
	framebufferCreateInfo.layers = cubemap ? 6 : 1;

	if (vkCreateFramebuffer(Vulkan::getDevice()->getLogical(), &framebufferCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
		Logger::log("Failed to create framebuffer", "Framebuffer", LogType::Error);
}

Framebuffer::~Framebuffer() {
	//Destroy the framebuffer
	vkDestroyFramebuffer(Vulkan::getDevice()->getLogical(), vulkanInstance, nullptr);
}
