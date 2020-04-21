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

#include "../../utils/SimplexNoise.h"

/*****************************************************************************
 * The HeightMapGenerator class is used to procedurally generate the data for
 * a height map
 *****************************************************************************/

class HeightMapGenerator {
private:
	/* Noise generator */
	SimplexNoise noiseGen;

	/* A 'seed' for modifying the output */
	int seed = 42;

	/* A scale for modifying the output */
	double scale = 0.015;

	/* Method used to add together noise at different frequencies */
	double noiseOctaves(int iterations, int x, int y, float persistence, float low, float high);
public:
	/* The constructor */
	HeightMapGenerator() {}

	/* The destructor */
	virtual ~HeightMapGenerator() {}

	/* Method used to create a height map and return its data */
	unsigned char* generate(int width, int height);

	/* Setters and getters */
	inline void setSeed(int seed) { this->seed = seed; }
	inline void setScale(double scale) { this->scale = scale; }

	inline int getSeed() { return seed; }
	inline double getScale() { return scale; }
};

