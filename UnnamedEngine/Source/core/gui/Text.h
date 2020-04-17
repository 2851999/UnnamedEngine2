/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "Font.h"

/*****************************************************************************
 * The Text class handles rendering of text using a Font
 *****************************************************************************/

class Text : public GameObject3D {
private:
	/* The current text this instance is setup to render */
	std::string currentText;

	/* Font instance used for this text object */
	Font* font;

	/* States whether this text is billboarded or not */
	bool billboarded = false;

	/* UBO and data structure for billboarding */
	UBO* shaderBillboardUBO;
	ShaderBlock_Billboard shaderBillboardData;

	/* The maximum number of characters this instance can render (only relevant for Vulkan) */
	unsigned int maxCharacters;

	/* The pipeline used to render this text */
	RenderPipeline* pipeline;
public:
	/* The default maximum number of characters */
	static unsigned int DEFAULT_MAX_CHARACTERS;

	/* The constructors */
	Text(Font* font, Colour colour = Colour::WHITE, unsigned int maxCharacters = 0, bool billboarded = false); //0 in maxCharacters indicates the default number should be used

	/* The destructor */
	virtual ~Text() {}

	/* Methods used to update this text instance ready to render some text */
	void update(std::string text);
	void update(std::string text, Vector2f position);
	void update(std::string text, Vector3f position);
	inline void update(std::string text, float x, float y, float z = 0.0f) { update(text, Vector3f(x, y, z)); }
	void update(Vector3f position);
	inline void update(Vector2f position) { update(Vector3f(position)); }
	inline void update(float x, float y, float z = 0.0f) { update(Vector3f(x, y, z)); }

	/* Method used to render the current text */
	virtual void render() override;

	/* Methods used to update and render some text */
	inline void render(std::string text, Vector2f position) {
		//Only update and render if there is text
		if (text.size() > 0) {
			update(text, position);
			render();
		}
	}

	inline void render(std::string text, float x, float y) {
		render(text, Vector2f(x, y));
	}

	inline void render(std::string text, Vector3f position) {
		//Only update and render if there is text
		if (text.size() > 0) {
			update(text, position);
			render();
		}
	}

	inline void render(std::string text, float x, float y, float z) {
		render(text, Vector3f(x, y, z));
	}

	/* Returns the width / height of the current text assigned to this object */
	inline float getWidth() { return font->getWidth(currentText); }
	inline float getHeigth() { return font->getHeight(currentText); }

	/* Other setters and getters */
	void setFont(Font* font);
	void setColour(Colour colour);

	inline Font* getFont() { return font; }
	inline Colour getColour() { return getMaterial()->getDiffuseColour(); }
};

