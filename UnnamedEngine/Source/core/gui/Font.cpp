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

#include "Font.h"

#include "../BaseEngine.h"
#include "../render/Renderer.h"
#include "../../utils/Logging.h"
#include "../vulkan/Vulkan.h"

/*****************************************************************************
 * The Font class
 *****************************************************************************/

FT_Library Font::ftLibrary;

const float Font::RENDER_SCALE = 2.0f;

void Font::setup(std::string path, unsigned int size, TextureParameters parameters) {
	FT_Face face;
	//Attempt to get the font face
	if (FT_New_Face(ftLibrary, path.c_str(), 0, &face)) {
		//An error occurred obtaining the font so log an error
		Logger::log("Failed to obtain the font face '" + path + "'", "Font", LogType::Error);
		return;
	}
	//Assign the size
	FT_Set_Pixel_Sizes(face, 0, size * RENDER_SCALE);

	//Get the GlyphSlot which stores information about a certain character that has been loaded
	FT_GlyphSlot glyphSlot = face->glyph;

	//The width and height of the final texture produced for OpenGL
	int width  = 0;
	int height = 0;

	//Go through each character required's ASCII code starting at 32 (space) and ending at 126 (~)
	for (unsigned int i = ASCII_START; i <= ASCII_END; i++) {
		//Attempt to load the current character
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			//An error occurred loading the character so log an error
			Logger::log("Failed to load the character'" + utils_string::str((char) i) + "' in the font face '" + path + "'", "Font", LogType::Error);
			return;
		}
		//Add onto the width
		width += glyphSlot->bitmap.width + GLYPH_SPACING;
		//Update the height to make it the same as the tallest letter
		height = utils_maths::max(height, glyphSlot->bitmap.rows);
	}

	if (! BaseEngine::usingVulkan()) {
		//Create and bind the texture atlas
		texture = new Texture(width, height, parameters);
		texture->bind();
		//Allows textures that are not a multiple of 4 in size
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//Define the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

		//The current x offset
		GLint xOffset = 0;
		//Go through each character required's ASCII code again
		for (unsigned int i = ASCII_START; i <= ASCII_END; ++i) {
			//Load the current character
			FT_Load_Char(face, i, FT_LOAD_RENDER);
			//Put the data into the texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, 0, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);

			//Assign the character info data
			glyphs[i - ASCII_START].advanceX    = glyphSlot->advance.x;
			glyphs[i - ASCII_START].advanceY    = glyphSlot->advance.y;
			glyphs[i - ASCII_START].glyphWidth  = glyphSlot->bitmap.width;
			glyphs[i - ASCII_START].glyphHeight = glyphSlot->bitmap.rows;
			glyphs[i - ASCII_START].glyphLeft   = glyphSlot->bitmap_left;
			glyphs[i - ASCII_START].glyphTop    = glyphSlot->bitmap_top;
			glyphs[i - ASCII_START].xOffset     = xOffset;

			//Increment the x offset
			xOffset += glyphSlot->bitmap.width + GLYPH_SPACING;
		}

		texture->applyParameters(false, true);

		//Return the GL_UNPACK_ALIGNMENT to its default state
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	} else {
		const VkDeviceSize imageSize = width * height;
		VkFormat format = VK_FORMAT_R8_SRGB;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, 0, imageSize, 0, &data);

		std::vector<unsigned char> values(imageSize, 0);

		//The current x offset
		int xOffset = 0;
		//Go through each character required's ASCII code again
		for (unsigned int i = ASCII_START; i <= ASCII_END; ++i) {
			//Load the current character
			FT_Load_Char(face, i, FT_LOAD_RENDER);

			for (unsigned int j = 0; j < (unsigned int) (glyphSlot->bitmap.rows); ++j) {
				for (unsigned int k = 0; k < (unsigned int) (glyphSlot->bitmap.width); ++k) {
					unsigned int index = ((xOffset + k) + (width * j));
					unsigned int index2 = (j * glyphSlot->bitmap.width) + k;
					values[index] = glyphSlot->bitmap.buffer[index2];
				}
			}

			//Assign the character info data
			glyphs[i - ASCII_START].advanceX    = glyphSlot->advance.x;
			glyphs[i - ASCII_START].advanceY    = glyphSlot->advance.y;
			glyphs[i - ASCII_START].glyphWidth  = glyphSlot->bitmap.width;
			glyphs[i - ASCII_START].glyphHeight = glyphSlot->bitmap.rows;
			glyphs[i - ASCII_START].glyphLeft   = glyphSlot->bitmap_left;
			glyphs[i - ASCII_START].glyphTop    = glyphSlot->bitmap_top;
			glyphs[i - ASCII_START].xOffset     = xOffset;

			//Increment the x offset
			xOffset += glyphSlot->bitmap.width + GLYPH_SPACING;
		}

		memcpy(static_cast<unsigned char*>(data), values.data(), static_cast<unsigned int>(values.size()));
		vkUnmapMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory);

		VkImage textureVkImage;
		VkDeviceMemory textureVkImageMemory;
		Vulkan::createImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVkImage, textureVkImageMemory);

		Vulkan::transitionImageLayout(textureVkImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1); //VK_IMAGE_LAYOUT_UNDEFINED is initial layout (from createImage)
		Vulkan::copyBufferToImage(stagingBuffer, textureVkImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

	    vkDestroyBuffer(Vulkan::getDevice()->getLogical(), stagingBuffer, nullptr);
	    vkFreeMemory(Vulkan::getDevice()->getLogical(), stagingBufferMemory, nullptr);
		VkImageView textureVkImageView = Vulkan::createImageView(textureVkImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);

		texture = new Texture(width, height, textureVkImage, textureVkImageMemory, textureVkImageView, parameters);
	}

	FT_Done_Face(face);
}

void Font::assignMeshData(MeshData* data, std::string text, bool billboarded) {
	//The current x and y positions
	float currentX = 0.0f;
	float currentY = 0.0f;

	if (billboarded) {
		currentX = -getWidth(text) / 2.0f;
		currentY = getHeight(text) / 2.0f;
	}

	unsigned int newLineCount = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); ++i) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0) {
			++newLineCount;
			//Go to a new line
			currentX = 0.0f;
			currentY += texture->getHeight();

			if (billboarded)
				currentX = -getWidth(text) / 2;
		} else {
			//Get the character data for the current character
			GlyphInfo& info = glyphs[((int) text.at(i)) - ASCII_START];

			//The positions used for the vertices
			float xPos = currentX + info.glyphLeft;
			float yPos = currentY - (info.glyphHeight + (info.glyphTop - info.glyphHeight));

			float width = info.glyphWidth;
			float height = info.glyphHeight;

			data->addPosition(Vector3f(xPos, yPos, 0.0f));
			data->addPosition(Vector3f(xPos + width, yPos, 0.0f));
			data->addPosition(Vector3f(xPos + width, yPos + height, 0.0f));
			data->addPosition(Vector3f(xPos, yPos + height, 0.0f));

			data->addTextureCoord(Vector2f((info.xOffset / (float) texture->getWidth()), 0.0f));
			data->addTextureCoord(Vector2f(((info.xOffset + info.glyphWidth) / (float) texture->getWidth()), 0.0f));
			data->addTextureCoord(Vector2f(((info.xOffset + info.glyphWidth) / (float) texture->getWidth()), (info.glyphHeight / (float) texture->getHeight())));
			data->addTextureCoord(Vector2f((info.xOffset / (float) texture->getWidth()), (info.glyphHeight / (float) texture->getHeight())));

			unsigned int ip = (i - newLineCount) * 4;

			data->addIndex(ip + 0);
			data->addIndex(ip + 1);
			data->addIndex(ip + 2);
			data->addIndex(ip + 3);
			data->addIndex(ip + 0);
			data->addIndex(ip + 2);

			currentX += (info.advanceX / 64.0f);
		}
	}
}

void Font::destroy() {
	delete texture;
}

float Font::getWidth(std::string text) {
	//The width
	float width = 0;
	//The width of the current line
	float lineWidth = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0)
			//Reset the width of the current line
			lineWidth = 0;
		else {
			//Get the character data for the current character
			GlyphInfo& info = glyphs[((int) text.at(i)) - ASCII_START];
			//Add onto the width
			lineWidth += (info.advanceX / 64);
		}

		//Find the maximum width
		width = utils_maths::max(width, lineWidth);
	}
	//Return the width
	return width * (1.0f / (float) RENDER_SCALE);
}

float Font::getHeight(std::string text) {
	//The height
	float height = 0;
	float lineHeight = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0) {
			height += lineHeight;
			lineHeight = 0;
		} else {
			//Get the character data for the current character
			GlyphInfo& info = glyphs[((int) text.at(i)) - ASCII_START];
			//Assign the height
			lineHeight = utils_maths::max(lineHeight, info.glyphHeight + (info.glyphTop - info.glyphHeight));
		}
	}
	if (height == 0)
		height = lineHeight;
	//Return the height
	return height * (1.0f / (float) RENDER_SCALE);
}


void Font::initialiseFreeType() {
	//Attempt to initialise FreeType
	if (FT_Init_FreeType(&ftLibrary)) {
		//An error occurred initialising FreeType so log an error
		Logger::log("Failed to initialise FreeType", "Font", LogType::Error);
	}
}

void Font::destroyFreeType() {
	FT_Done_FreeType(ftLibrary);
}

