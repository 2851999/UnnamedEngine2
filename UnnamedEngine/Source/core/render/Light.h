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

#ifndef CORE_RENDER_LIGHT_H_
#define CORE_RENDER_LIGHT_H_

#include "FBO.h"
#include "../Object.h"

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
	float constant = 0;
	float linear = 0;
	float quadratic = 0;

	/* The cutoff and outer cutoff values for spot lights */
	float cutoff = 0;
	float outerCutoff = 0;

	/* The FBO if assigned for shadow mapping */
	FBO* depthBuffer = NULL;

	/* The light projection and view matrices */
	Matrix4f lightProjection;
	Matrix4f lightView;
public:
	/* Various light types */
	static const unsigned int TYPE_DIRECTIONAL = 1;
	static const unsigned int TYPE_POINT       = 2;
	static const unsigned int TYPE_SPOT        = 3;

	/* The constructor */
	Light(unsigned int type, Vector3f position = Vector3f(), bool castShadows = false) : type(type) {
		setPosition(position);

		if (type == TYPE_DIRECTIONAL) {
			if (castShadows) {
				depthBuffer = new FBO(GL_FRAMEBUFFER);

				depthBuffer->attach(new FramebufferTexture(
						GL_TEXTURE_2D,
						GL_DEPTH_COMPONENT,
						1024,
						1024,
						GL_DEPTH_COMPONENT,
						GL_FLOAT,
						GL_DEPTH_ATTACHMENT
				));

				depthBuffer->setup();

				lightProjection.initOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 8.0f);
			}
		}
	}

	/* The destructor */
	virtual ~Light() { delete depthBuffer; }

	/* The method used to update the view matrix for this light - should be
	 * called when the light moves/after assigning the initial values */
	void update();

	/* The method called to assign the uniforms in a shader for this light */
	virtual void setUniforms(Shader* shader, std::string suffix);

	/* Setters and getters */
	inline Light* setType(unsigned int type) { this->type = type; return this; }
	inline Light* setDirection(Vector3f direction) { this->direction = direction; return this; }
	inline Light* setDirection(float x, float y, float z) { direction = Vector3f(x, y, z); return this; }
	inline Light* setDiffuseColour(Colour colour) { diffuseColour = colour; return this; }
	inline Light* setSpecularColour(Colour colour) { specularColour = colour; return this; }
	inline Light* setConstantAttenuation(float value) { constant = value; return this; }
	inline Light* setLinearAttenuation(float value) { linear = value; return this; }
	inline Light* setQuadraticAttenuation(float value) { quadratic = value; return this; }
	inline Light* setCutoff(float cutoff) { this->cutoff = cutoff; return this; }
	inline Light* setOuterCutoff(float outerCutoff) { this->outerCutoff = outerCutoff; return this; }

	inline int getType() { return type; }
	inline Vector3f getDirection() { return direction; }
	inline Colour getDiffuseColour() { return diffuseColour; }
	inline Colour getSpecularColour() { return specularColour; }
	inline float getConstantAttenuation() { return constant; }
	inline float getLinearAttenuation() { return linear; }
	inline float getQuadraticAttenuation() { return quadratic; }
	inline float getCutoff() { return cutoff; }
	inline float getOuterCutoff() { return outerCutoff; }

	inline FBO* getDepthBuffer() { return depthBuffer; }
	inline bool hasDepthBuffer() { return depthBuffer; }
	inline Matrix4f getLightSpaceMatrix() { return lightProjection * lightView; }
};

#endif /* CORE_RENDER_LIGHT_H_ */
