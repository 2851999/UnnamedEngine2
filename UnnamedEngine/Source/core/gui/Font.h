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

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Object.h"

/*****************************************************************************
 * The Font class handles font rendering
 *****************************************************************************/

class Font : public Resource {
private:
	/* The starting/ending ASCII codes used when creating a font */
	static const unsigned int ASCII_START = 32;
	static const unsigned int ASCII_END   = 126;

	/* A spacing value (in pixels) to separate glyphs in the bitmap image */
	static const unsigned int GLYPH_SPACING = 1;

	/* The FreeType library */
	static FT_Library ftLibrary;

	/* Information about the glyphs */
	struct GlyphInfo {
		float advanceX;
		float advanceY;
		float glyphWidth;
		float glyphHeight;
		float glyphLeft;
		float glyphTop;
		float xOffset;
	} glyphs[(ASCII_END - ASCII_START) + 1];

	/* The texture this font */
	Texture* texture;

	/* The method used to setup this Font instance given the font name and size */
	void setup(std::string path, unsigned int size, TextureParameters parameters);
public:
	/* Render scale used to render font at a higher size and then down scale later */
	static const float RENDER_SCALE;

	/* The constructors */
	Font(std::string path, unsigned int size = 18, TextureParameters parameters = TextureParameters().setClamp(GL_CLAMP_TO_EDGE).setFilter(GL_NEAREST)) { setup(path, size, parameters); }
	/* The destructors */
	virtual ~Font() {} //Destroy should be called when resource is released

	/* Method to assign a MeshData instance to render some text */
	void assignMeshData(MeshData* data, std::string text, bool billboarded);

	/* Method used to release all of the resources this font holds */
	void destroy() override;

	/* Methods used to get the width/height of text rendered with this font */
	float getWidth(std::string text);
	float getHeight(std::string text);

	/* The static method used to initialise FreeType for use */
	static void initialiseFreeType();
	/* The static method used to release resources used by FreeType */
	static void destroyFreeType();

	/* Setters and getters */
	inline Texture* getTexture() { return texture; }
};

