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
#include "../render/Mesh.h"

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

	FT_Face face;

	/* Information about the characters */
	struct CharInfo {
		float advanceX;
		float advanceY;
		float bitmapWidth;
		float bitmapHeight;
		float bitmapLeft;
		float bitmapTop;
		float xOffset;
	} characters[(ASCII_END - ASCII_START) + 1] ;

	/* The width/height of then texture atlas */
	unsigned int textureAtlasWidth  = 0;
	unsigned int textureAtlasHeight = 0;

	/* States whether this font is billboarded or not */
	bool billboarded = false;

	/* The GameObject2D instance for this font */
	GameObject2D* object2D = NULL;

	/* The GameObject3D instance for this font (For 3D billboarded text rendering) */
	GameObject3D* object3D = NULL;

	/* The method used to setup this Font instance given the font name and size */
	void setup(std::string name, unsigned int size, Colour colour, TextureParameters parameters);

	/* Methods used to update this font ready to render some text */
	void update(std::string text);
public:
	/* The constructor */
	Font(std::string name, unsigned int size = 18, Colour colour = Colour::WHITE, TextureParameters parameters = TextureParameters()) { setup(name, size, colour, parameters); }
	Font(std::string name, unsigned int size, Colour colour, bool billboarded, TextureParameters parameters = TextureParameters()) : billboarded(billboarded) { setup(name, size, colour, parameters); }
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
