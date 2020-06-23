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

#include "../BaseEngine.h"
#include "../Window.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The TextureParameters class
 *****************************************************************************/

/* Define the default parameters */
GLuint                         TextureParameters::DEFAULT_TARGET       = GL_TEXTURE_2D;
TextureParameters::Filter      TextureParameters::DEFAULT_FILTER       = Filter::NEAREST;
TextureParameters::AddressMode TextureParameters::DEFAULT_ADDRESS_MODE = AddressMode::REPEAT;
bool                           TextureParameters::DEFAULT_SRGB         = false;

void TextureParameters::apply(GLuint texture, bool bind, bool unbind) {
	//Bind the texture if necessary
	if (bind)
		glBindTexture(target, texture);
	//Setup the filter
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToGL(minFilter));
	GLenum magFilterGL = convertToGL(magFilter);
	if (magFilterGL == GL_NEAREST_MIPMAP_NEAREST || magFilterGL == GL_NEAREST_MIPMAP_LINEAR)
		magFilterGL = GL_NEAREST;
	else if (magFilterGL == GL_LINEAR_MIPMAP_NEAREST || magFilterGL == GL_LINEAR_MIPMAP_LINEAR)
		magFilterGL = GL_LINEAR;
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilterGL);
	//Setup texture clamping
	glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToGL(addressMode));
	glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToGL(addressMode));
	//One more value for cube maps
	if (target == GL_TEXTURE_CUBE_MAP)
		glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToGL(addressMode));
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
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	if (minFilter == Filter::NEAREST || minFilter == Filter::NEAREST_MIPMAP_NEAREST || minFilter == Filter::NEAREST_MIPMAP_LINEAR)
		samplerInfo.minFilter = VK_FILTER_NEAREST;
	else if (minFilter == Filter::LINEAR || minFilter == Filter::LINEAR_MIPMAP_NEAREST || minFilter == Filter::LINEAR_MIPMAP_LINEAR)
		samplerInfo.minFilter = VK_FILTER_LINEAR;

	if (magFilter == Filter::NEAREST || magFilter == Filter::NEAREST_MIPMAP_NEAREST || magFilter == Filter::NEAREST_MIPMAP_LINEAR)
		samplerInfo.magFilter = VK_FILTER_NEAREST;
	else if (magFilter == Filter::LINEAR || magFilter == Filter::LINEAR_MIPMAP_NEAREST || magFilter == Filter::LINEAR_MIPMAP_LINEAR)
		samplerInfo.magFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = convertToVk(addressMode);
	samplerInfo.addressModeV = convertToVk(addressMode);
	samplerInfo.addressModeW = convertToVk(addressMode);

	unsigned int anisotropicSamples = Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples;
	samplerInfo.anisotropyEnable = anisotropicSamples > 0 ? VK_TRUE : VK_FALSE;
	samplerInfo.maxAnisotropy = anisotropicSamples;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE; //Useful for PCF shadows
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	if (minFilter == Filter::NEAREST_MIPMAP_NEAREST || minFilter == Filter::LINEAR_MIPMAP_NEAREST)
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	else if (minFilter == Filter::NEAREST_MIPMAP_LINEAR || minFilter == Filter::LINEAR_MIPMAP_LINEAR)
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	else
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod     = 0.0f;
	samplerInfo.maxLod     = 0.0f; //SHOULD BE ASSIGNED IF MIPMAP USED (see Texture constructor)

	return samplerInfo;
}

GLenum TextureParameters::convertToGL(Filter filter) {
	switch (filter) {
		case Filter::NEAREST:
			return GL_NEAREST;
		case Filter::LINEAR:
			return GL_LINEAR;
		case Filter::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case Filter::NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		case Filter::LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case Filter::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return GL_NEAREST;
	}
}

GLenum TextureParameters::convertToGL(AddressMode addressMode) {
	switch (addressMode) {
		case AddressMode::REPEAT:
			return GL_REPEAT;
		case AddressMode::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case AddressMode::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case AddressMode::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		case AddressMode::MIRROR_CLAMP_TO_EDGE:
			return GL_MIRROR_CLAMP_TO_EDGE;
		default:
			return GL_REPEAT;
	}
}

VkSamplerAddressMode TextureParameters::convertToVk(AddressMode addressMode) {
	switch (addressMode) {
		case AddressMode::REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case AddressMode::MIRRORED_REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case AddressMode::CLAMP_TO_EDGE:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case AddressMode::CLAMP_TO_BORDER:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case AddressMode::MIRROR_CLAMP_TO_EDGE:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

/*****************************************************************************
 * The Texture class
 *****************************************************************************/

Texture::Texture(TextureParameters parameters) : parameters(parameters) {
	if (! BaseEngine::usingVulkan())
		create();
}

void Texture::create() {
	glGenTextures(1, &texture);
}

void Texture::setupVk(uint32_t width, uint32_t height, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout imageLayout) {
	//Create the image
	Vulkan::createImage(width, height, 1, 1, samples, format, VK_IMAGE_TILING_OPTIMAL, usage, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);
	//Create the image view
	textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_2D, format, aspectMask, 1, 1);
	//Create the sampler
	VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; //For depth

	if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

	//Setup the descriptor info
	imageInfo.imageLayout = imageLayout;
	imageInfo.imageView = textureVkImageView;
	imageInfo.sampler = textureVkSampler;
}

void Texture::setupCubemapVk(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout imageLayout) {
	//Create the image
	Vulkan::createImage(width, height, 1, 6, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);
	//Create the image view
	textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_CUBE, format, aspectMask, 1, 6);
	//Create the sampler
	VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; //For depth

	if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

	//Setup the descriptor info
	imageInfo.imageLayout = imageLayout;
	imageInfo.imageView = textureVkImageView;
	imageInfo.sampler = textureVkSampler;
}

void Texture::setupVk(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout imageLayout) {
	//Create the image
	Vulkan::createImage(width, height, mipLevels, 1, samples, format, VK_IMAGE_TILING_OPTIMAL, usage, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);
	//Create the image view
	textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_2D, format, aspectMask, mipLevels, 1);
	//Create the sampler
	VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; //For depth

	if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

	//Setup the descriptor info
	imageInfo.imageLayout = imageLayout;
	imageInfo.imageView = textureVkImageView;
	imageInfo.sampler = textureVkSampler;
}

void Texture::setupCubemapVk(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout imageLayout) {
	//Create the image
	Vulkan::createImage(width, height, mipLevels, 6, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);
	//Create the image view
	textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_CUBE, format, aspectMask, mipLevels, 6);
	//Create the sampler
	VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; //For depth

	if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

	//Setup the descriptor info
	imageInfo.imageLayout = imageLayout;
	imageInfo.imageView = textureVkImageView;
	imageInfo.sampler = textureVkSampler;
}

Texture::Texture(void* imageData, unsigned int numComponents, int width, int height, GLenum type, TextureParameters parameters, bool shouldApplyParameters) : width(width), height(height), numComponents(numComponents), parameters(parameters) {
	//Check whether using OpenGL or Vulkan
	if (! BaseEngine::usingVulkan()) {
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

		//Check if mipmaps should be used
		if (parameters.mipMapRequested())
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		//---------------------------------------------------LOAD AND CREATE THE TEXTURE---------------------------------------------------

		//When using Vulkan all textures are loaded with 4 components, but in the case this is one it is assigned elsewhere so the requested value should be used
		//e.g. for height map generation
		if (numComponents != 1)
			numComponents = 4;

		VkDeviceSize imageSize = width * height * numComponents;
		if (type == GL_FLOAT)
			imageSize *= sizeof(float);

		VkFormat format;
		Texture::getTextureFormatVk(numComponents, parameters.getSRGB(), type == GL_FLOAT, format);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, imageData, static_cast<size_t>(imageSize));
		vkUnmapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory);

		VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (mipLevels > 1)
			imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; //Used as source and destination of transfers (as will copy data for mipmaps)

		Vulkan::createImage(width, height, mipLevels, 1, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, imageUsageFlags, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);

		//First need to transition texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL then copy from staging buffer to the texture image
		Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1); //VK_IMAGE_LAYOUT_UNDEFINED is initial layout (from createImage)
		Vulkan::copyBufferToImage(stagingBuffer, textureVkImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		if (mipLevels <= 1) //Leave in VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL for mipmap generation
			//Prepare image for shader access
			Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 1);

	    vkDestroyBuffer(Vulkan::getDevice()->getLogical(), stagingBuffer, nullptr);
	    vkFreeMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, nullptr);

		if (mipLevels > 1)
			generateMipmapsVk(); //Transitions image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL here

		//Create the image view
		textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1);

		//------------------------------------------------------CREATE THE SAMPLER------------------------------------------------------
		VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
		samplerInfo.maxLod = static_cast<float>(mipLevels);

		//NOTE: Sampler not attached to image (can use again in TextureParameters?)

		if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
			Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

		//Setup the descriptor info
	    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	    imageInfo.imageView   = textureVkImageView;
	    imageInfo.sampler     = textureVkSampler;
	}
}

Texture::Texture(unsigned int width, unsigned int height, VkImage textureVkImage, VkDeviceMemory textureVkImageMemory, VkImageView textureVkImageView, TextureParameters parameters) : width(width), height(height), parameters(parameters), textureVkImage(textureVkImage), textureVkImageMemory(textureVkImageMemory), textureVkImageView(textureVkImageView) {
	//------------------------------------------------------CREATE THE SAMPLER------------------------------------------------------
	VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
	samplerInfo.maxLod = static_cast<float>(mipLevels);

	//NOTE: Sampler not attached to image (can use again in TextureParameters?)

	if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
		Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

	//Setup the descriptor info
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = this->textureVkImageView;
    imageInfo.sampler     = textureVkSampler;
}

Texture::~Texture() {
	if (texture > 0)
		glDeleteTextures(1, &texture);
	else if (textureVkImage != VK_NULL_HANDLE) {
		vkDestroyImageView(Vulkan::getDevice()->getLogical(), textureVkImageView, nullptr);

		vkDestroyImage(Vulkan::getDevice()->getLogical(), textureVkImage, nullptr);
		vkFreeMemory(Vulkan::getDevice()->getLogical(), textureVkImageMemory, nullptr);

		vkDestroySampler(Vulkan::getDevice()->getLogical(), textureVkSampler, nullptr);
	}
}

void Texture::generateMipmapsVk() {
	//Obtain the format used for this image
	VkFormat format;
	getTextureFormatVk(STBI_rgb_alpha, parameters.getSRGB(), false, format);

	//Check the used image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(Vulkan::getDevice()->getPhysical(), format, &formatProperties);

	if (! (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		Logger::log("Texture image format does not support linear blitting", "Texture", LogType::Error);
	else {
		VkCommandBuffer commandBuffer = Vulkan::beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image                           = textureVkImage;
		barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = 1;
		barrier.subresourceRange.levelCount     = 1;

		int32_t mipWidth  = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; ++i) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit = {};
			blit.srcOffsets[0]                 = { 0, 0, 0 };
			blit.srcOffsets[1]                 = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel       = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount     = 1;
			blit.dstOffsets[0]                 = { 0, 0, 0 };
			blit.dstOffsets[1]                 = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel       = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount     = 1;

			vkCmdBlitImage(commandBuffer, textureVkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, textureVkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR); //Can use nearest?

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			if (mipWidth > 1)  mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		Vulkan::endSingleTimeCommands(commandBuffer);
	}
}

unsigned char* Texture::loadTexture(std::string path, int& numComponents, int& width, int& height, bool srgb) {
	//Load the data using stb_image
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &numComponents, BaseEngine::usingVulkan() ? STBI_rgb_alpha : 0); //For Vulkan found other modes are not supported (Should really check for supported ones) - so force number of components

	//Ignore the returned value as is wrong for somereason when assigned manually
	if (BaseEngine::usingVulkan())
		numComponents = STBI_rgb_alpha;

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
	float* image = stbi_loadf(path.c_str(), &width, &height, &numComponents, BaseEngine::usingVulkan() ? STBI_rgb_alpha : 0); //For Vulkan found other modes are not supported (Should really check for supported ones) - so force number of components

	//Ignore the returned value as is wrong for somereason when assigned manually
	if (BaseEngine::usingVulkan())
		numComponents = STBI_rgb_alpha;

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

void Texture::getTextureFormatVk(int numComponents, bool srgb, bool useFloat, VkFormat& format) {
	if (useFloat) {
		if (srgb) {
			//sizeof(float) = 4 => 32 bit needed for loading float textures with stbi_loadf
			if (numComponents == 1)
				format = VK_FORMAT_R32_SFLOAT;
			else if (numComponents == 2)
				format = VK_FORMAT_R32G32_SFLOAT;
			else if (numComponents == 3)
				format = VK_FORMAT_R32G32B32_SFLOAT;
			else if (numComponents == 4)
				format = VK_FORMAT_R32G32B32A32_SFLOAT;
		} else {
			if (numComponents == 1)
				format = VK_FORMAT_R32_SFLOAT;
			else if (numComponents == 2)
				format = VK_FORMAT_R32G32_SFLOAT;
			else if (numComponents == 3)
				format = VK_FORMAT_R32G32B32_SFLOAT;
			else if (numComponents == 4)
				format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	} else {
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
		parameters.setFilter(TextureParameters::Filter::LINEAR);
		parameters.setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);

		if (! BaseEngine::usingVulkan()) {
			//Bind this cubemap
			bind();

			//Data required for setting up
			int numComponents, width, height, internalFormat, format;
			//The current texture
			unsigned char* image;

			//Go through each face
			for (unsigned int i = 0; i < faces.size(); ++i) {
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
			unsigned char* textureData[6];

			//Data required for setting up
			int numComponents, width, height;
			for (unsigned int i = 0; i < faces.size(); ++i)
				//Load the image for the current face
				textureData[i] = Texture::loadTexture(path + faces[i], numComponents, width, height, parameters.getSRGB());

			const VkDeviceSize layerSize = width * height * STBI_rgb_alpha;
			const VkDeviceSize imageSize = layerSize * 6; //Assume texture sizes are identical

			VkFormat format;
			Texture::getTextureFormatVk(STBI_rgb_alpha, parameters.getSRGB(), false, format);

			//Staging buffer
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
			void* data;
			vkMapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, 0, imageSize, 0, &data);
			for (unsigned int i = 0; i < 6; ++i)
				memcpy(static_cast<unsigned char*>(data) + ((layerSize * i)), textureData[i], static_cast<unsigned int>(layerSize));
			vkUnmapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory);

			Vulkan::createImage(width, height, 1, 6, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);

			VkCommandBuffer copyCommandBuffer = Vulkan::beginSingleTimeCommands();

			std::vector<VkBufferImageCopy> bufferCopyRegions;
			unsigned int offset = 0;

			for (unsigned int i = 0; i < 6; ++i) {
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = 0;
				bufferCopyRegion.imageSubresource.baseArrayLayer = i;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = width;
				bufferCopyRegion.imageExtent.height = height;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += layerSize;
			}

			Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 6, copyCommandBuffer);

			vkCmdCopyBufferToImage(
				copyCommandBuffer,
				stagingBuffer,
				textureVkImage,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				static_cast<uint32_t>(bufferCopyRegions.size()),
				bufferCopyRegions.data()
			);

			Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 6, copyCommandBuffer);

			Vulkan::endSingleTimeCommands(copyCommandBuffer);

		    vkDestroyBuffer(Vulkan::getDevice()->getLogical(), stagingBuffer, nullptr);
		    vkFreeMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, nullptr);

		    for (unsigned int i = 0; i < 6; ++i)
				//Free the loaded image data
				Texture::freeTexture(textureData[i]);

		    textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_CUBE, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 6);

			VkSamplerCreateInfo samplerInfo = parameters.getVkSamplerCreateInfo();
			samplerInfo.maxLod = static_cast<float>(1);

			if (vkCreateSampler(Vulkan::getDevice()->getLogical(), &samplerInfo, nullptr, &textureVkSampler) != VK_SUCCESS)
				Logger::log("Failed to create texture sampler", "Texture", LogType::Error);

			//Setup the descriptor info
		    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		    imageInfo.imageView   = textureVkImageView;
		    imageInfo.sampler     = textureVkSampler;
		}
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
