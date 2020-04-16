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

#include "Light.h"
#include "GeometryBuffer.h"
#include "pbr/PBREnvironment.h"
#include "../Object.h"
#include "PostProcessing.h"

/*****************************************************************************
 * The RenderScene3D class is used to help to manage the rendering of a set
 * of GameObject3D instances
 *****************************************************************************/

class RenderScene3D {
private:
	/* Structure used to group together objects with the same shader */
	struct RenderBatch {
		RenderShader* shader;
		std::vector<GameObject3D*> objects;
	};

	/* The RenderBatches in this scene */
	std::vector<RenderBatch> batches;

	/* The lights in this scene */
	std::vector<Light*> lights;

	/* Various shaders that might be needed */
	Shader* shadowMapShader;
	Shader* shadowCubemapShader;

	/* The ambient light used in lighting */
	Colour ambientLight = Colour(0.01f, 0.01f, 0.01f);

	/* Boolean to determine whether lighting should be used or not */
	bool lighting = true;

	/* States whether deferred rendering should be used */
	bool deferredRendering = false;

	/* The geometry buffer used in deferred rendering */
	GeometryBuffer* gBuffer = NULL;

	/* States whether the geometry pass is being rendered */
	bool geometryPass = false;

	/* States whether the skybox should be used as an environment map */
	bool useEnvironmentMap = false;

	/* States whether PBR should be used */
	bool pbr = false;

	/* States whether bloom should be used */
	bool bloom = false;

	/* States whether screen space reflections should be used */
	bool ssr = false;

	/* The environment used for PBR */
	PBREnvironment* pbrEnvironment = NULL;

	/* Framebuffer for storing lighting output with a 'bright' texture for bloom */
	FBO* lightingFramebuffer;

	/* Post processor used for applying bloom */
	PostProcessor* postProcessorBloom;

	/* Post processor used for applying a gaussian blur bloom */
	PostProcessor* postProcessorBlur1;
	PostProcessor* postProcessorBlur2;

	/* Post processor used for applying screen space reflections */
	PostProcessor* postProcessorSSR;

	/* Post processor used for applying gamma correction */
	PostProcessor* postProcessor = NULL;

	/* Intermediate FBO used for antialiasing */
	PostProcessor* intermediateFBO = NULL;

	/* States whether the scene should be rendered in wireframe mode */
	bool renderWireframe = false;

	/* The lighting data UBO for the shaders */
	UBO* shaderLightingUBO;

	/* The structure used to update the lighting data UBO */
	ShaderBlock_LightBatch shaderLightingData;

	/* The gamma correction UBO for the shaders */
	UBO* shaderGammaCorrectionUBO;

	/* The structure used to update the gamma correction data UBO */
	ShaderBlock_GammaCorrection shaderGammaCorrectionData;

	/* The PBR lighting core UBO for the shaders */
	UBO* shaderPBRLightingCoreUBO;

	/* The structure used to update the PBR lighting core data UBO */
	ShaderBlock_PBRLightingCore shaderPBRLightingCoreData;

	/* The shadow cubemap UBO for the shaders */
	UBO* shaderShadowCubemapUBO;

	/* The structure used to update the shadow cubemap data UBO */
	ShaderBlock_ShadowCubemap shaderShadowCubemapData;

	/* Methods used before and after actual rendering of the scene when using forward rendering to handle the
	 * framebuffers/gamma correction */
	void forwardPreRender();
	void forwardPostRender();

	/* Used to render the lighting pass given the shader to use (and index of the batch to render for forward rendering) */
	void renderLighting(RenderShader* renderShader, int indexOfBatch = -1);

	/* Used to render the scene to the shadow maps */
	void renderShadowMaps();

	/* Used to render the scene to a light's shadow map */
	void renderShadowMap(Light* light);
public:
	/* The number of lights in each set */
	static const unsigned int NUM_LIGHTS_IN_SET = 6;

	/* The constructor */
	RenderScene3D();

	/* The destructor */
	virtual ~RenderScene3D();

	/* Method used to enable deferred rendering */
	void enableDeferred();

	/* Method used to enable pbr rendering */
	inline void enablePBR() { pbr = true; }

	/* The method used to render all of the objects */
	void render();

	/* Used to add an object to this scene */
	void add(GameObject3D* object);

	/* Used to add a light to this scene */
	inline void addLight(Light* light) { lights.push_back(light); }

	/* Displays the various buffers on the screen used for deferred rendering */
	void showDeferredBuffers();

	/* Getters and setters */
	inline void setAmbientLight(Colour ambientLight) { this->ambientLight = ambientLight; }
	inline void enableLighting() { lighting = true; }
	inline void disableLighting() { lighting = false; }
	inline void enableWireframe() { renderWireframe = true; }
	inline void disableWireframe() { renderWireframe = false; }
	inline void enableSSR() { ssr = true; }
	inline void disableSSR() { ssr = false; }
	inline void enableBloom() { bloom = true; }
	inline void disableBloom() { bloom = false; }
	inline void setPBREnvironment(PBREnvironment* environment) { this->pbrEnvironment = environment; }
	inline Colour getAmbientLight() { return ambientLight; }
	inline bool isLightingEnabled() { return lighting; }
	inline PBREnvironment* getPBREnvironment() { return pbrEnvironment; }
	inline bool hasObjects() { return batches.size() > 0; }

	/* Used to apply some post processing options */
	void enableGammaCorrection();
	void disableGammaCorrection();
	void setExposure(float exposure);
	void enableFXAA();
	void disableFXAA();
};

