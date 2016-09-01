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
 * The FramebufferTexture class stores the data required create a texture
 * attachment for an FBO
 *****************************************************************************/

class FramebufferTexture : public Texture {
private:
	/* Various pieces of data */
	GLint   internalFormat;
	GLenum  format;
	GLenum  type;
	GLenum  attachment;

	GLuint  rbo;
public:
	/* The constructor */
	FramebufferTexture(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum attachment) :
			Texture(0, TextureParameters(target, GL_LINEAR, GL_CLAMP_TO_EDGE, false)){
		this->internalFormat = internalFormat;
		setWidth(width);
		setHeight(height);
		this->format = format;
		this->type = type;
		this->attachment = attachment;

		this->rbo = 0;
	}

	/* The destructor */
	virtual ~FramebufferTexture() {
		destroy();
	}

	virtual void destroy() override {
		if (getHandle() != 0)
			Texture::destroy();
		if (rbo != 0)
			glDeleteRenderbuffers(1, &rbo);
	}

	/* The method used to setup this texture */
	void setup(GLuint fboTarget);

	/* Setters and getters */
	inline GLint getInternalFormat() { return internalFormat; }
	inline GLenum getFormat() { return format; }
	inline GLenum getType() { return type; }
	inline GLenum getAttachment() { return attachment; }
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
	std::vector<FramebufferTexture*> textures;
public:
	/* The constructor */
	FBO(GLuint target) {
		this->framebuffer = 0;
		this->target = target;
	}

	/* The destructor */
	virtual ~FBO() {
		for (unsigned int i = 0; i < textures.size(); i++)
			delete textures[i];
		textures.clear();
	}

	/* The method used to attach a texture */
	inline void attach(FramebufferTexture* texture) { textures.push_back(texture); }

	/* The method used to setup this FBO */
	void setup();

	/* Returns the framebuffer texture at a given index */
	inline FramebufferTexture* getFramebufferTexture(unsigned int index) { return textures[index]; }

	/* The method used to bind this framebuffer */
	inline void bind() { glBindFramebuffer(target, framebuffer); }

	/* The method used to unbind this framebuffer */
	inline void unbind() { glBindFramebuffer(target, 0); }
};



#endif /* CORE_RENDER_FBO_H_ */
