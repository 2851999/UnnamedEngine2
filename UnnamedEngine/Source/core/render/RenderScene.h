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
#include "pbr/PBREnvironment.h"

 /*****************************************************************************
  * The RenderScene class handles the rendering of a set of objects
  *****************************************************************************/

class RenderScene {
private:
	/* Structure containing a batch of objects that need to be rendered, along with the GraphicsPipeline required to do it */
	struct ObjectBatch {
		/* States whether this batch uses skinning */
		bool skinning;

		/* Pipelines for rendering */
		GraphicsPipeline* graphicsPipeline = NULL;
		GraphicsPipeline* graphicsPipelineBlend = NULL; //May be NULL if deferred geometry

		/* The objects to be rendered with the above pipeline */
		std::vector<GameObject3D*> objects;
	};

	/* The object batches containing all of the scene objects to be rendered */
	std::unordered_map<unsigned int, ObjectBatch> objectBatches;

	/* Lights in this scene */
	std::vector<Light*> lights;

	/* Boolean that states whether lighting should be used or not */
	bool lighting = true;

	/* Boolean that states whether deferred rendering should be used or not */
	bool deferred;

	/* Boolean that states whether PBR should be used or not */
	bool pbr;

	/* Boolean that states whether SSR should be used or not */
	bool ssr;

	/* Boolean that states whether bloom should be used or not */
	bool bloom;

	/* Boolean that states whether post processing should be used or not */
	bool postProcessing;

	/* PBREnvironment instance (For IBL) */
	PBREnvironment* pbrEnvironment = NULL;

	/* Descriptor set passing the environment to the shader */
	DescriptorSet* descriptorSetPBREnvironment = NULL;

	/* The ambient light used in phong shading/when using pbr this can control the
	   contribution ambient lighting has */
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
	GraphicsPipeline* pipelineGammaCorrectionFXAA;
	GraphicsPipeline* pipelineDeferredLighting;
	GraphicsPipeline* pipelineDeferredLightingBlend;
	GraphicsPipeline* pipelineGaussianBlur1;
	GraphicsPipeline* pipelineGaussianBlur2;
	GraphicsPipeline* pipelineBloomCombine;
	GraphicsPipeline* pipelineDeferredSSR;

	/* Meshes for rendering to the screen */
	Mesh* screenTextureMesh;
	Mesh* deferredRenderingScreenTextureMesh;
	Mesh* bloomSSRScreenTextureMesh; //Renders to SSR pass with bloom info
	/* These render to the gaussian blur pass, where
	   1 - uses attachment from lighting framebuffer
	   2 - uses attachment from gaussianBlur1RenderPass1
	   3 - uses attachment from gaussianBlur1RenderPass2 */
	Mesh* gaussianBlurBloomScreenTextureMesh1;
	Mesh* gaussianBlurBloomScreenTextureMesh2;
	Mesh* gaussianBlurBloomScreenTextureMesh3;

	/* Graphics pipeline for rendering the final quad */
	GraphicsPipeline* pipelineFinal;

	/* Deferred geometry render pass */
	RenderPass* deferredGeometryRenderPass = NULL;

	/* Bloom render pass -> rendering to normal/bright textures */
	RenderPass* deferredBloomRenderPass = NULL;

	/* Render passes for gaussian blur */
	RenderPass* gaussianBlur1RenderPass = NULL;
	RenderPass* gaussianBlur2RenderPass = NULL;

	/* SSR render pass */
	RenderPass* deferredPBRSSRRenderPass = NULL;

	/* Post processing render pass */
	RenderPass* postProcessingRenderPass = NULL;

	/* Descriptor set for the geometry buffer */
	DescriptorSet* descriptorSetGeometryBuffer;

	/* Descriptor sets for gaussian blur */
	ShaderBlock_GaussianBlur gaussianBlurData[2];
	DescriptorSet* descriptorSetsGaussianBlur[2];

	/* Number of blurs to execute for bloom */
	unsigned int gaussianBlurAmount = 6;

	/* Descriptor set for the geometry buffer to be used for the SSR shader*/
	DescriptorSet* descriptorSetGeometryBufferSSR;

	/* Returns a MeshData* instance for rendering to the screen */
	MeshData* createScreenMeshData();

	/* Method used to render this scene (Ignoring any post processing) */
	void renderScene();
public:
	/* The number of lights that can be rendered at once */
	static const unsigned int NUM_LIGHTS_IN_BATCH = 6;

	/* Constructor */
	RenderScene(bool deferred, bool pbr, bool ssr, bool bloom, bool postProcessing, PBREnvironment* pbrEnvironment = NULL);

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
	inline bool hasObjects() { return objectBatches.size() > 0; }
};
