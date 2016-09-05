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

#ifndef CORE_RENDER_RENDERSCENE_H_
#define CORE_RENDER_RENDERSCENE_H_

#include "../Object.h"
#include "Light.h"

/*****************************************************************************
 * The RenderScene3D class is used to help to manage the rendering of a set
 * of GameObject3D instances
 *****************************************************************************/

class RenderScene3D {
private:
	/* The GameObjects in this scene */
	std::vector<GameObject3D*> objects;

	/* The lights in this scene */
	std::vector<Light*> lights;

	/* Various shaders that might be needed */
	Shader* shadowMapShader;
	Shader* lightingShader;
public:
	/* The constructor */
	RenderScene3D();

	/* The destructor */
	virtual ~RenderScene3D();

	/* The method used to render all of the objects */
	void render();

	/* Used to add an object to this scene */
	inline void add(GameObject3D* object) { objects.push_back(object); }

	/* Used to add a light to this scene */
	inline void addLight(Light* light) { lights.push_back(light); }
};

#endif /* CORE_RENDER_RENDERSCENE_H_ */
