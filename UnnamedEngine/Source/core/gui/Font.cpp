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

#include "../render/Renderer.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The Font class
 *****************************************************************************/

#define FONT_SCALE 2

FT_Library Font::ftLibrary;

void Font::setup(std::string path, unsigned int size, Colour colour, TextureParameters parameters) {
	FT_Face face;
	//Attempt to get the font face
	if (FT_New_Face(ftLibrary, path.c_str(), 0, &face)) {
		//An error occurred obtaining the font so log an error
		Logger::log("Failed to obtain the font face '" + path + "'", "Font", LogType::Error);
		return;
	}
	//Assign the size
	FT_Set_Pixel_Sizes(face, 0, size * FONT_SCALE);

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
		width += glyphSlot->bitmap.width;
		//Update the height to make it the same as the tallest letter
		height = utils_maths::max(height, glyphSlot->bitmap.rows);
	}
	//Assign the texture atlas width/height variables
	textureAtlasWidth  = width;
	textureAtlasHeight = height;

	//Create and bind the texture atlas
	Texture* textureAtlas = new Texture(width, height, parameters);
	textureAtlas->bind();
	//Allows textures that are not a multiple of 4 in size
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//Define the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

	//The current x offset
	GLint xOffset = 0;
	//Go through each character required's ASCII code again
	for (unsigned int i = ASCII_START; i <= ASCII_END; i++) {
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
		xOffset += glyphSlot->bitmap.width;
	}

	textureAtlas->applyParameters(false, true);

	//Return the GL_UNPACK_ALIGNMENT to its default state
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	FT_Done_Face(face);

	//The shader type to use
	std::string shaderType;
	if (! billboarded)
		shaderType = Renderer::SHADER_FONT;
	else
		shaderType = Renderer::SHADER_BILLBOARDED_FONT;

	//Create the GameObject3D instance and assign the texture
	object3D = new GameObject3D(new Mesh(MeshBuilder::createQuad3D(textureAtlasWidth, textureAtlasHeight, textureAtlas, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS)), shaderType);
	object3D->getMaterial()->diffuseColour = colour;
	object3D->getMaterial()->diffuseTexture = textureAtlas;
	object3D->setScale(1.0f / (float) FONT_SCALE, 1.0f / (float) FONT_SCALE, 1.0f);
	object3D->getMesh()->setCullingEnabled(false);
	object3D->update();
}

void Font::update(std::string text) {
	//The pointer to the MeshData instance in the object
	MeshData* data;

	data = object3D->getMesh()->getData();

	//Clear the previous data
	data->clearPositions();
	data->clearTextureCoords();
	data->clearIndices();

	//The current x and y positions
	float currentX = 0.0f;
	float currentY = 0.0f;

	if (billboarded) {
		currentX = -getWidth(text) / 2.0f;
		currentY = getHeight(text) / 2.0f;
	}

	unsigned int newLineCount = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0) {
			newLineCount ++;
			//Go to a new line
			currentX = 0.0f;
			currentY += textureAtlasHeight;

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

			//Pad the texture coordinates to reduce bleeding artifacts
			float offsetX = 0.002f / (float) object3D->getMaterial()->diffuseTexture->getWidth();
			float offsetY = 0.002f / (float) object3D->getMaterial()->diffuseTexture->getHeight();

			data->addTextureCoord(Vector2f((info.xOffset / (float) textureAtlasWidth) + offsetX, 0.0f + offsetY));
			data->addTextureCoord(Vector2f(((info.xOffset + info.glyphWidth) / (float) textureAtlasWidth) - offsetX, 0.0f + offsetY));
			data->addTextureCoord(Vector2f(((info.xOffset + info.glyphWidth) / (float) textureAtlasWidth) - offsetX, (info.glyphHeight / (float) textureAtlasHeight) - offsetY));
			data->addTextureCoord(Vector2f((info.xOffset / (float) textureAtlasWidth) + offsetX, (info.glyphHeight / (float) textureAtlasHeight) - offsetY));

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
	object3D->getMesh()->getRenderData()->updatePositions(data);
	object3D->getMesh()->getRenderData()->updateTextureCoords();
	object3D->getMesh()->getRenderData()->updateIndices(data);
}

void Font::update(std::string text, Vector2f position) {
	update(text);

	object3D->setPosition(Vector3f(position));
	object3D->update();
}

void Font::update(std::string text, Vector3f position) {
	update(text);

	object3D->setPosition(position);
	object3D->update();
}

void Font::render() {
	if (billboarded) {
		Shader* shader = object3D->getShader();
		shader->use();

		Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

		shader->setUniformVector3("Camera_Right", Vector3f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2)));
		shader->setUniformVector3("Camera_Up", Vector3f(-matrix.get(1, 0), -matrix.get(1, 1), -matrix.get(1, 2)));
		shader->setUniformVector2("Billboard_Size", Vector2f(0.005f, 0.005f));
		shader->setUniformVector3("Billboard_Centre", object3D->getPosition());

		shader->setUniformMatrix4("ProjectionViewMatrix", (Renderer::getCamera()->getProjectionViewMatrix()));

		object3D->render();

		shader->stopUsing();
	} else
		object3D->render();
}

void Font::destroy() {
	delete object3D;
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
	return width * (1.0f / (float) FONT_SCALE);
}

float Font::getHeight(std::string text) {
	//The height
	float height = 0;
	float lineHeight = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0) {
			lineHeight = 0;
			height += lineHeight;
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
	return height * (1.0f / (float) FONT_SCALE);
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

