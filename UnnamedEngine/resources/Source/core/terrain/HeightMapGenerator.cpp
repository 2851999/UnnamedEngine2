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

#include <math.h>

/*****************************************************************************
 * The HeightMapGenerator class
 *****************************************************************************/

double HeightMapGenerator::noiseOctaves(int iterations, int x, int y, float persistence, float low, float high) {
	double maxAmplitude = 0;
	double amplitude = 1;
	double frequency = scale;
	double noise = 0;

	for (int i = 0; i < iterations; i++) {
		noise += noiseGen.noise(x * frequency, y * frequency) * amplitude;
		maxAmplitude += amplitude;
		amplitude *= persistence;
		frequency *= 2;
	}

	noise /= maxAmplitude;
	noise = noise * (high - low) / 2 + (high + low) / 2;

	return noise;
}

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
			//The x and y positions to use
			double nx = seed + x;
			double ny = seed + y;
			//Assign the data
			data[index] = (unsigned int) noiseOctaves(16, nx, ny, 0.5, 0, 255);
		}
	}
	//Return the data
	return data;
}
