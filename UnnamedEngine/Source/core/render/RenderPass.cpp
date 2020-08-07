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

#include "../BaseEngine.h"
#include "../../utils/Logging.h"

 /*****************************************************************************
  * The RenderPass class
  *****************************************************************************/

RenderPass::RenderPass(FBO* fbo) : fbo(fbo) {
	//Check using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Depth attachment reference
		VkAttachmentReference depthStencilAttachmentReference = {};
		depthStencilAttachmentReference.attachment = VK_ATTACHMENT_UNUSED;
		depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		//Use subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies;

		//Colour attachment references
		std::vector<VkAttachmentReference> colourAttachmentReferences;

		//States whether there is a depth stencil attachment
		bool hasDepthStencilAttachment = false;

		//Check if an FBO instance was given
		if (fbo) {
			vulkanClearValues.resize(fbo->getAttachmentCount());

			//Go through all of the FBO attachments
			for (unsigned int i = 0; i < fbo->getAttachmentCount(); ++i) {
				//Obtain the type of the current FBO
				FramebufferAttachment::Type type = fbo->getAttachment(i)->getType();

				if (type == FramebufferAttachment::Type::DEPTH || type == FramebufferAttachment::Type::DEPTH_TEXTURE || type == FramebufferAttachment::Type::DEPTH_CUBEMAP) {
					if (hasDepthStencilAttachment)
						Logger::log("Cannot have more than one depth attachment in FBO", "RenderPass", LogType::Error);

					//Assign the depth stencil attachment reference
					depthStencilAttachmentReference.attachment = i;
					depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					vulkanClearValues[i].depthStencil = { 1.0f, 0 };
					hasDepthStencilAttachment = true;
				} else {
					//Create a new colour attachment reference
					VkAttachmentReference colourAttachmentReference;
					colourAttachmentReference.attachment = i;
					colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vulkanClearValues[i].color = { 0.0f, 0.0f, 0.0f, 0.0f };

					colourAttachmentReferences.push_back(colourAttachmentReference);
				}
			}

			if (hasDepthStencilAttachment && colourAttachmentReferences.size() == 0) {
				//Only have depth info assume shadow map

				dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[0].dstSubpass = 0;
				dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				dependencies[1].srcSubpass = 0;
				dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			} else {

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
				dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
				dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}
		} else {
			//Using default framebuffer directly
			dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass      = 0;
			dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass      = 0;
			dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkAttachmentReference colourAttachmentReference;
			colourAttachmentReference.attachment = 0;
			colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colourAttachmentReferences.push_back(colourAttachmentReference);

			depthStencilAttachmentReference.attachment = 1;
			depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			hasDepthStencilAttachment = true;

			vulkanClearValues.resize(2);
			vulkanClearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			vulkanClearValues[1].depthStencil = { 1.0f, 0 }; //1.0 is far view plane, 0.0 is near view plane
		}

		//Assign the attachments
		subpass.colorAttachmentCount = static_cast<uint32_t>(colourAttachmentReferences.size());
		if (subpass.colorAttachmentCount > 0)
			subpass.pColorAttachments = colourAttachmentReferences.data();
		if (hasDepthStencilAttachment)
			subpass.pDepthStencilAttachment = &depthStencilAttachmentReference;

		//Assign the number of colour attachments
		numColourAttachments = colourAttachmentReferences.size();

		//The attachment descriptions
		std::vector<VkAttachmentDescription> attachments;

		//Need to resolve colour attachment if using MSAA (try and keep in scope for vkCreateRenderPass)
		VkAttachmentReference colourAttachmentResolveRef = {};

		if (fbo)
			attachments = fbo->getVkAttachmentDescriptions();
		else {
			attachments = Vulkan::getSwapChain()->getDefaultAttachmentDescriptions();

			//Check for MSAA
			if (Vulkan::getSwapChain()->getNumSamples() > 0) {
				//Assign number of samples
				numSamples = Vulkan::getSwapChain()->getNumSamples();

				//Need to resolve colour attachment
				colourAttachmentResolveRef.attachment = 2;
				colourAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				subpass.pResolveAttachments = &colourAttachmentResolveRef;
			}
		}

		//Create the render pass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments    = attachments.data();
		renderPassInfo.subpassCount    = 1;
		renderPassInfo.pSubpasses      = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies   = dependencies.data();

		if (vkCreateRenderPass(Vulkan::getDevice()->getLogical(), &renderPassInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
			Logger::log("Failed to create render pass", "RenderPass", LogType::Error);
	}

	//Setup the FBO if there is one
	if (fbo)
		fbo->setup(this);
}

RenderPass::~RenderPass() {
	//Destroy created objects
	delete fbo;
	if (vulkanInstance != VK_NULL_HANDLE)
		vkDestroyRenderPass(Vulkan::getDevice()->getLogical(), vulkanInstance, nullptr);
}

void RenderPass::begin() {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Begin the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkanInstance;

		renderPassInfo.renderArea.offset = { 0, 0 };

		if (fbo) {
			renderPassInfo.framebuffer = fbo->getFramebuffer()->getVkInstance();
			renderPassInfo.renderArea.extent = { fbo->getWidth(), fbo->getHeight() };
		} else {
			renderPassInfo.framebuffer = Vulkan::getSwapChain()->getDefaultFramebuffer(Vulkan::getCurrentFrame())->getVkInstance(); //Default framebuffer
			renderPassInfo.renderArea.extent = Vulkan::getSwapChain()->getExtent();
		}

		renderPassInfo.clearValueCount = static_cast<uint32_t>(vulkanClearValues.size());
		renderPassInfo.pClearValues = vulkanClearValues.data();

		vkCmdBeginRenderPass(Vulkan::getCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	} else {
		//Bind the FBO if it exists
		if (fbo)
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->getGLFBO());
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0); //Use default framebuffer

		//Clear the contents
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void RenderPass::end() {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan())
		//End the render pass
		vkCmdEndRenderPass(Vulkan::getCurrentCommandBuffer());
}
