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

#include "Light.h"
#include "GeometryBuffer.h"
#include "../Object.h"

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

	/* The ambient light used in lighting */
	Colour ambientLight = Colour(0.01f, 0.01f, 0.01f);

	/* Boolean to determine whether lighting should be used or not */
	bool lightingEnabled = true;

	/* States whether deferred rendering should be used */
	bool deferredRendering = false;

	/* The geometry buffer used in deferred rendering */
	GeometryBuffer* gBuffer = NULL;

	/* States whether the geometry pass is being rendered */
	bool geometryPass = false;

	/* States whether the skybox should be used as an environment map */
	bool useEnvironmentMap = false;

	/* Method used to perform rendering for deferred and forward rendering with lighting */
	void renderWithLights(unsigned int uniformNumLights, Colour uniformLightAmbient, unsigned int lightStartIndex);

	/* Method used to set the lighting uniforms common to both forward and deferred lighting */
	void setLightingUniforms(Shader* shader, unsigned int uniformNumLights, Colour uniformLightAmbient);

	/* Method used to set all of the uniforms for the lights - numDepthMaps stores
	 * the number of depth textures that were bound in the process */
	void setLightUniforms(Shader* shader, unsigned int startIndex, unsigned int maxLightsInBatch, unsigned int &numDepthMaps);

	/* Method used to render the shadow map of a light */
	void renderShadowMap(Light* light);

	/* Method used to render all of the shadow maps as required */
	void renderShadowMaps();

	/* Method used to render the scene without the lights */
	void renderWithoutLights();

	/* Method used to render the scene with the available lights */
	void renderWithLights();
public:
	/* The number of lights in each batch */
	static const unsigned int NUM_LIGHTS_IN_BATCH = 6;

	/* The constructor */
	RenderScene3D();

	/* The destructor */
	virtual ~RenderScene3D();

	/* Method used to enable deferred rendering */
	void enableDeferred();

	/* The method used to render all of the objects */
	void render();

	/* Used to add an object to this scene */
	void add(GameObject3D* object);

	/* Used to add a light to this scene */
	inline void addLight(Light* light) { lights.push_back(light); }

	/* Getters and setters */
	inline void setAmbientLight(Colour ambientLight) { this->ambientLight = ambientLight; }
	inline void enableLighting() { lightingEnabled = true; }
	inline void disableLighting() { lightingEnabled = false; }
	inline Colour getAmbientLight() { return ambientLight; }
	inline bool isLightingEnabled() { return lightingEnabled; }
};

#endif /* CORE_RENDER_RENDERSCENE_H_ */
