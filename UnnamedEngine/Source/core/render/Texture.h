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

#ifndef CORE_RENDER_TEXTURE_H_
#define CORE_RENDER_TEXTURE_H_

#include <GL/glew.h>
#include <string>

#include "../Resource.h"

/*****************************************************************************
 * The TextureParameters class stores data about how a texture should be
 * rendered as well as actually applying the parameters to a texture
 *****************************************************************************/

class TextureParameters {
private:
	/* The texture parameters with their default values */
	GLuint target = DEFAULT_TARGET;
	GLuint filter = DEFAULT_FILTER;
	GLuint clamp  = DEFAULT_CLAMP;
	bool shouldClamp = DEFAULT_SHOULD_CLAMP;
public:
	/* The default values which are assigned unless otherwise specified */
	static GLuint DEFAULT_TARGET;
	static GLuint DEFAULT_FILTER;
	static GLuint DEFAULT_CLAMP;
	static bool   DEFAULT_SHOULD_CLAMP;

	/* Various constructors */
	TextureParameters() {}
	TextureParameters(bool shouldClamp) : shouldClamp(shouldClamp) {}
	TextureParameters(GLuint target) : target(target) {}
	TextureParameters(GLuint target, bool shouldClamp) : target(target), shouldClamp(shouldClamp) {}
	TextureParameters(GLuint target, GLuint filter) : target(target), filter(filter) {}
	TextureParameters(GLuint target, GLuint filter, GLuint clamp) : target(target), filter(filter), clamp(clamp) { shouldClamp = true; }
	TextureParameters(GLuint target, GLuint filter, GLuint clamp, bool shouldClamp) : target(target), filter(filter), clamp(clamp), shouldClamp(shouldClamp) {}

	void apply(GLuint texture, bool bind, bool unbind);
	inline void apply(GLuint texture, bool unbind) { apply(texture, true, unbind); }
	inline void apply(GLuint texture) { apply(texture, true, false); }

	inline TextureParameters setTarget(GLuint target) { this->target = target; return (*this); }
	inline TextureParameters setFilter(GLuint filter) { this->filter = filter; return (*this); }
	inline TextureParameters setClamp(GLuint clamp)   { this->clamp  = clamp;  return (*this); }
	inline TextureParameters setShouldClamp(bool shouldClamp) { this->shouldClamp = shouldClamp; return (*this); }

	inline GLuint getTarget() { return target; }
	inline GLuint getFilter() { return filter; }
	inline GLuint getClamp()  { return clamp;  }
	inline bool getShouldClamp() { return shouldClamp; }
};

/*****************************************************************************
 * The Texture class stores the data required to render a texture
 *****************************************************************************/

class Texture : public Resource {
private:
	GLuint texture = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int numComponents = 0;
protected:
	TextureParameters parameters;
public:
	float top    = 0.0f;
	float bottom = 1.0f;
	float left   = 0.0f;
	float right  = 1.0f;

	Texture(TextureParameters parameters = TextureParameters()) : parameters(parameters) { create(); }
	Texture(GLuint texture, TextureParameters parameters = TextureParameters()) : texture(texture), parameters(parameters) {}
	Texture(unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : width(width), height(height), parameters(parameters) { create(); }
	Texture(GLuint texture, unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : texture(texture), width(width), height(height), parameters(parameters) {}
	virtual ~Texture() { destroy(); }

	inline void create() {
		glGenTextures(1, &texture);
	}

	inline void applyParameters() {
		if (texture > 0)
			parameters.apply(texture);
	}
	inline void applyParameters(bool unbind) {
		if (texture > 0)
			parameters.apply(texture, unbind);
	}
	inline void applyParameters(bool bind, bool unbind) {
		if (texture > 0)
			parameters.apply(texture, bind, unbind);
	}

	inline void bind()   { glBindTexture(parameters.getTarget(), texture); }
	inline void unbind() { glBindTexture(parameters.getTarget(), 0);       }

	virtual void destroy() override {
		glDeleteTextures(1, &texture);
	}

	inline void setParameters(TextureParameters& parameters) { this->parameters = parameters; }
	inline void setWidth(unsigned int width) { this->width = width; }
	inline void setHeight(unsigned int height) { this->height = height; }
	inline void setSize(unsigned int width, unsigned int height) { this->width = width; this->height = height; }
	inline void setNumComponents(unsigned int numComponents) { this->numComponents = numComponents; }
	inline GLuint getHandle() { return texture; }
	inline TextureParameters& getParameters() { return parameters; }
	inline unsigned int getWidth() { return width; }
	inline unsigned int getHeight() { return height; }
	inline int getNumComponents() { return numComponents; }
	inline bool hasTexture() { return texture > 0; }

	/* Returns the data necessary to load a texture */
	static unsigned char* loadTexture(std::string path, int& numComponents, GLsizei& width, GLsizei &height, GLint& format);

	/* Returns a Texture instance after reading its data from a file */
	static Texture* loadTexture(std::string path, TextureParameters parameters = TextureParameters(), bool applyParameters = true);
};

#endif /* CORE_RENDER_TEXTURE_H_ */
