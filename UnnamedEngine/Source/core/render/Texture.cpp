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

#include <stb/stb_image.h>

#include "Texture.h"

#include "../Window.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The TextureParameters class
 *****************************************************************************/

/* Define the default parameters */
GLuint TextureParameters::DEFAULT_TARGET       = GL_TEXTURE_2D;
GLuint TextureParameters::DEFAULT_FILTER       = GL_NEAREST;
GLuint TextureParameters::DEFAULT_CLAMP        = GL_CLAMP_TO_EDGE;
bool   TextureParameters::DEFAULT_SHOULD_CLAMP = false;
bool   TextureParameters::DEFAULT_SRGB         = false;

void TextureParameters::apply(GLuint texture, bool bind, bool unbind) {
	//Bind the texture if necessary
	if (bind)
		glBindTexture(target, texture);
	//Setup the filter
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	//Setup texture clamping if necessary
	if (shouldClamp) {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, clamp);
		//One more value for cube maps
		if (target == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(target, GL_TEXTURE_WRAP_R, clamp);
	}
	//Sets up mip-mapping if requested
	if (mipMapRequested()) {
		glGenerateMipmap(target);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples);
	}
	//Unbind the texture if necessary
	if (unbind)
		glBindTexture(target, 0);
}

VkSamplerCreateInfo TextureParameters::getVkSamplerCreateInfo() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	if (minFilter == GL_NEAREST)
		samplerInfo.minFilter = VK_FILTER_NEAREST;
	else if (minFilter == GL_LINEAR)
		samplerInfo.minFilter = VK_FILTER_LINEAR;

	if (magFilter == GL_NEAREST)
		samplerInfo.magFilter = VK_FILTER_NEAREST;
	else if (magFilter == GL_LINEAR)
		samplerInfo.magFilter = VK_FILTER_LINEAR;

	if (clamp == GL_CLAMP_TO_EDGE) {
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	} else if (clamp == GL_REPEAT) {
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}

	unsigned int anisotropicSamples = Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples;
	samplerInfo.anisotropyEnable = anisotropicSamples > 0 ? VK_TRUE : VK_FALSE;
	samplerInfo.maxAnisotropy = anisotropicSamples;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE; //Useful for PCF shadows
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	return samplerInfo;
}

/*****************************************************************************
 * The Texture class
 *****************************************************************************/

Texture::Texture(void* imageData, unsigned int numComponents, int width, int height, GLenum type, TextureParameters parameters, bool shouldApplyParameters) : width(width), height(height), numComponents(numComponents), parameters(parameters) {
	//Check whether using OpenGL or Vulkan
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		create();
		//Bind the texture and then pass the texture data to OpenGL
		bind();

		//Obtain the correct format to use
		GLint internalFormat, format;
		getTextureFormatGL(numComponents, parameters.getSRGB(), internalFormat, format);

		glTexImage2D(parameters.getTarget(), 0, internalFormat, width, height, 0, format, type, imageData);

		//Apply the parameters if requested, but don't need to bind the texture again,
		//and it doesn't need to unbind either
		if (shouldApplyParameters)
			applyParameters(false, false);

		unbind();
	} else {
		//Setup the Vulkan texture
		//---------------------------------------------------LOAD AND CREATE THE TEXTURE---------------------------------------------------
		VkDeviceSize imageSize = width * height * STBI_rgb_alpha; //Don't seem to have support for any other colour formats when tiling (at least on chosen device)
		VkFormat format;
		Texture::getTextureFormatVk(STBI_rgb_alpha, false, format);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, imageData, static_cast<size_t>(imageSize));
		vkUnmapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory);

		Vulkan::createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);

		//First need to transition texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL then copy from staging buffer to the texture image
		Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); //VK_IMAGE_LAYOUT_UNDEFINED is initial layout (from createImage)
		Vulkan::copyBufferToImage(stagingBuffer, textureVkImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		//Prepare image for shader access
		Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	    vkDestroyBuffer(Vulkan::getDevice()->getLogical(), stagingBuffer, nullptr);
	    vkFreeMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, nullptr);

		//------------------------------------------------------CREATE THE IMAGE VIEW------------------------------------------------------
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image      = textureVkImage;
		viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D; //TODO: Use target in parameters
		viewInfo.format     = format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;

		if (vkCreateImageView(Vulkan::getDevice()->getLogical(), &viewInfo, nullptr, &textureVkImageView) != VK_SUCCESS)
		    Logger::log("Failed to create texture image view", "Texture", LogType::Error);

		//------------------------------------------------------CREATE THE SAMPLER------------------------------------------------------
		VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();

		//NOTE: Sampler not attached to image (can use again in TextureParameters?)

		if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
			Logger::log("Failed to create texture sampler", "Texture", LogType::Error);
	}
}

void Texture::destroy() {
	if (texture > 0)
		glDeleteTextures(1, &texture);
	else if (textureVkImage != VK_NULL_HANDLE) {
		vkDestroyImageView(Vulkan::getDevice()->getLogical(), textureVkImageView, nullptr);

	    vkDestroyImage(Vulkan::getDevice()->getLogical(), textureVkImage, nullptr);
	    vkFreeMemory(Vulkan::getDevice()->getLogical(), textureVkImageMemory, nullptr);

		vkDestroySampler(Vulkan::getDevice()->getLogical(), textureVkSampler, nullptr);
	}
}

VkDescriptorImageInfo Texture::getVkImageInfo() {
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = textureVkImageView;
    imageInfo.sampler     = textureVkSampler;
    return imageInfo;
}

unsigned char* Texture::loadTexture(std::string path, int& numComponents, int& width, int& height, bool srgb) {
	//Load the data using stb_image
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &numComponents, Window::getCurrentInstance()->getSettings().videoVulkan ? STBI_rgb_alpha : 0); //For Vulkan found other modes are not supported (Should really check for supported ones) - so force number of components

	//Check that the data was loaded
	if (image == nullptr) {
		//Log an error if not
		Logger::log("Failed to load the image from the path '" + path + "'", "Texture", LogType::Error);
		return NULL;
	}

	return image;
}

float* Texture::loadTexturef(std::string path, int& numComponents, int& width, int& height, bool srgb) {
	//Load the data using stb_image
	float* image = stbi_loadf(path.c_str(), &width, &height, &numComponents, Window::getCurrentInstance()->getSettings().videoVulkan ? STBI_rgb_alpha : 0); //For Vulkan found other modes are not supported (Should really check for supported ones) - so force number of components

	//Check that the data was loaded
	if (image == nullptr) {
		//Log an error if not
		Logger::log("Failed to load the image from the path '" + path + "'", "Texture", LogType::Error);
		return NULL;
	}

	return image;
}

void Texture::getTextureFormatGL(int numComponents, bool srgb, GLint& internalFormat, GLint& format) {
	//Check the number of components and assign the right OpenGL format
	if (numComponents == 1)
		internalFormat = GL_RED;
	else if (numComponents == 2)
		internalFormat = GL_RG;
	else if (numComponents == 3) {
		if (srgb)
			internalFormat = GL_SRGB;
		else
			internalFormat = GL_RGB;
	} else if (numComponents == 4) {
		if (srgb)
			internalFormat = GL_SRGB_ALPHA;
		else
			internalFormat = GL_RGBA;
	}

	//SRGB corrects incoming textures to a linear colour space
	if (internalFormat == GL_SRGB)
		format = GL_RGB;
	else if (internalFormat == GL_SRGB_ALPHA)
		format = GL_RGBA;
	else
		format = internalFormat;
}

void Texture::getTextureFormatVk(int numComponents, bool srgb, VkFormat& format) {
	if (srgb) {
		if (numComponents == 1)
			format = VK_FORMAT_R8_SRGB;
		else if (numComponents == 2)
			format = VK_FORMAT_R8G8_SRGB;
		else if (numComponents == 3)
			format = VK_FORMAT_R8G8B8_SRGB;
		else if (numComponents == 4)
			format = VK_FORMAT_R8G8B8A8_SRGB;
	} else {
		if (numComponents == 1)
			format = VK_FORMAT_R8_UNORM;
		else if (numComponents == 2)
			format = VK_FORMAT_R8G8_UNORM;
		else if (numComponents == 3)
			format = VK_FORMAT_R8G8B8_UNORM;
		else if (numComponents == 4)
			format = VK_FORMAT_R8G8B8A8_UNORM;
	}
}

void Texture::freeTexture(void* texture) {
	stbi_image_free(texture);
}

Texture* Texture::createTexture(std::string path, void* data, int numComponents, int width, int height, GLenum type, TextureParameters parameters, bool applyParameters) {
	if (data == NULL)
		return NULL;

	//Create the Texture instance and set it up
	Texture* texture = new Texture(data, numComponents, width, height, type, parameters, applyParameters);
	texture->setPath(path);

	return texture;
}

Texture* Texture::loadTexture(std::string path, TextureParameters parameters, bool applyParameters) {
	//The data needed for the texture
	int numComponents, w, h;
	//Obtain the texture data
	unsigned char* image = loadTexture(path, numComponents, w, h, parameters.getSRGB());

	//Create the texture - case where image is NULL is handled by this as well
	Texture* texture = createTexture(path, image, numComponents, w, h, GL_UNSIGNED_BYTE, parameters, applyParameters);

	//Free the image data as it is no longer needed
	stbi_image_free(image);

	return texture;
}

Texture* Texture::loadTexturef(std::string path, TextureParameters parameters, bool applyParameters) {
	//The data needed for the texture
	int numComponents, w, h;
	//Obtain the texture data
	float* image = loadTexturef(path, numComponents, w, h, parameters.getSRGB());

	//Create the texture - case where image is NULL is handled by this as well
	Texture* texture = createTexture(path, image, numComponents, w, h, GL_FLOAT, parameters, applyParameters);

	//Free the image data as it is no longer needed
	stbi_image_free(image);

	return texture;
}

void Texture::setFlipVerticallyOnLoad(bool flip) {
	//Assign the value
	stbi_set_flip_vertically_on_load(flip);
}

/*****************************************************************************
 * The Cubemap class
 *****************************************************************************/

Cubemap::Cubemap(std::string path, std::vector<std::string> faces) : Texture() {
	//Ensure the correct number of textures are present
	if (faces.size() == 6) {
		//Assign the texture parameters
		parameters.setTarget(GL_TEXTURE_CUBE_MAP);
		parameters.setFilter(GL_LINEAR);
		parameters.setClamp(GL_CLAMP_TO_EDGE);
		parameters.setShouldClamp(true);
		//Bind this cubemap
		bind();

		//Data required for setting up
		int numComponents, width, height, internalFormat, format;
		//The current texture
		unsigned char* image;

		//Go through each face
		for (unsigned int i = 0; i < faces.size(); i++) {
			//Load the image for the current face
			image = Texture::loadTexture(path + faces[i], numComponents, width, height, parameters.getSRGB());
			Texture::getTextureFormatGL(numComponents, parameters.getSRGB(), internalFormat, format);
			//Setup the texture
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				internalFormat,
				width,
				height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				image
			);
			//Free the loaded image data
			Texture::freeTexture(image);
		}

		//Now that the cubemap is fully loaded, apply the texture parameters
		applyParameters(false, true);
	} else {
		//Log an error
		Logger::log("Need 6 faces for a cubemap texture", "Cubemap", LogType::Error);
	}
}

Cubemap* Cubemap::createCubemap(GLsizei size, GLint internalFormat, GLint format, GLenum type, TextureParameters parameters) {
	//Ensure the parameters are suitable
	parameters.setTarget(GL_TEXTURE_CUBE_MAP);

	//Create the cubemap instance (thereby generating the cubemap handle)
	Cubemap* cubemap = new Cubemap(parameters);

	//Bind the cubemap and setup the various sides
	cubemap->bind();
	for (unsigned int i = 0; i < 6; i++)
	    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size, size, 0, format, type, nullptr);
	//Assign the texture parameters
	cubemap->applyParameters(false);

	//Return the cubemap
	return cubemap;
}
