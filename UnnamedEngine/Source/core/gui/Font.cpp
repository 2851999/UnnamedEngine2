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

void Font::setup(std::string name, unsigned int size, Colour colour, TextureParameters parameters) {
	//Attempt to get the font face
	if (FT_New_Face(ftLibrary, name.c_str(), 0, &face)) {
		//An error occurred obtaining the font so log an error
		Logger::log("Failed to obtain the font face '" + name + "'", "Font", LogType::Error);
		return;
	}
	//Assign the size
	FT_Set_Pixel_Sizes(face, 0, size * FONT_SCALE);

	//Get the GlyphSlot which stores information about a certain character that has been loaded
	FT_GlyphSlot glyphSlot = face->glyph;

	//The width and height of the final texture produced for OpenGL
	unsigned int width  = 0;
	unsigned int height = 0;

	//Go through each character required's ASCII code starting at 32 (space) and ending at 126 (~)
	for (unsigned int i = ASCII_START; i <= ASCII_END; i++) {
		//Attempt to load the current character
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			//An error occurred loading the character so log an error
			Logger::log("Failed to load the character'" + StrUtils::str((char) i) + "' in the font face '" + name + "'", "Font", LogType::Error);
			return;
		}
		//Add onto the width
		width += glyphSlot->bitmap.width;
		//Update the height to make it the same as the tallest letter
		height = MathsUtils::max(height, glyphSlot->bitmap.rows);
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
		characters[i - ASCII_START].advanceX     = glyphSlot->advance.x;
		characters[i - ASCII_START].advanceY     = glyphSlot->advance.y;
		characters[i - ASCII_START].bitmapWidth  = glyphSlot->bitmap.width;
		characters[i - ASCII_START].bitmapHeight = glyphSlot->bitmap.rows;
		characters[i - ASCII_START].bitmapLeft   = glyphSlot->bitmap_left;
		characters[i - ASCII_START].bitmapTop    = glyphSlot->bitmap_top;
		characters[i - ASCII_START].xOffset      = xOffset;

		//Increment the x offset
		xOffset += glyphSlot->bitmap.width;
	}

	textureAtlas->applyParameters(false, true);

	//Return the GL_UNPACK_ALIGNMENT to its default state
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	FT_Done_Face(face);

	//Check whether this font is billboarded or not (i.e. Is it going to be used in the 3D world or not)
	if (billboarded) {
		//Create the GameObject3D instance and assign the texture
		object3D = new GameObject3D({ new Mesh(MeshBuilder::createQuad3D(textureAtlasWidth, textureAtlasHeight, textureAtlas, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS)) }, "BillboardedFont");
		object3D->getMaterial()->diffuseColour = colour;
		object3D->getMaterial()->diffuseTexture = textureAtlas;
		object3D->setScale(1.0f / (float) FONT_SCALE, 1.0f / (float) FONT_SCALE, 1.0f);
		object3D->update();
	} else {
		//Create the GameObject2D instance and assign the texture
		object2D = new GameObject2D({ new Mesh(MeshBuilder::createQuad(textureAtlasWidth, textureAtlasHeight, textureAtlas, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS)) }, "Font");
		object2D->getMaterial()->diffuseColour = colour;
		object2D->getMaterial()->diffuseTexture = textureAtlas;
		object2D->setScale(1.0f / (float) FONT_SCALE, 1.0f / (float) FONT_SCALE);
		object2D->update();
	}
}

void Font::update(std::string text) {
	//The pointer to the MeshData instance in the object
	MeshData* data;

	if (billboarded)
		data = object3D->getMeshes()[0]->getData();
	else
		data = object2D->getMeshes()[0]->getData();

	//Clear the previous data
	data->clearPositions();
	data->clearTextureCoords();
	data->clearIndices();

	//The current x and y positions
	float currentX = 0;
	float currentY = 0;

	if (billboarded) {
		currentX = -getWidth(text) / 2;
		currentY = getHeight(text) / 2;
	}

	unsigned int newLineCount = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0) {
			newLineCount ++;
			//Go to a new line
			currentX = 0;
			currentY += textureAtlasHeight;

			if (billboarded)
				currentX = -getWidth(text) / 2;
		} else {
			//Get the character data for the current character
			CharInfo info = characters[((int) text.at(i)) - ASCII_START];

			//The positions used for the vertices
			float xPos = currentX + info.bitmapLeft;
			float yPos = currentY - (info.bitmapHeight + (info.bitmapTop - info.bitmapHeight));

			float width = info.bitmapWidth;
			float height = info.bitmapHeight;

			if (billboarded) {
				data->addPosition(Vector3f(xPos, yPos, 0.0f));
				data->addPosition(Vector3f(xPos + width, yPos, 0.0f));
				data->addPosition(Vector3f(xPos + width, yPos + height, 0.0f));
				data->addPosition(Vector3f(xPos, yPos + height, 0.0f));
			} else {
				data->addPosition(Vector2f(xPos, yPos));
				data->addPosition(Vector2f(xPos + width, yPos));
				data->addPosition(Vector2f(xPos + width, yPos + height));
				data->addPosition(Vector2f(xPos, yPos + height));
			}

			data->addTextureCoord(Vector2f(info.xOffset / (float) textureAtlasWidth, 0.0f));
			data->addTextureCoord(Vector2f(((info.xOffset + info.bitmapWidth) / (float) textureAtlasWidth), 0.0f));
			data->addTextureCoord(Vector2f(((info.xOffset + info.bitmapWidth) / (float) textureAtlasWidth), info.bitmapHeight / (float) textureAtlasHeight));
			data->addTextureCoord(Vector2f(info.xOffset / (float) textureAtlasWidth, info.bitmapHeight / (float) textureAtlasHeight));

			unsigned int ip = (i - newLineCount) * 4;

			data->addIndex(ip + 0);
			data->addIndex(ip + 1);
			data->addIndex(ip + 2);
			data->addIndex(ip + 3);
			data->addIndex(ip + 0);
			data->addIndex(ip + 2);

			currentX += (info.advanceX / 64);
		}
	}
	if (billboarded) {
		object3D->getMeshes()[0]->getRenderData()->updatePositions(data);
		object3D->getMeshes()[0]->getRenderData()->updateTextureCoords();
		object3D->getMeshes()[0]->getRenderData()->updateIndices(data);
	} else {
		object2D->getMeshes()[0]->getRenderData()->updatePositions(data);
		object2D->getMeshes()[0]->getRenderData()->updateTextureCoords();
		object2D->getMeshes()[0]->getRenderData()->updateIndices(data);
	}
}

void Font::update(std::string text, Vector2f position) {
	update(text);

	if (! billboarded) {
		object2D->setPosition(position);
		object2D->update();
	}
}

void Font::update(std::string text, Vector3f position) {
	update(text);

	if (billboarded) {
		object3D->setPosition(position);
		object3D->update();
	}
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
		object2D->render();
}

void Font::destroy() {
	if (billboarded)
		delete object3D;
	else
		delete object2D;
}

float Font::getWidth(std::string text) {
	//The width
	float width = 0;
	//The width of the current line
	float lineWidth = 0;
	//Go through each character in the text
	for (unsigned int i = 0; i < text.length(); i++) {
		//Get the character data for the current character
		CharInfo info = characters[((int) text.at(i)) - ASCII_START];
		//Add onto the width
		lineWidth += (info.advanceX / 64);

		//Check for a new line escape character
		if (text.compare(i, 1, "\n") == 0)
			//Reset the width of the current line
			lineWidth = 0;

		//Find the maximum width
		width = MathsUtils::max(width, lineWidth);
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
			CharInfo info = characters[((int) text.at(i)) - ASCII_START];
			//Assign the height
			lineHeight = MathsUtils::max(lineHeight, info.bitmapHeight + (info.bitmapTop - info.bitmapHeight));
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

