/*****************************************************************************
 *
 *   Copyright 2017 Joel Davies
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

#include "../Vector.h"
#include "../render/Texture.h"

/*****************************************************************************
 * The CDLODQHeightMap class is used for obtaining data from a heightmap
 *****************************************************************************/

class CDLODHeightMap {
private:
	/* The data from the height map */
	unsigned char* data;

	/* The size of the height map (its width/height) */
	float size;

	/* The scale factor for the heights */
	float heightScale = 30.0f;

	/* The number of components per pixel */
	unsigned int numComponentsPerPixel;

	/* The texture instance for the height map */
	Texture* texture;

	/* Method used to setup this height map */
	void setup(unsigned char* data, int numComponents, int width, int height);

	/* Method used to get the 'normal' for a particular point */
	Vector3f calculateNormal(float x, float y);
public:
	/* The constructors */
	CDLODHeightMap(std::string path);
	CDLODHeightMap(unsigned char* data, int numComponents, int width, int height);

	/* The destructor */
	virtual ~CDLODHeightMap();

	/* Method used to get the height at a particular location (in world coordinates) */
	float getHeight(float x, float y);

	/* Method used to get the value at a particular location (where (0, 0) is the top left of the height map and (size - 1, size - 1) is the lower right) */
	float getValue(int mapX, int mapY);

	/* Returns a value between -1 and 1 indicating the 'steepness' of the height map at a particular
	 * location with 1 indicating the steepest increasing gradient and -1 the steepest decreasing gradient */
	float getSteepness(float x, float y, Vector3f direction);

	/* Methods used to get the minimum and maximum heights over a certain area of the height map */
	float getMinHeight(float x, float y, float areaSize);
	float getMaxHeight(float x, float y, float areaSize);

	/* Method used to check if a location is within the height map or not */
	bool isInMap(int mapX, int mapY);

	/* Setters and getters */
	inline void setHeightScale(float scale) { this->heightScale = scale; }

	inline unsigned int getSize() { return size; }
	inline float getHeightScale() { return heightScale; }
	inline Texture* getTexture() { return texture; }
};

