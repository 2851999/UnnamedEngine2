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
	if (deferredRendering)
		delete gBuffer;
}

/* Method used to enable deferred rendering */
void RenderScene3D::enableDeferred() {
	deferredRendering = true;

	//Create the geometry buffer
	if (! gBuffer)
		gBuffer = new GeometryBuffer();
}

void RenderScene3D::add(GameObject3D* object) {
	RenderShader* objectShader = object->getRenderShader();

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
	//Check for deferred rendering
	if (deferredRendering) {

		//Render the shadow maps as required
		renderShadowMaps();

		gBuffer->bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		geometryPass = true;
		renderWithoutLights();
		geometryPass = false;

		gBuffer->unbind();

		renderWithLights();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->getHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		unsigned int windowWidth = Window::getCurrentInstance()->getSettings().windowWidth;
		unsigned int windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Renderer::render(gBuffer->getFramebufferTexture(2));
	} else {
		//Check for lighting
		if (lights.size() > 0 && lightingEnabled) {
			//Render the shadow maps as required
			renderShadowMaps();

			renderWithLights();

			//Renderer::render(lights[0]->getDepthBuffer()->getFramebufferTexture(0), NULL);
		} else {
			renderWithoutLights();
		}
	}
}

void RenderScene3D::renderWithLights(unsigned int uniformNumLights, Colour uniformLightAmbient, unsigned int lightStartIndex) {
	if (deferredRendering) {
		//Get the shader for the current batch
		Shader* shader = Renderer::getRenderShader(Renderer::SHADER_DEFERRED_LIGHTING)->getShader();

		shader->use();

		shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

		shader->setUniformi("NumLights", uniformNumLights);
		shader->setUniformColourRGB("LightAmbient", uniformLightAmbient);

		unsigned int numDepthMaps = 0;

		//Set the light uniforms
		setLightUniforms(shader, lightStartIndex, NUM_LIGHTS_IN_BATCH, numDepthMaps);

		//Bind the buffers
		shader->setUniformi("PositionBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(0)));
		shader->setUniformi("NormalBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(1)));
		shader->setUniformi("AlbedoBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(2)));

		//Render
		Renderer::getScreenTextureMesh()->render();

		Renderer::unbindTexture();
		Renderer::unbindTexture();
		Renderer::unbindTexture();

		for (unsigned int i = 0; i < numDepthMaps; i++)
			Renderer::unbindTexture();

		shader->stopUsing();
	} else {
		//Go through the batches
		for (unsigned int i = 0; i < batches.size(); i++) {
			//Get the shader for the current batch
			Shader* shader = batches[i].shader->getShader();

			shader->use();

			if (! pbr) {
				if (useEnvironmentMap)
					shader->setUniformi("EnvironmentMap", Renderer::bindTexture(((Camera3D*) Renderer::getCamera())->getSkyBox()->getCubemap()));
				shader->setUniformi("UseEnvironmentMap", useEnvironmentMap);
			} else {
				//Check if the PBREnvironment has been assigned and assign the textures for it
				if (pbrEnvironment) {
					shader->setUniformi("IrradianceMap", Renderer::bindTexture(pbrEnvironment->getIrradianceCubemap()));
					shader->setUniformi("PrefilterMap", Renderer::bindTexture(pbrEnvironment->getPrefilterCubemap()));
					shader->setUniformi("BRDFLUT", Renderer::bindTexture(pbrEnvironment->getBRDFLUTTexture()));
				}
			}

			setLightingUniforms(shader, uniformNumLights, uniformLightAmbient);

			unsigned int numDepthMaps = 0;

			//Set the light uniforms
			setLightUniforms(shader, lightStartIndex, NUM_LIGHTS_IN_BATCH, numDepthMaps);

			//Go through the objects in the batch
			for (unsigned int o = 0; o < batches[i].objects.size(); o++) {
				Matrix4f modelMatrix = batches[i].objects[o]->getModelMatrix();

				shader->setUniformMatrix4("ModelMatrix", modelMatrix);
				shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

				batches[i].objects[o]->render();
			}

			if (! pbr) {
				if (useEnvironmentMap)
					Renderer::unbindTexture();
			} else {
				if (pbrEnvironment) {
					Renderer::unbindTexture();
					Renderer::unbindTexture();
					Renderer::unbindTexture();
				}
			}

			for (unsigned int i = 0; i < numDepthMaps; i++)
				Renderer::unbindTexture();

			shader->stopUsing();
		}
	}
}

void RenderScene3D::renderWithoutLights() {
	for (unsigned int i = 0; i < batches.size(); i++) {
		if (geometryPass)
			batches[i].shader->useGeometryShader(true);

		Shader* shader = batches[i].shader->getShader();;
		shader->use();

		if (geometryPass) {
			shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

			if (useEnvironmentMap)
				shader->setUniformi("EnvironmentMap", Renderer::bindTexture(((Camera3D*) Renderer::getCamera())->getSkyBox()->getCubemap()));
			shader->setUniformi("UseEnvironmentMap", useEnvironmentMap);
		}

		shader->setUniformi("NumLights", 0);

		for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
			if (geometryPass) {
				Matrix4f modelMatrix = batches[i].objects[j]->getModelMatrix();
				shader->setUniformMatrix4("ModelMatrix", modelMatrix);
				shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());
			}
			batches[i].objects[j]->render();
		}

		if (geometryPass && useEnvironmentMap) {
			Renderer::unbindTexture();
		}

		if (geometryPass)
			batches[i].shader->useGeometryShader(false);
	}
}

void RenderScene3D::renderWithLights() {
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

		renderWithLights(uniformNumLights, uniformLightAmbient, b);
	}


	if (blendNeeded) {
		//Disable blending
		glDepthFunc(GL_LESS);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}

void RenderScene3D::setLightingUniforms(Shader* shader, unsigned int uniformNumLights, Colour uniformLightAmbient) {
	shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());
	shader->setUniformi("NumLights", uniformNumLights);
	shader->setUniformColourRGB("LightAmbient", uniformLightAmbient);
}

void RenderScene3D::setLightUniforms(Shader* shader, unsigned int startIndex, unsigned int maxLightsInBatch, unsigned int &numDepthMaps) {
	unsigned int lightNumInBatch = 0;

	//Go through the lights in this batch
	for (unsigned int l = startIndex; (l < startIndex + maxLightsInBatch) && (l < lights.size()); l++) {
		lightNumInBatch = l - startIndex;

		lights[l]->setUniforms(shader, "[" + utils_string::str(lightNumInBatch) + "]");

		if (lights[l]->hasDepthBuffer()) {
			shader->setUniformMatrix4("LightSpaceMatrix[" + utils_string::str(lightNumInBatch) + "]", lights[l]->getLightSpaceMatrix());
			shader->setUniformi("Light_ShadowMap[" + utils_string::str(lightNumInBatch) + "]", Renderer::bindTexture(lights[l]->getDepthBuffer()->getFramebufferTexture(0)));
			shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightNumInBatch) + "]", 1);

			numDepthMaps++;
		} else
			shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightNumInBatch) + "]", 0);
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

void RenderScene3D::renderShadowMaps() {
	//Go through all the lights and render the shadow map for it if necessary
	for (unsigned int i = 0; i < lights.size(); i++) {
		if (lights[i]->hasDepthBuffer())
			renderShadowMap(lights[i]);
	}

	//Reset the view port to the default one
	glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);
}
