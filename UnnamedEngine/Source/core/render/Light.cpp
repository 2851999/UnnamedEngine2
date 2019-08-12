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

#include "Light.h"

/*****************************************************************************
 * The Light class
 *****************************************************************************/

Light::Light(unsigned int type, Vector3f position, bool castShadows) : type(type) {
	setPosition(position);

	if (castShadows) {
		if (type == TYPE_DIRECTIONAL) {
			depthBuffer = new FBO(GL_FRAMEBUFFER);

			depthBuffer->attach(new FramebufferStore(
					GL_TEXTURE_2D,
					GL_DEPTH_COMPONENT24,
					shadowMapSize,
					shadowMapSize,
					GL_DEPTH_COMPONENT,
					GL_FLOAT,
					GL_DEPTH_ATTACHMENT,
					GL_LINEAR,
					GL_CLAMP_TO_BORDER, //Want to clamp to avoid repeating shadows when out of the bounds of the shadow map
					true
			));

			depthBuffer->setup();

			lightProjection.initOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
		} else if (type == TYPE_POINT) {
			depthBuffer = new FBO(GL_FRAMEBUFFER);

			//Setup the depth buffer cubemap
			depthBuffer->attach(new FramebufferStoreCubemap(
					GL_DEPTH_COMPONENT24,
					shadowMapSize,
					GL_DEPTH_COMPONENT,
					GL_FLOAT,
					GL_DEPTH_ATTACHMENT,
					GL_NEAREST,
					GL_CLAMP_TO_EDGE, //Want to clamp to avoid repeating shadows when out of the bounds of the shadow map
					true
			));

			depthBuffer->setup();

			//Setup the shadow transforms for each face of the cubemap
			for (unsigned int i = 0; i < 6; ++i)
				lightShadowTransforms.push_back(Matrix4f());

			//Setup the projection matrix for the shadow map rendering
			lightProjection.initPerspective(90.0f, (float) shadowMapSize / (float) shadowMapSize, 1.0f, 25.0f);
		}
		//Update this light (in case it is static)
		update();
	}
}

void Light::update() {
	//Check the type of light this is
	if (type == TYPE_DIRECTIONAL) {
		Vector3f right = direction.cross(Vector3f(0.0f, 1.0f, 0.0f)).normalise();
		Vector3f up = right.cross(direction).normalise();

		lightView.initLookAt(direction * -5, (direction * 5) + direction, up);

		lightProjectionView = lightProjection * lightView;

		//Update the frustum
		frustum.update(lightProjectionView);
	} else if (type == TYPE_POINT && hasDepthBuffer()) {
		//The position of the point light
		Vector3f pos = getPosition();
		//Assign each of the transform matrices
		lightShadowTransforms[0] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f));
		lightShadowTransforms[1] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f));
		lightShadowTransforms[2] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f,  0.0f,  1.0f));
		lightShadowTransforms[3] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f,  0.0f, -1.0f));
		lightShadowTransforms[4] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, -1.0f,  0.0f));
		lightShadowTransforms[5] = lightProjection * Matrix4f().initLookAt(pos, pos + Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, -1.0f,  0.0f));
	}
}

void Light::setUniforms(ShaderStruct_Light& lightData) {
	lightData.type = type;
	lightData.position = Vector4f(getPosition(), 0.0f);
	lightData.direction = Vector4f(direction, 0.0f);
	lightData.diffuseColour = Vector4f(diffuseColour, 0.0f);
	lightData.specularColour = Vector4f(specularColour, 0.0f);
	lightData.constant = constant;
	lightData.linear = linear;
	lightData.quadratic = quadratic;
	lightData.innerCutoff = innerCutoff;
	lightData.outerCutoff = outerCutoff;
}

