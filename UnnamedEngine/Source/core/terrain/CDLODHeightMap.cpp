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
	//Ensure the data is valid i.e. the height map is a square with sides a power of 2
	if (w == h && w != 0 && (w & (w - 1)) == 0) {
		//Assign the width and height values
		this->size = w;
		//Assign the number of components per pixel
		this->numComponentsPerPixel = numComponents;
		//Create the texture instance
		TextureParameters p;
		p.setFilter(GL_LINEAR);
		p.setClamp(GL_REPEAT);
		p.setShouldClamp(true);
		this->texture = Texture::createTexture(path, data, numComponents, w, h, format);
	} else {
		//Output an error message
		Logger::log("HeightMap with the path " + path + " is invalid", "CDLODHeightMap", LogType::Error);
	}
}

CDLODHeightMap::~CDLODHeightMap() {
	//Free the texture data
	Texture::freeTexture(data);
}

float CDLODHeightMap::getHeight(float x, float y) {
	//Calculate the coordinates in the height map for the location (with the centre at (0,0))
	int mapX = (int) (x + size / 2.0f);
	int mapY = (int) (y + size / 2.0f);
	//Return the height
	return getValue(mapX, mapY);
}

//If changed to float's can interpolate or something
float CDLODHeightMap::getValue(int mapX, int mapY) {
	//Ensure the position is valid within the terrain
	if (mapX >= 0.0f && mapX <= size && mapY >= 0.0f && mapY <= size) {
		//Calculate the index of the pixel in the texture that needs to be looked up
		unsigned int pixelIndex = (int) ((mapY * size) + mapX);
		//Calculate the actual index
		unsigned int valueIndex = pixelIndex * numComponentsPerPixel;
		//Get the height value and return it
		return (((float) data[valueIndex]) / 255.0f) * 10.0f;
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
	for (float currentY = startY; currentY < endY; currentY++) {
		for (float currentX = startX; currentX < endX; currentX++) {
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
	for (float currentY = startY; currentY < endY; currentY++) {
		for (float currentX = startX; currentX < endX; currentX++) {
			//Assign the value
			max = utils_maths::max(max, getHeight(currentX, currentY));
		}
	}
	//Return the value
	return max;
}

