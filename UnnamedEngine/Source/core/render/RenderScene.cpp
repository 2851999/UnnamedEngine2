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
	//Setup the post processor
	postProcessor = new PostProcessor("resources/shaders/postprocessing/GammaCorrectionShader");
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
		gBuffer = new GeometryBuffer(pbr);
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
	//Ensure lighting is enabled
	if (lighting) {
		//Render the scene to the shadow maps as necessary
		renderShadowMaps();
		//Check if deferred rendering or not
		if (deferredRendering) {
			//Deferred rendering

			//Bind the geometry buffer to render to it
			gBuffer->bind();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//Go through all of the objects in this scene
			for (unsigned int i = 0; i < batches.size(); i++) {
				//Ensure the geometry shader is used to render all of the objects in this batch
				batches[i].shader->useGeometryShader(true);

				//Get the current shader
				Shader* shader = batches[i].shader->getShader();
				//Use the shader
				shader->use();

				shader->setUniformi("NumLights", 0);
				shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

				//Go through all of the objects in the current batch
				for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
					//Pointer to the current object
					GameObject3D* object = batches[i].objects[j];

					//Get the model matrix for the current object
					Matrix4f modelMatrix = object->getModelMatrix();

					//Assign the required matrix uniforms for the object
					shader->setUniformMatrix4("ModelMatrix", modelMatrix);
					shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

					//Render the object with the geometry shader
					object->render();
				}

				//Stop using the geometry shader
				batches[i].shader->useGeometryShader(false);
			}

			//Stop rendering to the geometry buffer
			gBuffer->unbind();

			//Render to the post processor's framebuffer
			postProcessor->start();

			//Render to the screen with the correct lighting shader
			if (pbr)
				renderLighting(Renderer::getRenderShader(Renderer::SHADER_PBR_DEFERRED_LIGHTING), -1);
			else
				renderLighting(Renderer::getRenderShader(Renderer::SHADER_DEFERRED_LIGHTING), -1);

			//Stop using the post processor's framebuffer
			postProcessor->stop();

			//Render the final output
			postProcessor->render();

			//Copy depth data to the default framebuffer so forward rendessssssssssring is still possible after this scene was rendered
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->getHandle());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			unsigned int windowWidth = Window::getCurrentInstance()->getSettings().windowWidth;
			unsigned int windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;
			glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		} else {
			//Forward rendering

			//Render to the post processor's framebuffer
			postProcessor->start();

			//Go through all of the objects in this scene
			for (unsigned int i = 0; i < batches.size(); i++) {
				//Render the current batch
				renderLighting(batches[i].shader, i);
			}

			//Stop using the post processor's framebuffer
			postProcessor->stop();

			//Render the final output
			postProcessor->render();
		}
	}
}

void RenderScene3D::renderLighting(RenderShader* renderShader, int indexOfBatch) {
	Renderer::saveTextures();

	//Get the shader to use
	Shader* shader = renderShader->getShader();

	//Use the shader
	shader->use();

	//Assign any uniforms that are needed for all kinds of lighting
	shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

	//Check whether using PBR or phong lighting
	if (pbr) {
		//Ensure ambient lighting is added
		shader->setUniformi("UseAmbient", 1);
		//Check if the environment textures should be bound
		if (pbrEnvironment) {
			//Bind the textures
			shader->setUniformi("IrradianceMap", Renderer::bindTexture(pbrEnvironment->getIrradianceCubemap()));
			shader->setUniformi("PrefilterMap", Renderer::bindTexture(pbrEnvironment->getPrefilterCubemap()));
			shader->setUniformi("BRDFLUT", Renderer::bindTexture(pbrEnvironment->getBRDFLUTTexture()));
		}
	} else {
		//Assign the shader uniforms only needed for 'normal' phong shading that don't need information from the
		//model or current light
		shader->setUniformColourRGB("LightAmbient", ambientLight);
	}

	//Also check for deferred rendering
	if (deferredRendering) {
		//Bind the geometry buffer textures
		shader->setUniformi("PositionBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(0)));
		shader->setUniformi("NormalBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(1)));
		shader->setUniformi("AlbedoBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(2)));

		//Check if the metalness is also needed (for PBR)
		if (pbr)
			shader->setUniformi("MetalnessAOBuffer", Renderer::bindTexture(gBuffer->getFramebufferTexture(3)));
	}

	//States the number of lights in the current batch (assigned later)
	unsigned int uniformNumLights;

	//States whether blending was needed
	bool blendNeeded = false;

	//Go through each set of lights
	for (unsigned int s = 0; s < lights.size(); s += NUM_LIGHTS_IN_SET) {
		//Calculate the number of lights in this set
		uniformNumLights = utils_maths::min(NUM_LIGHTS_IN_SET, lights.size() - s);

		//Check if blending needs to be setup
		if (s == NUM_LIGHTS_IN_SET) {
			//In the second set of lights, hence need to blend on top of the previous set
			blendNeeded = true;

			//Assign the ambient light parameter if using phong shading (don't want to include it twice)
			if (pbr)
				shader->setUniformi("UseAmbient", 0);
			else
				shader->setUniformColourRGB("LightAmbient", Colour(0.0f, 0.0f, 0.0f));

			//Setup blending
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
			glDepthMask(false);
			glDepthFunc(GL_LEQUAL);
		}

		//Assign the number of lights that will be rendered in this set
		shader->setUniformi("NumLights", uniformNumLights);

		//The light number in the current set
		unsigned int lightIndexInSet;

		//Go through each light in the set
		for (unsigned int l = s; (l < s + NUM_LIGHTS_IN_SET) && (l < lights.size()); l++) {
			//Calculate the index of the current light as it will appear in the shader
			lightIndexInSet = l - s;
			//Assign the uniforms for the current light
			lights[l]->setUniforms(shader, "[" + utils_string::str(lightIndexInSet) + "]");

			//Assign the data for shadow mapping
			if (lights[l]->hasDepthBuffer()) {
				shader->setUniformMatrix4("LightSpaceMatrix[" + utils_string::str(lightIndexInSet) + "]", lights[l]->getLightSpaceMatrix());
				shader->setUniformi("Light_ShadowMap[" + utils_string::str(lightIndexInSet) + "]", Renderer::bindTexture(lights[l]->getDepthBuffer()->getFramebufferTexture(0)));
				shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightIndexInSet) + "]", 1);
			} else
				shader->setUniformi("Light_UseShadowMap[" + utils_string::str(lightIndexInSet) + "]", 0);
		}

		//Now check whether forward or deferred rendering
		if (deferredRendering)
			//Deferred rendering, so render the quad on the screen with the current set of lights
			Renderer::getScreenTextureMesh()->render();
		else {
			//Go through each object in the current batch and render it with the shader
			for (unsigned int j = 0; j < batches[indexOfBatch].objects.size(); j++) {
				//Get the model matrix for the current object
				Matrix4f modelMatrix = batches[indexOfBatch].objects[j]->getModelMatrix();

				//Assign the required matrix uniforms for the object
				shader->setUniformMatrix4("ModelMatrix", modelMatrix);
				shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

				//Render the object with the shadow map shader
				batches[indexOfBatch].objects[j]->render();
			}
		}
	}

	shader->stopUsing();

	//Stop using blending if it was needed
	if (blendNeeded) {
		glDepthFunc(GL_LESS);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}

	Renderer::releaseNewTextures();
}

void RenderScene3D::renderShadowMaps() {
	//Go through all of the lights and render the shadow map as required
	for (unsigned int i = 0; i < lights.size(); i++) {
		if (lights[i]->hasDepthBuffer())
			renderShadowMap(lights[i]);
	}

	//Reset the view port to the default one
	glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);
}

void RenderScene3D::renderShadowMap(Light* light) {
	//Get the depth buffer FBO to render to
	FBO* depthBuffer = light->getDepthBuffer();
	//Ensure the following objects are rendered to it
	depthBuffer->bind();

	//Setup the required values for rendering tothe depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Assign the view port to match the shadow maps size
	glViewport(0, 0, light->getShadowMapSize(), light->getShadowMapSize());

	shadowMapShader->use();

	//Required for rendering the light, however needs to be post multiplied by objects model matrix
	Matrix4f lightSpaceMatrix = light->getLightSpaceMatrix();

	//Go through all of the objects in this scene
	for (unsigned int i = 0; i < batches.size(); i++) {
		//Go through all of the objects in the current batch
		for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
			//Pointer to the current object
			GameObject3D* object = batches[i].objects[j];
			//Assign the required uniforms that could not have been done outside of the loop
			shadowMapShader->setUniformMatrix4("LightSpaceMatrix", lightSpaceMatrix * object->getModelMatrix());
			//Ensure the object uses the shadow map shader to render
			object->getRenderShader()->addForwardShader(shadowMapShader);

			//Render the object with the shadow map shader
			object->render();

			object->getRenderShader()->removeForwardShader(shadowMapShader);
		}
	}

	shadowMapShader->stopUsing();

	//Stop drawing to the depth buffer
	depthBuffer->unbind();
}
