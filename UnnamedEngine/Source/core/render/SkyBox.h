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

#ifndef CORE_RENDER_SKYBOX_H_
#define CORE_RENDER_SKYBOX_H_

#include "Cubemap.h"
#include "../Object.h"

/*****************************************************************************
 * The SkyBox class is used to create and render a SkyBox
 *****************************************************************************/

class SkyBox {
private:
	/* The skybox texture */
	Cubemap* cubemap;

	/* The skybox */
	GameObject3D* box;
public:
	/* The constructor */
	SkyBox(std::string path, std::string front, std::string back, std::string left, std::string right, std::string top, std::string bottom, float size);

	/* The destructor */
	virtual ~SkyBox() { destroy(); }

	/* Method to update this skybox's position */
	void update(Vector3f cameraPosition);
	/* Method to render this skybox */
	void render();

	/* Method to free up all resources used by this skybox */
	void destroy();

	/* Getters */
	inline Cubemap* getCubemap() { return cubemap; }
};

#endif /* CORE_RENDER_SKYBOX_H_ */
