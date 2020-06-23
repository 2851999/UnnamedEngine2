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

FramebufferAttachment::FramebufferAttachment(uint32_t width, uint32_t height, Type type, TextureParameters textureParameters, unsigned int samples, unsigned int numStorageMipMaps) : Texture(textureParameters), type(type), samples(samples), numStorageMipMaps(numStorageMipMaps) {
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
	beenSetup = true;

	//Check whether using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Setup the texture for Vulkan

		VkImageUsageFlags usage;
		VkImageLayout imageLayout;

		if (type == Type::COLOUR_TEXTURE) {
			vulkanFormat      = VK_FORMAT_R16G16B16A16_SFLOAT; //Equivalent to OpenGL below - Should really check if supported first
			usage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			vulkanAspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::COLOUR_CUBEMAP) {
			vulkanFormat      = VK_FORMAT_R16G16B16A16_SFLOAT;
			usage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			vulkanAspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//getParameters().setFilter(TextureParameters::Filter::LINEAR); //Assigned in PBREnvironment currently
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_TEXTURE) {
			vulkanFormat      = Vulkan::findDepthFormat();
			usage             = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			vulkanAspectMask  = VK_IMAGE_ASPECT_DEPTH_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			//getParameters().setFilter(TextureParameters::Filter::LINEAR);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_BORDER);
		} else if (type == Type::DEPTH) {
			vulkanFormat	  = Vulkan::findDepthFormat();
			usage             = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			vulkanAspectMask  = Vulkan::hasStencilComponent(vulkanFormat) ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_DEPTH_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_CUBEMAP) {
			vulkanFormat      = Vulkan::findDepthFormat();
			usage             = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			vulkanAspectMask  = VK_IMAGE_ASPECT_DEPTH_BIT;
			vulkanFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		}

		if (type != Type::COLOUR_CUBEMAP && type != Type::DEPTH_CUBEMAP)
			setupVk(getWidth(), getHeight(), (numStorageMipMaps == 0 ? 1 : numStorageMipMaps), static_cast<VkSampleCountFlagBits>(samples == 0 ? 1 : samples), vulkanFormat, usage, vulkanAspectMask, imageLayout);
		else
			setupCubemapVk(getWidth(), getHeight(), (numStorageMipMaps == 0 ? 1 : numStorageMipMaps), vulkanFormat, usage, vulkanAspectMask, imageLayout);
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

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::COLOUR_CUBEMAP) {
			getParameters().setTarget(GL_TEXTURE_CUBE_MAP);
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			glType = GL_FLOAT;
			attachment = GL_COLOR_ATTACHMENT0 + indexOfColourAttachment;

			//getParameters().setFilter(TextureParameters::Filter::LINEAR); //Assigned in PBREnvironment currently
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_TEXTURE) {
			getParameters().setTarget(GL_TEXTURE_2D);
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			glType = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;

			//getParameters().setFilter(TextureParameters::Filter::LINEAR);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_BORDER);
		} else if (type == Type::DEPTH) {
			getParameters().setTarget(GL_RENDERBUFFER);
			internalFormat = GL_DEPTH_COMPONENT32;
			format = GL_DEPTH_COMPONENT;
			glType = GL_UNSIGNED_INT;
			attachment = GL_DEPTH_ATTACHMENT;

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		} else if (type == Type::DEPTH_CUBEMAP) {
			getParameters().setTarget(GL_TEXTURE_CUBE_MAP);
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			glType = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;

			//getParameters().setFilter(TextureParameters::Filter::NEAREST);
			//getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
		}

		if (type != Type::COLOUR_CUBEMAP && type != Type::DEPTH_CUBEMAP) {
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
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), glRBO);
			} else {
				//Setup the texture
				create();
				bind();
				//if (multisample)
				//	glTexImage2DMultisample(getParameters().getTarget(), Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
				//else
				glTexImage2D(getParameters().getTarget(), 0, internalFormat, getWidth(), getHeight(), 0, format, glType, NULL);
				glTexParameteri(getParameters().getTarget(), GL_TEXTURE_MAX_LEVEL, numStorageMipMaps);

				applyParameters(false);

				//Attach to framebuffer
				//glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), getHandle(), 0);
				unbind();
			}
		} else {
			//Setup the texture
			create();
			bind();

			//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA16F, getWidth(), getHeight());

			for (unsigned int i = 0; i < 6; ++i) {
				//if (multisample)
				//	glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
				//else
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, getWidth(), getHeight(), 0, format, glType, NULL);
			}

			//https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
			//Seem to have to do here in order to not have GL_INVALID_OPERATION occur when trying later
			//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			glTexParameteri(getParameters().getTarget(), GL_TEXTURE_MAX_LEVEL, numStorageMipMaps);
			applyParameters(false, false);

			//Attach to framebuffer
			//glFramebufferTexture(GL_FRAMEBUFFER, attachment, getHandle(), mipLevel);
			unbind();
		}
	}
}

void FramebufferAttachment::setupFBO(unsigned int indexOfColourAttachment, int mipLevel) {
	//Ensure textures have been setup assigned
	if (beenSetup) {
		//Has already been setup, so just need to use framebuffer appropriately

		//Check whether using Vulkan
		if (BaseEngine::usingVulkan()) {


			//Check to see if the mip level is such that a specific level should be used in the FBO
			if (mipLevel > -1) {
				//Add the new level if needed
				if (vulkanMipMapImageViews.find(mipLevel) == vulkanMipMapImageViews.end()) {
					//Create and append the new image view

					VkImageView imageView;
					if (type != Type::COLOUR_CUBEMAP && type != Type::DEPTH_CUBEMAP)
						imageView = Vulkan::createImageView(getVkImage(), VK_IMAGE_VIEW_TYPE_2D, vulkanFormat, vulkanAspectMask, 1, mipLevel, 1);
					else
						imageView = Vulkan::createImageView(getVkImage(), VK_IMAGE_VIEW_TYPE_CUBE, vulkanFormat, vulkanAspectMask, 1, mipLevel, 6);
					vulkanMipMapImageViews.insert(std::pair<uint32_t, VkImageView>(mipLevel, imageView));
				}
			}
		} else {
			GLenum attachment;
			if (type == Type::COLOUR_TEXTURE) {
				attachment = GL_COLOR_ATTACHMENT0 + indexOfColourAttachment;
			} else if (type == Type::COLOUR_CUBEMAP) {
				attachment = GL_COLOR_ATTACHMENT0 + indexOfColourAttachment;
			} else if (type == Type::DEPTH_TEXTURE) {
				attachment = GL_DEPTH_ATTACHMENT;
			} else if (type == Type::DEPTH) {
				attachment = GL_DEPTH_ATTACHMENT;
			} else if (type == Type::DEPTH_CUBEMAP) {
				attachment = GL_DEPTH_ATTACHMENT;
			}

			if (type != Type::COLOUR_CUBEMAP && type != Type::DEPTH_CUBEMAP) {
				//Check for render buffer object
				if (getParameters().getTarget() == GL_RENDERBUFFER) {
					glBindRenderbuffer(getParameters().getTarget(), 0);

					//Attach to framebuffer
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), glRBO);
				} else {
					//Setup the texture
					bind();

					//Attach to framebuffer
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, getParameters().getTarget(), getHandle(), (mipLevel == -1) ? 0 : mipLevel);
					unbind();
				}
			} else {
				bind();

				//https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
				//Seem to have to do here in order to not have GL_INVALID_OPERATION occur when trying later
				//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

				//Attach to framebuffer
				glFramebufferTexture(GL_FRAMEBUFFER, attachment, getHandle(), (mipLevel == -1) ? 0 : mipLevel);
				unbind();
			}
		}
	}
}

VkAttachmentDescription FramebufferAttachment::getVkAttachmentDescription(bool clearOnLoad) {
	//Create and return the structure
	VkAttachmentDescription description = {};
	description.format         = vulkanFormat;
	description.samples        = static_cast<VkSampleCountFlagBits>(samples == 0 ? 1 : samples);
	if (clearOnLoad)
		description.loadOp     = VK_ATTACHMENT_LOAD_OP_CLEAR;
	else
		description.loadOp     = VK_ATTACHMENT_LOAD_OP_LOAD;
	description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE; //Might want to allow this to change to VK_ATTACHMENT_STORE_OP_DONT_CARE when not needed e.g. depth
	description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	if (clearOnLoad)
		description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	else
		description.initialLayout = vulkanFinalLayout;
	description.finalLayout    = vulkanFinalLayout;

	return description;
}

 /*****************************************************************************
  * The FBO class
  *****************************************************************************/

FBO::FBO(uint32_t width, uint32_t height, std::vector<FramebufferAttachmentInfo> attachments) : width(width), height(height), attachments(attachments) {
	//Setup each attachment
	unsigned int index = 0;
	for (unsigned int i = 0; i < attachments.size(); ++i) {
		//Setup the current attachment
		if (! this->attachments[i].attachment->hasBeenSetup())
			this->attachments[i].attachment->setup(index);
		else
			this->attachments[i].shouldDelete = false;

		if (this->attachments[i].attachment->getType() == FramebufferAttachment::Type::COLOUR_TEXTURE || this->attachments[i].attachment->getType() == FramebufferAttachment::Type::COLOUR_CUBEMAP)
			index++;
	}

	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Go through each attachment and add its description
		for (unsigned int i = 0; i < this->attachments.size(); ++i)
			vulkanAttachmentDescriptions.push_back(this->attachments[i].attachment->getVkAttachmentDescription(this->attachments[i].clearOnLoad));
	}
}

FBO::~FBO() {
	//Delete the framebuffer
	if (framebuffer)
		delete framebuffer;
	if (! BaseEngine::usingVulkan())
		glDeleteFramebuffers(1, &glFBO);
	//Delete all of the attachments
	for (FramebufferAttachmentInfo& attachment : attachments) {
		if (attachment.shouldDelete)
			delete attachment.attachment;
	}
}

void FBO::setup(RenderPass* renderPass) {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		//The framebuffer attachments' image views
		std::vector<VkImageView> framebufferAttachments;

		//Go through each attachment
		for (unsigned int i = 0; i < attachments.size(); ++i) {
			//Setup the current attachment
			attachments[i].attachment->setupFBO(0, attachments[i].mipLevel);
			//Add the attachment and its description to the lists
			if (attachments[i].mipLevel >= 0)
				framebufferAttachments.push_back(attachments[i].attachment->getVkImageViewMipMap(attachments[i].mipLevel));
			else
				framebufferAttachments.push_back(attachments[i].attachment->getVkImageView());
		}

		//Create the framebuffer
		framebuffer = new Framebuffer(renderPass->getVkInstance(), width, height, framebufferAttachments, attachments.size() == 1 && (attachments[0].attachment->getType() == FramebufferAttachment::Type::DEPTH_CUBEMAP));
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
			attachments[i].attachment->setupFBO(index, attachments[i].mipLevel);

			//Add the colour attachments
			if (attachments[i].attachment->getType() == FramebufferAttachment::Type::COLOUR_TEXTURE || attachments[i].attachment->getType() == FramebufferAttachment::Type::COLOUR_CUBEMAP) {
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