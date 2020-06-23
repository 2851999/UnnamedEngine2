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
		COLOUR_TEXTURE, COLOUR_CUBEMAP, DEPTH_TEXTURE, DEPTH, DEPTH_CUBEMAP
	};
private:
	/* Type of attachment this is */
	Type type;

	/* The number of samples of this store */
	unsigned int samples;

	/* The format of this store */
	VkFormat vulkanFormat;

	/* The final layout required for this attachment */
	VkImageLayout vulkanFinalLayout;

	/* Render buffer object (For OpenGL) */
	GLuint glRBO;

	/* States whether this attachment has been setup */
	bool beenSetup = false;
public:
	/* Constructor */
	FramebufferAttachment(uint32_t width, uint32_t height, Type type, TextureParameters textureParameters = TextureParameters(), unsigned int samples = 0);

	/* Destructor */
	virtual ~FramebufferAttachment();

	/* Method to setup this attachment for use with an FBO, given the index of the 
	   current colour attachment in the FBO (used for OpenGL) */
	void setup(unsigned int indexOfColourAttachment);

	/* Method to obtain the attachment description of this attachment for Vulkan */
	VkAttachmentDescription getVkAttachmentDescription(bool clearOnLoad);

	/* Getters */
	inline Type getType() { return type; }
	inline bool hasBeenSetup() { return beenSetup; }
};

/*****************************************************************************
 * The FramebufferAttachmentInfo structure is used for storing
 * information about an attachment and how it should be used
 *****************************************************************************/
struct FramebufferAttachmentInfo {
	FramebufferAttachment* attachment;
	bool                   clearOnLoad;
	bool                   shouldDelete = true; //Used to ensure if a new attachment is supplied to FBO, then it will delete it when destroyed, otherwise assumes it could be used elsewhere
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
	std::vector<FramebufferAttachmentInfo> attachments;

	/* Attachment descriptions for this FBO */
	std::vector<VkAttachmentDescription> vulkanAttachmentDescriptions;
public:
	/* Constructor */
	FBO(uint32_t width, uint32_t height, std::vector<FramebufferAttachmentInfo> attachments);

	/* Destructor */
	virtual ~FBO();

	/* Method to setup this FBO for use (should be called after attaching all required attachments) */
	void setup(RenderPass* renderPass);

	/* Getters */
	inline uint32_t getWidth() { return width; }
	inline uint32_t getHeight() { return height; }
	inline Framebuffer* getFramebuffer() { return framebuffer; }
	inline GLenum getGLFBO() { return glFBO; }
	inline std::vector<VkAttachmentDescription>& getVkAttachmentDescriptions() { return vulkanAttachmentDescriptions; }
	inline FramebufferAttachment* getAttachment(unsigned int index) { return attachments[index].attachment; }
	inline unsigned int getAttachmentCount() { return attachments.size(); }
};