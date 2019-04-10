/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
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

#ifndef CORE_RENDER_FBO_H_
#define CORE_RENDER_FBO_H_

#include "Texture.h"

/*****************************************************************************
 * The FramebufferStore class stores the data required create a texture
 * attachment for an FBO
 *****************************************************************************/

class FramebufferStore : public Texture {
protected:
	/* Various pieces of data */
	GLint   internalFormat;
	GLenum  format;
	GLenum  type;
	GLenum  attachment;

	GLuint  rbo;
public:
	/* The constructors */
	FramebufferStore(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum attachment, GLuint filter, GLuint clamp, bool shouldClamp) :
			Texture(0, TextureParameters(target, filter, clamp, shouldClamp)) {
		this->internalFormat = internalFormat;
		setWidth(width);
		setHeight(height);
		this->format = format;
		this->type = type;
		this->attachment = attachment;

		this->rbo = 0;
	}

	FramebufferStore(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum attachment) : FramebufferStore(target, internalFormat, width, height, format, type, attachment, GL_LINEAR, GL_CLAMP_TO_EDGE, false) {}

	/* The destructor */
	virtual ~FramebufferStore() {
		destroy();
	}

	virtual void destroy() override {
		if (getHandle() != 0)
			Texture::destroy();
		if (rbo != 0)
			glDeleteRenderbuffers(1, &rbo);
	}

	/* The method used to setup this texture */
	virtual void setup(GLuint fboTarget, bool multisample);

	/* Setters and getters */
	inline GLint getInternalFormat() { return internalFormat; }
	inline GLenum getFormat() { return format; }
	inline GLenum getType() { return type; }
	inline GLenum getAttachment() { return attachment; }
};

/*****************************************************************************
 * The FramebufferStoreCubemap class stores the data required create a cubemap
 * attachment for an FBO
 *****************************************************************************/

class FramebufferStoreCubemap : public FramebufferStore {
public:
	/* The constructors */
	FramebufferStoreCubemap(GLint internalFormat, GLsizei size, GLenum format, GLenum type, GLenum attachment, GLuint filter, GLuint clamp, bool shouldClamp) : FramebufferStore(GL_TEXTURE_CUBE_MAP, internalFormat, size, size, format, type, attachment, filter, clamp, shouldClamp) {}

	virtual ~FramebufferStoreCubemap() {}

	/* The method used to setup this texture */
	void setup(GLuint fboTarget, bool multisample) override;
};

/*****************************************************************************
 * The FBO class creates a Frame Buffer Object (FBO) using
 * texture attachments defined above
 *****************************************************************************/

class FBO {
private:
	/* The framebuffer */
	GLuint framebuffer;

	/* The framebuffer target */
	GLuint target;

	/* The attachments */
	std::vector<FramebufferStore*> stores;

	/* States whether this FBO should use multisampling */
	bool multisample;
public:
	/* The constructor */
	FBO(GLuint target, bool multisample = false) {
		this->framebuffer = 0;
		this->target = target;
		this->multisample = multisample;
	}

	/* The destructor */
	virtual ~FBO() {
		for (unsigned int i = 0; i < stores.size(); i++)
			delete stores[i];
		stores.clear();
	}

	/* Method used to attach a texture */
	inline void attach(FramebufferStore* texture) { stores.push_back(texture); }

	/* The method used to setup this FBO */
	void setup();

	/* Copies the contents of a part of this FBO to another FBO */
	void copyTo(unsigned int fboHandle, GLenum sourceMode, GLenum destMode, int sourceWidth, int sourceHeight, int destX, int destY, int destWidth, int destHeight, GLbitfield mask);

	/* Copies the contents of a particular frambuffer store in this framebuffer to another */
	void copyTo(FBO* fbo, unsigned int sourceStoreIndex, unsigned int destStoreIndex);

	/* Copies the contents of a particular framebuffer store in this framebuffer to the screen */
	void copyToScreen(unsigned int sourceStoreIndex, int x, int y, int width, int height);

	/* Returns the framebuffer texture at a given index */
	inline FramebufferStore* getFramebufferStore(unsigned int index) { return stores[index]; }

	/* Binds this framebuffer */
	inline void bind() { glBindFramebuffer(target, framebuffer); }

	/* Unbinds this framebuffer */
	inline void unbind() { glBindFramebuffer(target, 0); }

	/* Getters */
	inline GLuint getHandle() { return framebuffer; }
};



#endif /* CORE_RENDER_FBO_H_ */
