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

#include "Renderer.h"
#include "../Object.h"
#include "../Frustum.h"

/*****************************************************************************
 * The Light class contains the information required to create a light
 *****************************************************************************/

class Light : public GameObject3D {
private:
	/* This light's type */
	unsigned int type;

	/* The direction of the light - for directional and spot */
	Vector3f direction;

	/* The diffuse and specular colours */
	Colour diffuseColour = Colour::WHITE;
	Colour specularColour = Colour::WHITE;

	/* The constant, linear and quadratic terms for point lights */
	float constant  = 0.0f;
	float linear    = 0.0f;
	float quadratic = 1.0f;

	/* The inner and outer cutoff values for spot lights (taken as cos of the angle) */
	float innerCutoff = 0;
	float outerCutoff = 0;

	/* The size of the shadow map (width and height) */
	unsigned int shadowMapSize = 1024;

	/* The light projection and view matrices */
	Matrix4f lightProjection;
	Matrix4f lightView;

	/* Shadow transforms for point lights */
	std::vector<Matrix4f> lightShadowTransforms;

	/* Frustum used for frustum culling when rendering shadow maps */
	Frustum frustum;

	/* Combination of the above matrices, assigned in the update method */
	Matrix4f lightProjectionView;
public:
	/* Various light types */
	static const unsigned int TYPE_DIRECTIONAL = 1;
	static const unsigned int TYPE_POINT       = 2;
	static const unsigned int TYPE_SPOT        = 3;

	/* The constructor */
	Light(unsigned int type, Vector3f position = Vector3f(), bool castShadows = false);

	/* The destructor */
	virtual ~Light() {}

	/* The method used to update the view matrix for this light - should be
	 * called when the light moves/after assigning the initial values */
	void update();

	/* The method called to assign the uniforms in a shader for this light */
	virtual void setUniforms(ShaderStruct_Light& lightData);

	/* Setters and getters */
	inline Light* setType(unsigned int type) { this->type = type; return this; }
	inline Light* setDirection(Vector3f direction) { this->direction = direction; return this; }
	inline Light* setDirection(float x, float y, float z) { direction = Vector3f(x, y, z); return this; }
	inline Light* setDiffuseColour(Colour colour) { diffuseColour = colour; return this; }
	inline Light* setSpecularColour(Colour colour) { specularColour = colour; return this; }
	inline Light* setConstantAttenuation(float value) { constant = value; return this; }
	inline Light* setLinearAttenuation(float value) { linear = value; return this; }
	inline Light* setQuadraticAttenuation(float value) { quadratic = value; return this; }
	inline Light* setInnerCutoff(float innerCutoff) { this->innerCutoff = innerCutoff; return this; }
	inline Light* setOuterCutoff(float outerCutoff) { this->outerCutoff = outerCutoff; return this; }
	inline Light* setInnerCutoffDegrees(float innerCutoff) { this->innerCutoff = cos(utils_maths::toRadians(innerCutoff)); return this; }
	inline Light* setOuterCutoffDegrees(float outerCutoff) { this->outerCutoff = cos(utils_maths::toRadians(outerCutoff)); return this; }

	inline int getType() { return type; }
	inline Vector3f getDirection() { return direction; }
	inline Colour getDiffuseColour() { return diffuseColour; }
	inline Colour getSpecularColour() { return specularColour; }
	inline float getConstantAttenuation() { return constant; }
	inline float getLinearAttenuation() { return linear; }
	inline float getQuadraticAttenuation() { return quadratic; }
	inline float getInnerCutoff() { return innerCutoff; }
	inline float getOuterCutoff() { return outerCutoff; }

	inline unsigned int getShadowMapSize() { return shadowMapSize; }
	inline Matrix4f getLightProjectionMatrix() { return lightProjection; }
	inline Matrix4f& getLightShadowTransform(unsigned int index) { return lightShadowTransforms[index]; }
	inline Matrix4f getLightViewMatrix() { return lightView; }
	inline Matrix4f getLightSpaceMatrix() { return lightProjectionView; }
	inline Frustum& getFrustum() { return frustum; }
};

