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
#include "RenderPass.h"

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

	/* Boolean that states whether deferred rendering should be used or not */
	bool deferred;

	/* Boolean that states whether SSR should be used or not */
	bool ssr;

	/* Boolean that states whether post processing should be used or not */
	bool postProcessing;

	/* The ambient light used in phong shading */
	Colour ambientLight = Colour(0.01f, 0.01f, 0.01f);

	/* The light batch descriptor sets */
	std::vector<DescriptorSet*> descriptorSetLightBatches;

	/* The data to be sent to the shader about a light batch when doing lighting */
	ShaderBlock_LightBatch shaderLightBatchData;

	/* The data to be sent to the gama correction/FXAA shader */
	ShaderBlock_GammaCorrectionFXAA shaderGammaCorrectionFXAAData;

	/* Desriptor set for gamma correction/FXAA data*/
	DescriptorSet* descriptorSetGammaCorrectionFXAA;

	/* The graphics pipelines required for rendering */
	GraphicsPipeline* pipelineMaterial;
	GraphicsPipeline* pipelineLighting;
	GraphicsPipeline* pipelineLightingBlend;
	GraphicsPipeline* pipelineLightingSkinning;
	GraphicsPipeline* pipelineLightingSkinningBlend;

	GraphicsPipeline* pipelineGammaCorrectionFXAA;
	GraphicsPipeline* pipelineDeferredLightingGeometry;
	GraphicsPipeline* pipelineDeferredLightingSkinningGeometry;
	GraphicsPipeline* pipelineDeferredLighting;
	GraphicsPipeline* pipelineDeferredLightingBlend;
	GraphicsPipeline* pipelineDeferredSSR;

	/* Meshes for rendering to the screen */
	Mesh* screenTextureMesh;
	Mesh* deferredRenderingScreenTextureMesh;

	/* Graphics pipeline for rendering the final quad */
	GraphicsPipeline* pipelineFinal;

	/* Deferred geometry render pass */
	RenderPass* deferredGeometryRenderPass = NULL;

	/* SSR render pass */
	RenderPass* deferredPBRSSRRenderPass = NULL;

	/* Post processing render pass */
	RenderPass* postProcessingRenderPass = NULL;

	/* Descriptor set for the geometry buffer */
	DescriptorSet* descriptorSetGeometryBuffer;

	/* Descriptor set for the geometry buffer SSR */
	DescriptorSet* descriptorSetGeometryBufferSSR;

	/* Method used to render this scene (Ignoring any post processing) */
	void renderScene();
public:
	/* The number of lights that can be rendered at once */
	static const unsigned int NUM_LIGHTS_IN_BATCH = 6;

	/* Constructor */
	RenderScene(bool deferred, bool pbr, bool ssr, bool postProcessing);

	/* Destructor */
	virtual ~RenderScene();

	/* Adds an object to the scene */
	void add(GameObject3D* object);

	/* Adds a light to the scene */
	void addLight(Light* light);

	/* Method used to do any rendering without the default render pass */
	void renderOffscreen();

	/* Method used to render all of the objects */
	void render();

	/* Setters and getters */
	inline void enableLighting() { this->lighting = true; }
	inline void disableLighting() { this->lighting = false; }
	inline void setAmbientLight(Colour ambientLight) { this->ambientLight = ambientLight; }
	void setPostProcessingParameters(bool gammaCorrection, bool fxaa, float exposureIn = -1.0f);

	inline Colour getAmbientLight() { return ambientLight; }
};
