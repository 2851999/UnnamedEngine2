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

#include "TextureAtlas.h"

/*****************************************************************************
 * The TextureAtlas class
 *****************************************************************************/

TextureAtlas::TextureAtlas(Texture* texture, unsigned int numColumns, unsigned int numRows, unsigned int numTextures) {
	this->texture = texture;
	this->numColumns = numColumns;
	this->numRows = numRows;
	this->numTextures = numTextures;

	textureWidth = (float) texture->getWidth() / (float) numColumns;
	textureHeight = (float) texture->getHeight() / (float) numRows;
}

void TextureAtlas::getSides(unsigned int textureIndex, float& top, float& left, float& bottom, float& right) {
	//Coordinates of the tile in pixels
	float x = (textureIndex % numColumns) * textureWidth;
	float y = (textureIndex / numColumns) * textureHeight;

	//Apply the x and y offsets
	x += offsets.left;
	y += offsets.top;

	//Assign the texture coordinates
	top = y / (float) texture->getHeight();
	left = x / (float) texture->getWidth();
	bottom =  top + (getSubTextureHeight() / (float) texture->getHeight());
	right = left + (getSubTextureWidth() / (float) texture->getWidth());
}
