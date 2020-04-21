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

#pragma once

#include "Framebuffer.h"
#include "Texture.h"

class RenderPass;

 /*****************************************************************************
  * The FramebufferAttachment class is used for storing attachment data for an
  * FBO
  *****************************************************************************/

class FramebufferAttachment : public Texture {
public:
	enum class Type {
		COLOUR, DEPTH
	};
private:
	/* Type of attachment this is */
	Type type;

	/* The format of this store */
	VkFormat vulkanFormat;

	/* The final layout required for this attachment */
	VkImageLayout vulkanFinalLayout;

	/* Render buffer object (For OpenGL) */
	GLuint glRBO;
public:
	/* Constructor */
	FramebufferAttachment(uint32_t width, uint32_t height, Type type);

	/* Destructor */
	virtual ~FramebufferAttachment();

	/* Method to setup this attachment for use with an FBO, given the index of the 
	   current colour attachment in the FBO (used for OpenGL) */
	void setup(unsigned int indexOfColourAttachment);

	/* Method to obtain the attachment description of this attachment for Vulkan */
	VkAttachmentDescription getVkAttachmentDescription();

	/* Getters */
	inline Type getType() { return type; }
};

 /*****************************************************************************
  * The FBO class manages a frambuffer and its attachments for rendering
  *****************************************************************************/

class FBO {
private:
	/* The width and height of the framebuffer to be used */
	uint32_t width;
	uint32_t height;

	/* The framebuffer */
	Framebuffer* framebuffer = NULL;

	/* The FBO for OpenGL */
	GLuint glFBO;

	/* The attachments */
	std::vector<FramebufferAttachment*> attachments;

	/* Attachment descriptions for this FBO */
	std::vector<VkAttachmentDescription> vulkanAttachmentDescriptions;
public:
	/* Constructor */
	FBO(uint32_t width, uint32_t height, std::vector<FramebufferAttachment*> attachments);

	/* Destructor */
	virtual ~FBO();

	/* Method to setup this FBO for use (should be called after attaching all required attachments) */
	void setup(RenderPass* renderPass);

	/* Getters */
	inline Framebuffer* getFramebuffer() { return framebuffer; }
	inline GLenum getGLFBO() { return glFBO; }
	inline std::vector<VkAttachmentDescription>& getVkAttachmentDescriptions() { return vulkanAttachmentDescriptions; }
	inline FramebufferAttachment* getAttachment(unsigned int index) { return attachments[index]; }
};