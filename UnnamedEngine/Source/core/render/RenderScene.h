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
#include "GraphicsPipeline.h"
#include "../vulkan/VulkanRenderPass.h"

class RenderScene {
private:
	/* Objects within this scene */
	std::vector<GameObject3D*> objects;

	/* Skinned objects within this scene */
	std::vector<GameObject3D*> skinnedObjects;

	/* Lights in this scene */
	std::vector<Light*> lights;

	/* Boolean that states whether lighting should be used or not */
	bool lighting = true;

	/* The ambient light used in phong shading */
	Colour ambientLight = Colour(0.01f, 0.01f, 0.01f);

	/* The light batch descriptor sets */
	std::vector<DescriptorSet*> descriptorSetLightBatches;

	/* The data to be sent to the shader about a light batch when doing lighting */
	ShaderBlock_LightBatch shaderLightBatchData;

	/* The graphics pipelines required for rendering */
	GraphicsPipeline* pipelineMaterial;
	GraphicsPipeline* pipelineLighting;
	GraphicsPipeline* pipelineLightingBlend;
	GraphicsPipeline* pipelineLightingSkinning;
	GraphicsPipeline* pipelineLightingSkinningBlend;

	/* Mesh for rendering to the screen */
	Mesh* screenTextureMesh;

	/* Graphics pipeline for rendering the final quad */
	GraphicsPipeline* pipelineFinal;

	/* Final render pass */
	VulkanRenderPass* finalRenderPass;
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
	void addLight(Light* light);

	/* Method used to render all of the objects */
	void render();

	/* Setters and getters */
	inline void enableLighting() { this->lighting = true; }
	inline void disableLighting() { this->lighting = false; }
	inline void setAmbientLight(Colour ambientLight) { this->ambientLight = ambientLight; }

	inline Colour getAmbientLight() { return ambientLight; }
};
