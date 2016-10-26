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

#ifndef CORE_RENDER_TEXTUREATLAS_H_
#define CORE_RENDER_TEXTUREATLAS_H_

#include "Texture.h"

/*****************************************************************************
 * The TextureAtlas class manages a texture atlas
 *****************************************************************************/

class TextureAtlas {
private:
	/* The texture */
	Texture* texture;
public:
	/* The number of textures in this atlas */
	unsigned int numTextures = 0;

	/* The number of columns/rows of textures in this atlas */
	unsigned int numColumns = 0;
	unsigned int numRows    = 0;

	/* The width/height of a texture within this atlas */
	float textureWidth;
	float textureHeight;

	/* The constructor */
	TextureAtlas(Texture* texture, unsigned int numColumns, unsigned int numRows, unsigned int numTextures);

	/* The method used to get the data for the sides of the texture */
	void getSides(unsigned int textureIndex, float& top, float& left, float& bottom, float& right);

	/* Getters */
	inline Texture* getTexture() { return texture; }
};

#endif /* CORE_RENDER_TEXTUREATLAS_H_ */
