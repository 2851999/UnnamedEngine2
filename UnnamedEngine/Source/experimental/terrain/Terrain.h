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

#ifndef EXPERIMENTAL_TERRAIN_TERRAIN_H_
#define EXPERIMENTAL_TERRAIN_TERRAIN_H_

#include "../../core/Object.h"

class Terrain : public GameObject3D {
public:
	/* The constructor */
	Terrain() {}

	/* The destructor */
	virtual ~Terrain() {}

	/* Method used to setup and create the terrain */
	void setup(std::string filePath, unsigned int bitsPerPixel);

	virtual bool shouldCull() override { return false; }
};


#endif /* EXPERIMENTAL_TERRAIN_TERRAIN_H_ */
