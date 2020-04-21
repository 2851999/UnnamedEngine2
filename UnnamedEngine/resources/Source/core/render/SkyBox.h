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

#pragma once

#include "Texture.h"
#include "../Object.h"

/*****************************************************************************
 * The SkyBox class is used to create and render a SkyBox
 *****************************************************************************/

class SkyBox {
private:
	/* The graphics pipeline required to render the skybox */
	GraphicsPipeline* pipelineSkybox;

	/* The skybox texture */
	Cubemap* cubemap;

	/* The skybox */
	GameObject3D* box;
public:
	/* The constructors */
	SkyBox(Cubemap* cubemap);
	SkyBox(std::string path, std::string front, std::string back, std::string left, std::string right, std::string top, std::string bottom) : SkyBox(new Cubemap(path, { right, left, top, bottom, back, front })) {}
	SkyBox(std::string path, std::string fileExtension) : SkyBox(path, "front" + fileExtension, "back" + fileExtension, "left" + fileExtension, "right" + fileExtension, "top" + fileExtension, "bottom" + fileExtension) {}

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

