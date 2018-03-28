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

#include "CDLODHeightMap.h"

#include "../../utils/Logging.h"
#include "../../utils/Utils.h"

/*****************************************************************************
 * The CDLODHeightMap class
 *****************************************************************************/

CDLODHeightMap::CDLODHeightMap(std::string path) {
	//Places to store data about the height map texture
	int numComponents, w, h, format;
	//Load the height map data
	this->data = Texture::loadTexture(path, numComponents, w, h, format);
	//Setup this height map
	setup(data, numComponents, w, h, format);
}

CDLODHeightMap::CDLODHeightMap(unsigned char* data, int numComponents, int width, int height, int format) {
	this->data = data;
	//Setup this height map
	setup(data, numComponents, width, height, format);
}

CDLODHeightMap::~CDLODHeightMap() {
	//Free the texture data
	Texture::freeTexture(data);
}

void CDLODHeightMap::setup(unsigned char* data, int numComponents, int width, int height, int format) {
	//Ensure the data is valid i.e. the height map is a square with sides a power of 2
	if (width == height && width != 0 && (width & (width - 1)) == 0) {
		//Assign the width and height values
		this->size = width;
		//Assign the number of components per pixel
		this->numComponentsPerPixel = numComponents;
		//Create the texture instance
		TextureParameters p;
		p.setFilter(GL_LINEAR);
		p.setClamp(GL_REPEAT);
		p.setShouldClamp(true);
		this->texture = Texture::createTexture("", data, numComponents, width, width, format);
	} else {
		//Output an error message
		Logger::log("HeightMap is invalid", "CDLODHeightMap", LogType::Error);
	}
}

float CDLODHeightMap::getHeight(float x, float y) {
	//The x and y locations in the map
	float mapX = (x + size / 2.0f);
	float mapY = (y + size / 2.0f);

	//The maximum/minimum grid values possible
	int mapMaxX = (int) ceil(mapX);
	int mapMinX = (int) floor(mapX);
	int mapMaxY = (int) ceil(mapY);
	int mapMinY = (int) floor(mapY);

	//Height values at each corner of the potential quad
	float v1 = getValue(mapMinX, mapMaxY);
	float v2 = getValue(mapMaxX, mapMaxY);
	float v3 = getValue(mapMinX, mapMinY);
	float v4 = getValue(mapMaxX, mapMinY);

	//Interpolate in the x direction
	float fxy1;
	if (mapMaxX == mapMinX)
		fxy1 = v3;
	else
		fxy1 = ((mapMaxX - mapX)/(mapMaxX - mapMinX)) * v3 + ((mapX - mapMinX)/(mapMaxX - mapMinX)) * v4;

	float fxy2;
	if (mapMaxX == mapMinX)
		fxy2 = v1;
	else
		fxy2 = ((mapMaxX - mapX)/(mapMaxX - mapMinX)) * v1 + ((mapX - mapMinX)/(mapMaxX - mapMinX)) * v2;

	//Interpolate in y direction
	float height;
	if (mapMaxY == mapMinY)
		height = fxy1;
	else
		height = ((mapMaxY - mapY)/(mapMaxY - mapMinY)) * fxy1 + ((mapY - mapMinY)/(mapMaxY - mapMinY)) * fxy2;

	//Return the height
	return height;
}

//If changed to float's can interpolate or something
float CDLODHeightMap::getValue(int mapX, int mapY) {
	//Ensure the position is valid within the terrain
	if (isInMap(mapX, mapY)) {
		//Calculate the index of the pixel in the texture that needs to be looked up
		unsigned int pixelIndex = (int) ((mapY * size) + mapX);
		//Calculate the actual index
		unsigned int valueIndex = pixelIndex * numComponentsPerPixel;
		//Get the height value and return it
		return ((((float) data[valueIndex]) / 255.0f) - 0.5f) * heightScale;
	} else {
		Logger::log("Location (" + utils_string::str(mapX) + "," + utils_string::str(mapY) + ") is outside of the height map", "CDLODHeightMap", LogType::Debug);
		return 0.0f;
	}
}

float CDLODHeightMap::getMinHeight(float x, float y, float areaSize) {
	//The minimum value
	float min = 9999999.0f;
	//The starting and ending values
	float startX = x - areaSize / 2.0f;
	float endX   = x + areaSize / 2.0f;
	float startY = y - areaSize / 2.0f;
	float endY   = y + areaSize / 2.0f;

	//Go through each coordinate in the area (taking into account the nodeSize)
	for (float currentY = startY; currentY <= endY; currentY++) {
		for (float currentX = startX; currentX <= endX; currentX++) {
			//Assign the value
			min = utils_maths::min(min, getHeight(currentX, currentY));
		}
	}
	//Return the value
	return min;
}

float CDLODHeightMap::getMaxHeight(float x, float y, float areaSize) {
	//The maximum value
	float max = -9999999.0f;
	//The starting and ending values
	float startX = x - areaSize / 2.0f;
	float endX   = x + areaSize / 2.0f;
	float startY = y - areaSize / 2.0f;
	float endY   = y + areaSize / 2.0f;
	//Go through each coordinate in the area (taking into account the nodeSize)
	for (float currentY = startY; currentY <= endY; currentY++) {
		for (float currentX = startX; currentX <= endX; currentX++) {
			//Assign the value
			max = utils_maths::max(max, getHeight(currentX, currentY));
		}
	}
	//Return the value
	return max;
}

bool CDLODHeightMap::isInMap(int mapX, int mapY) {
	return mapX >= 0.0f && mapX <= size && mapX >= 0.0f && mapX <= size;
}

