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

	/* Methods used to apply the texture parameters to a texture */
	void apply(GLuint texture, bool bind, bool unbind);
	inline void apply(GLuint texture, bool unbind) { apply(texture, true, unbind); }
	inline void apply(GLuint texture) { apply(texture, true, false); }

	/* Setters and getters */
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
	/* OpenGL handle to the texture */
	GLuint texture = 0;

	/* The width and height */
	unsigned int width = 0;
	unsigned int height = 0;

	/* The number of colour components in this texture e.g.
	 * 3 = RGB, 4 = RGBA */
	unsigned int numComponents = 0;
protected:
	/* The texture parameters for this texture */
	TextureParameters parameters;
public:
	/* Refers to the various positions of the texture, can be used in texture
	 * atlas's to define the position of a sub texture */
	float top    = 0.0f;
	float bottom = 1.0f;
	float left   = 0.0f;
	float right  = 1.0f;

	/* The constructors */
	Texture(TextureParameters parameters = TextureParameters()) : parameters(parameters) { create(); }
	Texture(GLuint texture, TextureParameters parameters = TextureParameters()) : texture(texture), parameters(parameters) {}
	Texture(unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : width(width), height(height), parameters(parameters) { create(); }
	Texture(GLuint texture, unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : texture(texture), width(width), height(height), parameters(parameters) {}

	/* The destructor */
	virtual ~Texture() { destroy(); }

	/* The create method simply obtains a handle for the texture from OpenGL */
	inline void create() {
		glGenTextures(1, &texture);
	}

	/* Various methods to apply the texture parameters, but will only do so if the
	 * texture has been assigned */
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

	/* Basic bind and unbind methods for OpenGL */
	inline void bind()   { glBindTexture(parameters.getTarget(), texture); }
	inline void unbind() { glBindTexture(parameters.getTarget(), 0);       }

	/* Called to delete this texture */
	virtual void destroy() override {
		if (texture > 0)
			glDeleteTextures(1, &texture);
	}

	/* The setters and getters */
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

	/* Returns the data necessary to load a texture - note freeTexture/stbi_image_free should
	 * be called once the image data is no longer needed */
	static unsigned char* loadTexture(std::string path, int& numComponents, GLsizei& width, GLsizei &height, GLint& format);

	/* Calls sybi_image_free */
	static void freeTexture(unsigned char* texture);

	/* Returns a Texture instance after reading its data from a file */
	static Texture* loadTexture(std::string path, TextureParameters parameters = TextureParameters(), bool applyParameters = true);
};

/*****************************************************************************
 * The Cubemap class inherits from Texture to create a cubemap
 *****************************************************************************/

class Cubemap : public Texture {
public:
	/* The constructor */
	Cubemap(std::string path, std::vector<std::string> faces);
};

#endif /* CORE_RENDER_TEXTURE_H_ */
