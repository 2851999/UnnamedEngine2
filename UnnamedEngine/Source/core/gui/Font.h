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

#ifndef CORE_GUI_FONT_H_
#define CORE_GUI_FONT_H_

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

	/* The width/height of then texture atlas */
	unsigned int textureAtlasWidth  = 0;
	unsigned int textureAtlasHeight = 0;

	/* States whether this font is billboarded or not */
	bool billboarded = false;

	/* The GameObject3D instance for this font */
	GameObject3D* object3D = NULL;

	/* The method used to setup this Font instance given the font name and size */
	void setup(std::string path, unsigned int size, Colour colour, TextureParameters parameters);

	/* Methods used to update this font ready to render some text */
	void update(std::string text);
public:
	/* The constructor */
	Font(std::string path, unsigned int size = 18, Colour colour = Colour::WHITE, TextureParameters parameters = TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST)) { setup(path, size, colour, parameters); }
	Font(std::string path, unsigned int size, Colour colour, bool billboarded, TextureParameters parameters = TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST)) : billboarded(billboarded) { setup(path, size, colour, parameters); }
	virtual ~Font() { destroy(); }

	/* Methods used to update this font ready to render some text */
	void update(std::string text, Vector2f position);
	inline void update(std::string text, float x, float y) { update(text, Vector2f(x, y)); }
	void update(std::string text, Vector3f position);
	inline void update(std::string text, float x, float y, float z) { update(text, Vector3f(x, y, z)); }

	/* Method used to render the current text */
	void render();

	/* Methods used to update and render some text */
	inline void render(std::string text, Vector2f position) {
		update(text, position);
		render();
	}

	inline void render(std::string text, float x, float y) {
		render(text, Vector2f(x, y));
	}

	inline void render(std::string text, Vector3f position) {
		update(text, position);
		render();
	}

	inline void render(std::string text, float x, float y, float z) {
		render(text, Vector3f(x, y, z));
	}

	/* Method used to release all of the resources this font holds */
	void destroy() override;

	/* Methods used to get the width/height of text rendered with this font */
	float getWidth(std::string text);
	float getHeight(std::string text);

	/* The static method used to initialise FreeType for use */
	static void initialiseFreeType();
	/* The static method used to release resources used by FreeType */
	static void destroyFreeType();
};

#endif /* CORE_GUI_FONT_H_ */
