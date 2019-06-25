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

#include "../Window.h"
#include <string>

#include "../Resource.h"

/*****************************************************************************
 * The TextureParameters class stores data about how a texture should be
 * rendered as well as actually applying the parameters to a texture
 *****************************************************************************/

class TextureParameters {
private:
	/* The texture parameters with their default values */
	GLuint target    = DEFAULT_TARGET;
	GLuint minFilter = DEFAULT_FILTER;
	GLuint magFilter = DEFAULT_FILTER;
	GLuint clamp     = DEFAULT_CLAMP;

	bool srgb = DEFAULT_SRGB;
	bool generateMipMapsIfAvailable = true;
public:
	/* The default values which are assigned unless otherwise specified */
	static GLuint DEFAULT_TARGET;
	static GLuint DEFAULT_FILTER;
	static GLuint DEFAULT_CLAMP;
	static bool   DEFAULT_SRGB;

	/* Various constructors */
	TextureParameters() {}
	TextureParameters(GLuint target) : target(target) {}
	TextureParameters(GLuint target, GLuint filter) : target(target), minFilter(filter), magFilter(filter) {}
	TextureParameters(GLuint target, GLuint filter, GLuint clamp) : target(target), minFilter(filter), magFilter(filter), clamp(clamp) {}
	TextureParameters(GLuint target, GLuint filter, GLuint clamp, bool srgb = DEFAULT_SRGB) : target(target), minFilter(filter), magFilter(filter), clamp(clamp), srgb(srgb) {}

	/* Methods used to apply the texture parameters to a texture */
	void apply(GLuint texture, bool bind, bool unbind);
	inline void apply(GLuint texture, bool unbind) { apply(texture, true, unbind); }
	inline void apply(GLuint texture) { apply(texture, true, false); }

	/* Returns the creation info for a sampler with the properties of this instance (Vulkan) */
	VkSamplerCreateInfo getVkSamplerCreateInfo();

	/* Setters and getters */
	inline TextureParameters setTarget(GLuint target) { this->target = target; return (*this); }
	inline TextureParameters setFilter(GLuint filter) { this->minFilter = filter; this->magFilter = filter; return (*this); }
	inline TextureParameters setMinFilter(GLuint minFilter) { this->minFilter = minFilter; return (*this); }
	inline TextureParameters setMagFilter(GLuint magFilter) { this->magFilter = magFilter; return (*this); }
	inline TextureParameters setClamp(GLuint clamp)   { this->clamp  = clamp;  return (*this); }
	inline TextureParameters setSRGB(bool srgb) { this->srgb = srgb; return (*this); }

	inline void preventGenerateMipMaps() { generateMipMapsIfAvailable = false; }

	inline GLuint getTarget() { return target; }
	inline GLuint getMinFilter() { return minFilter; }
	inline GLuint getMagFilter() { return magFilter; }
	inline GLuint getClamp()  { return clamp;  }
	inline bool getSRGB() { return srgb; }

	/* Returns whether a mipmap should be generated */
	inline bool mipMapRequested() {
		return generateMipMapsIfAvailable &&
			   ((minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_LINEAR) ||
			    (magFilter == GL_NEAREST_MIPMAP_NEAREST || magFilter == GL_NEAREST_MIPMAP_LINEAR || magFilter == GL_LINEAR_MIPMAP_NEAREST || magFilter == GL_LINEAR_MIPMAP_LINEAR));
	}
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

	/* The path this texture was loaded from (if applicable) */
	std::string path;

	/* The number of mip levels for this texture (for Vulkan) */
	uint32_t mipLevels = 1;

	/* Method to generate mipmaps (for Vulkan) */
	void generateMipmapsVk();
protected:
	/* The texture parameters for this texture */
	TextureParameters parameters;

	/* Required objects for Vulkan to access and use a texture */
	VkImage        textureVkImage       = VK_NULL_HANDLE;
	VkDeviceMemory textureVkImageMemory = VK_NULL_HANDLE;
	VkImageView    textureVkImageView   = VK_NULL_HANDLE;
	VkDescriptorImageInfo imageInfo;

	/* Sampler for this texture (Should change to use one for many rather than
	 * create one for each texture) */
	VkSampler textureVkSampler = VK_NULL_HANDLE;
public:
	/* The constructors */
	Texture(TextureParameters parameters = TextureParameters());
	Texture(GLuint texture, TextureParameters parameters = TextureParameters()) : texture(texture), parameters(parameters) {}
	Texture(unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : width(width), height(height), parameters(parameters) { create(); }
	Texture(GLuint texture, unsigned int width, unsigned int height, TextureParameters parameters = TextureParameters()) : texture(texture), width(width), height(height), parameters(parameters) {}
	Texture(void* data, unsigned int numComponents, int width, int height, GLenum type, TextureParameters parameters = TextureParameters(), bool shouldApplyParameters = true);

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
	inline void unbind() { glBindTexture(parameters.getTarget(), 0); }

	/* Called to delete this texture */
	virtual void destroy() override;

	/* The setters and getters */
	inline void setParameters(TextureParameters& parameters) { this->parameters = parameters; }
	inline void setWidth(unsigned int width) { this->width = width; }
	inline void setHeight(unsigned int height) { this->height = height; }
	inline void setSize(unsigned int width, unsigned int height) { this->width = width; this->height = height; }
	inline void setNumComponents(unsigned int numComponents) { this->numComponents = numComponents; }
	inline void setPath(std::string path) { this->path = path; }

	inline GLuint getHandle() { return texture; }
	inline TextureParameters& getParameters() { return parameters; }
	inline unsigned int getWidth() { return width; }
	inline unsigned int getHeight() { return height; }
	inline int getNumComponents() { return numComponents; }
	inline bool hasTexture() { return texture > 0; }
	inline std::string getPath() { return path; }
	inline bool hasPath() { return path.length() > 0; }
	VkImageView& getVkImageView() { return textureVkImageView; }
	const VkDescriptorImageInfo* getVkImageInfo() { return &imageInfo; }

	/* Returns the data necessary to load a texture - note freeTexture/stbi_image_free should
	 * be called once the image data is no longer needed */
	static unsigned char* loadTexture(std::string path, int& numComponents, int& width, int& height, bool srgb);
	/* Returns the data necessary to load a texture taking the data as a float, again the
	 * texture should be freed afterwards*/
	static float* loadTexturef(std::string path, int& numComponents, int& width, int& height, bool srgb);

	/* Obtains the OpenGL texture format and internal format that an image should have from its number of colour
	 * components and whether it should be SRGB */
	static void getTextureFormatGL(int numComponents, bool srgb, GLint& internalFormat, GLint& format);

	/* Obtains the Vulkan format that an image should have from its number of colour
	 * components and whether it should be SRGB */
	static void getTextureFormatVk(int numComponents, bool srgb, VkFormat& format);

	/* Returns a texture instance created using the data given */
	static Texture* createTexture(std::string path, void* data, int numComponents, int width, int height, GLenum type, TextureParameters parameters = TextureParameters(), bool applyParameters = true);

	/* Calls stbi_image_free */
	static void freeTexture(void* texture);

	/* Returns a Texture instance after reading its data from a file */
	static Texture* loadTexture(std::string path, TextureParameters parameters = TextureParameters(), bool applyParameters = true);
	static Texture* loadTexturef(std::string path, TextureParameters parameters = TextureParameters(), bool applyParameters = true);

	/* Allows loaded textures to be flipped when being loaded */
	static void setFlipVerticallyOnLoad(bool flip);
};

/*****************************************************************************
 * The Cubemap class inherits from Texture to create a cubemap
 *****************************************************************************/

class Cubemap : public Texture {
public:
	/* The constructors */
	Cubemap(TextureParameters parameters = TextureParameters()) : Texture(parameters) {}
	Cubemap(std::string path, std::vector<std::string> faces);
	Cubemap(GLuint handle, TextureParameters parameters = TextureParameters().setTarget(GL_TEXTURE_CUBE_MAP)) : Texture(handle, parameters) {} //Will not apply texture parameters

	/* Method used to create a cube map given various values TextureParameters will be assigned
	 * and the cubemap will remain bound */
	static Cubemap* createCubemap(GLsizei size, GLint internalFormat, GLint format, GLenum type, TextureParameters parameters = TextureParameters());
};

#endif /* CORE_RENDER_TEXTURE_H_ */
