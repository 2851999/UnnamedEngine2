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

#include "RenderScene.h"

#include "Renderer.h"
#include "../Window.h"
#include "../../utils/Utils.h"

/*****************************************************************************
 * The RenderScene3D class
 *****************************************************************************/

RenderScene3D::RenderScene3D() {
	//Get the required shaders
	shadowMapShader = Renderer::getRenderShader(Renderer::SHADER_SHADOW_MAP)->getShader();
}

RenderScene3D::~RenderScene3D() {
	for (unsigned int i = 0; i < batches.size(); i++) {
		for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
			delete batches[i].objects[j];
		}
	}
	batches.clear();
}

void RenderScene3D::add(GameObject3D* object) {
	Shader* objectShader = object->getShader();

	//Try and add the object to a batch with the same shader
	for (unsigned int i = 0; i < batches.size(); i++) {
		if (batches[i].shader == objectShader) {
			batches[i].objects.push_back(object);
			return;
		}
	}

	RenderBatch newBatch;
	newBatch.shader = objectShader;
	newBatch.objects.push_back(object);
	batches.push_back(newBatch);
}

void RenderScene3D::render() {
	//Check for lighting
	if (lights.size() > 0 && lightingEnabled) {
		//Go through all the lights and render the shadow map for it if necessary
		for (unsigned int i = 0; i < lights.size(); i++) {
			if (lights[i]->hasDepthBuffer())
				renderShadowMap(lights[i]);
		}

		glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);

		renderWithLights();

//		Renderer::render(lights[0]->getDepthBuffer()->getFramebufferTexture(0), NULL);
	} else {
		for (unsigned int i = 0; i < batches.size(); i++) {
			Shader* shader = batches[i].shader;

			shader->use();
			shader->setUniformi("NumLights", 0);

			for (unsigned int j = 0; j < batches[i].objects.size(); j++)
				batches[i].objects[j]->render();
		}
	}
}

void RenderScene3D::renderWithLights() {
	bool useEnvironmentMap = false;

	bool blendNeeded = false;

	//Uniform values that need to be assigned in the object specific shader before rendering
	int uniformNumLights = 0;
	Colour uniformLightAmbient;

	//Go through each batch of lights
	for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {
		uniformNumLights = utils_maths::min(NUM_LIGHTS_IN_BATCH, lights.size() - b);

		//Don't use blending until 2nd batch
		if (b == 0)
			uniformLightAmbient = ambientLight;
		else if (b == NUM_LIGHTS_IN_BATCH) {
			blendNeeded = true;

			//Blend the other batches on top, and don't use the ambient light again
			uniformLightAmbient = Colour(0.0f, 0.0f, 0.0f);

			//Setup blending
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
			glDepthMask(false);
			glDepthFunc(GL_LEQUAL);
		}

		//Go through the batches
		for (unsigned int i = 0; i < batches.size(); i++) {
			//Get the shader for the current batch
			Shader* shader = batches[i].shader;

			shader->use();

			shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

			if (useEnvironmentMap)
				shader->setUniformi("EnvironmentMap", Renderer::bindTexture(((Camera3D*) Renderer::getCamera())->getSkyBox()->getCubemap()));
			shader->setUniformi("UseEnvironmentMap", useEnvironmentMap);

			shader->setUniformi("NumLights", uniformNumLights);
			shader->setUniformColourRGB("LightAmbient", uniformLightAmbient);

			unsigned int numDepthMaps = 0;
			unsigned int lightNumInBatch = 0;

			//Go through the lights in this batch
			for (unsigned int l = b; (l < b + NUM_LIGHTS_IN_BATCH) && (l < lights.size()); l++) {
				lightNumInBatch = l - b;

				lights[l]->setUniforms(shader, "[" + utils_string::str(lightNumInBatch) + "]");

				if (lights[l]->hasDepthBuffer()) {
					shader->setUniformMatrix4("LightSpaceMatrix[" + utils_string::str(lightNumInBatch) + "]", lights[l]->getLightSpaceMatrix());
					shader->setUniformi("Light_ShadowMap[" + utils_string::str(lightNumInBatch) + "]", Renderer::bindTexture(lights[l]->getDepthBuffer()->getFramebufferTexture(0)));
					shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightNumInBatch) + "]", 1);

					numDepthMaps++;
				} else
					shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightNumInBatch) + "]", 0);
			}

			//Go through the objects in the batch
			for (unsigned int o = 0; o < batches[i].objects.size(); o++) {
				Matrix4f modelMatrix = batches[i].objects[o]->getModelMatrix();


				shader->setUniformMatrix4("ModelMatrix", modelMatrix);
				shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

				batches[i].objects[o]->render();
			}


			if (useEnvironmentMap)
				Renderer::unbindTexture();

			for (unsigned int i = 0; i < numDepthMaps; i++)
				Renderer::unbindTexture();

			shader->stopUsing();
		}
	}


	if (blendNeeded) {
		//Disable blending
		glDepthFunc(GL_LESS);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}

void RenderScene3D::renderShadowMap(Light* light) {
	FBO* depthBuffer = light->getDepthBuffer();

	depthBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1024, 1024);

	shadowMapShader->use();

	Matrix4f lightSpaceMatrix = light->getLightSpaceMatrix();

//	Renderer::getCamera()->setViewMatrix(lights[i]->getLightViewMatrix());
//	Renderer::getCamera()->setProjectionMatrix(lights[i]->getLightProjectionMatrix());

	for (unsigned int i = 0; i < batches.size(); i++) {
		for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
			if (((Camera3D*) Renderer::getCamera())->getFrustum().sphereInFrustum(Vector3f(batches[i].objects[j]->getModelMatrix() * Vector4f(batches[i].objects[j]->getMesh()->getBoundingSphereCentre(), 1.0f)), batches[i].objects[j]->getMesh()->getBoundingSphereRadius())) {
				shadowMapShader->setUniformMatrix4("LightSpaceMatrix", lightSpaceMatrix * batches[i].objects[j]->getModelMatrix());
				batches[i].objects[j]->getRenderShader()->addForwardShader(shadowMapShader);

				batches[i].objects[j]->render();

				batches[i].objects[j]->getRenderShader()->removeForwardShader(shadowMapShader);
			}
		}
	}

	shadowMapShader->stopUsing();
	depthBuffer->unbind();
}
