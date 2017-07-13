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

#include "HeightMapGenerator.h"

/*****************************************************************************
 * The HeightMapGenerator class
 *****************************************************************************/

unsigned char* HeightMapGenerator::generate(int width, int height) {
	//The size of the data array
	int size = width * height;
	//The place to store the data
	unsigned char* data = new unsigned char[size];
	//Go through the x and y coordinates
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//Get the index in the data
			int index = y * height + x;
			//The x and y positions to use for the noise
			double nx = ((double) x / (double) width) - 0.5;
			double ny = ((double) y / (double) height) - 0.5;
			//Assign the data
			data[index] = (unsigned int) ((noiseGen.noise(nx, ny) + 1.0) / 2.0 * 255.0);
		}
	}
	//Return the data
	return data;
}
