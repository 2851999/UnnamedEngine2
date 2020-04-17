/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "Light.h"
#include "RenderPipeline.h"

class RenderScene {
private:
	/* Objects within this scene */
	std::vector<GameObject3D*> objects;

	/* Lights in this scene */
	std::vector<Light*> lights;

	/* Boolean that states whether lighting should be used or not */
	bool lighting = true;

	/* The light batch descriptor set */
	DescriptorSet* descriptorSetLightBatch;

	/* The data to be sent to the shader about a light batch when doing lighting */
	ShaderBlock_LightBatch shaderLightBatchData;

	/* The pipelines required for rendering */
	RenderPipeline* pipelineMaterial;
	RenderPipeline* pipelineLighting;
public:
	/* The number of lights that can be rendered at once */
	static const unsigned int NUM_LIGHTS_IN_BATCH = 6;

	/* Constructor */
	RenderScene();

	/* Destructor */
	virtual ~RenderScene();

	/* Adds an object to the scene */
	void add(GameObject3D* object);

	/* Adds a light to the scene */
	inline void addLight(Light* light) { lights.push_back(light); }

	/* Method used to render all of the objects */
	void render();

	/* Setters and getters */
	inline void enableLighting() { this->lighting = true; }
	inline void disableLighting() { this->lighting = false; }
};
