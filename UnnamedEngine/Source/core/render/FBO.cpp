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
#include "../BaseEngine.h"
#include "../../utils/Logging.h"

 /*****************************************************************************
  * The FramebufferAttachment class
  *****************************************************************************/

FramebufferAttachment::FramebufferAttachment(uint32_t width, uint32_t height, Type type) : Texture(), type(type) {
	setWidth(width);
	setHeight(height);
}

FramebufferAttachment::~FramebufferAttachment() {
	if (! BaseEngine::usingVulkan()) {
		if (getParameters().getTarget() == GL_RENDERBUFFER)
			glDeleteRenderbuffers(1, &glRBO);
	}
}

void FramebufferAttachment::setup(unsigned int indexOfColourAttachment) {
	//Check whether using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Setup the texture for Vulkan

		VkImageUsageFlags usage;
		VkImageAspectFlags aspectMask;
		VkImageLayout imageLayout;

		if (type == Type::COLOUR_TEXTURE) {
			vulkanFormat = Vulkan::getSwapChain()->getSurfaceFormat();
			usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		} else if (type == Type::DEPTH_TEXTURE) {
			vulkanFormat = Vulkan::getSwapChain()->getDepthFormat();
			usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		} else if (type == Type::DEPTH) {
			vulkanFormat = Vulkan::getSwapChain()->getDepthFormat();
			usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			aspectMask = Vulkan::hasStencilComponent(Vulkan::getSwapChain()->getDepthFormat()) ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_DEPTH_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		setupVk(getWidth(), getHeight(), vulkanFormat, usage, aspectMask, imageLayout);
	} else {
		GLint internalFormat;
		GLenum format;
		GLenum glType;
		GLenum attachment;

		if (type == Type::COLOUR_TEXTURE) {
			getParameters().setTarget(GL_TEXTURE_2D);
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			glType = GL_FLOAT;
			attachment = GL_COLOR_ATTACHMENT0 + indexOfColourAttachment;

			getParameters().setFilter(GL_NEAREST);
			getParameters().setClamp(GL_CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_TEXTURE) {
			getParameters().setTarget(GL_TEXTURE_2D);
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			glType = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;

			getParameters().setFilter(GL_LINEAR);
			getParameters().setClamp(GL_CLAMP_TO_BORDER);
		} else if (type == Type::DEPTH) {
			getParameters().setTarget(GL_RENDERBUFFER);
			internalFormat = GL_DEPTH_COMPONENT32;
			format = GL_DEPTH_COMPONENT;
			glType = GL_UNSIGNED_INT;
			attachment = GL_DEPTH_ATTACHMENT;

			getParameters().setFilter(GL_NEAREST);
			getParameters().setClamp(GL_CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_CUBEMAP) {
			getParameters().setTarget(GL_TEXTURE_CUBE_MAP);
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			glType = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;

			getParameters().setFilter(GL_NEAREST);
			getParameters().setClamp(GL_CLAMP_TO_EDGE);
		}

		if (type != Type::DEPTH_CUBEMAP) {
			//Check for render buffer object
			if (getParameters().getTarget() == GL_RENDERBUFFER) {
				//Setup the render buffer
				glGenRenderbuffers(1, &glRBO);
				glBindRenderbuffer(getParameters().getTarget(), glRBO);
				//if (multisample)
				//	glRenderbufferStorageMultisample(getParameters().getTarget(), Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight());
				//else
				glRenderbufferStorage(getParameters().getTarget(), internalFormat, getWidth(), getHeight());
				glBindRenderbuffer(getParameters().getTarget(), 0);

				//Attach to framebuffer
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), glRBO);
			} else {
				//Setup the texture
				create();
				bind();
				//if (multisample)
				//	glTexImage2DMultisample(getParameters().getTarget(), Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
				//else
				glTexImage2D(getParameters().getTarget(), 0, internalFormat, getWidth(), getHeight(), 0, format, glType, NULL);

				applyParameters(false);

				//Attach to framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), getHandle(), 0);
				unbind();
			}
		} else {
			//Setup the texture
			create();
			bind();

			for (unsigned int i = 0; i < 6; ++i) {
				//if (multisample)
				//	glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
				//else
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, getWidth(), getHeight(), 0, format, glType, NULL);
			}

			applyParameters(false, false);

			//Attach to framebuffer
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, getHandle(), 0);
			unbind();
		}
	}
}

VkAttachmentDescription FramebufferAttachment::getVkAttachmentDescription() {
	//Create and return the structure
	VkAttachmentDescription description = {};
	description.format = vulkanFormat;
	description.samples = VK_SAMPLE_COUNT_1_BIT;
	description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //Might want to allow this to change to VK_ATTACHMENT_STORE_OP_DONT_CARE when not needed e.g. depth
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = vulkanFinalLayout;

	return description;
}

 /*****************************************************************************
  * The FBO class
  *****************************************************************************/

FBO::FBO(uint32_t width, uint32_t height, std::vector<FramebufferAttachment*> attachments) : width(width), height(height), attachments(attachments) {
	//Setup each attachment
	unsigned int index = 0;
	for (unsigned int i = 0; i < attachments.size(); ++i) {
		//Setup the current attachment
		attachments[i]->setup(index);

		if (attachments[i]->getType() == FramebufferAttachment::Type::COLOUR_TEXTURE)
			index++;
	}

	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Go through each attachment and add its description
		for (unsigned int i = 0; i < attachments.size(); ++i)
			vulkanAttachmentDescriptions.push_back(attachments[i]->getVkAttachmentDescription());
	}
}

FBO::~FBO() {
	//Delete the framebuffer
	if (framebuffer)
		delete framebuffer;
	if (! BaseEngine::usingVulkan())
		glDeleteFramebuffers(1, &glFBO);
	//Delete all of the attachments
	for (FramebufferAttachment* attachment : attachments)
		delete attachment;
}

void FBO::setup(RenderPass* renderPass) {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		//The framebuffer attachments' image views
		std::vector<VkImageView> framebufferAttachments;

		//Go through each attachment
		for (unsigned int i = 0; i < attachments.size(); ++i)
			//Add the attachment and its description to the lists
			framebufferAttachments.push_back(attachments[i]->getVkImageView());

		//Create the framebuffer
		framebuffer = new Framebuffer(renderPass->getVkInstance(), width, height, framebufferAttachments);
	} else {
		//Generate and bind the FBO
		glGenFramebuffers(1, &glFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, glFBO);

		//List of all of the colour attachments being used
		std::vector<unsigned int> colourAttachments;

		unsigned int index = 0;

		//Go though each attached FramebufferAttachment
		for (unsigned int i = 0; i < attachments.size(); i++) {
			//Setup the current attachment
			attachments[i]->setup(index);

			//Assume that if it is not a depth attachment then it is a colour attachment
			if (attachments[i]->getType() == FramebufferAttachment::Type::COLOUR_TEXTURE) {
				colourAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
				index++;
			}
		}

		if (colourAttachments.size() == 0) {
			//No colour attachment
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		} else
			glDrawBuffers(colourAttachments.size(), colourAttachments.data());

		//Check to see whether the setup was successful
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			Logger::log("Framebuffer is not complete, current status: " + utils_string::str(status), "FramebufferObject", LogType::Error);

		//Bind the default FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}