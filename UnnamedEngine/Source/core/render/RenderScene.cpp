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
#include "../../utils/GLUtils.h"

/*****************************************************************************
 * The RenderScene3D class
 *****************************************************************************/

RenderScene3D::RenderScene3D() {
	//Get the required shaders
	shadowMapShader = Renderer::getRenderShader(Renderer::SHADER_SHADOW_MAP)->getForwardShader();
	shadowCubemapShader = Renderer::getRenderShader(Renderer::SHADER_SHADOW_CUBEMAP)->getForwardShader();
	//Setup the post processor
	postProcessor = new PostProcessor("resources/shaders/postprocessing/GammaCorrectionShader", false);
	//Set the default post processing options
	enableGammaCorrection();
	setExposure(-1);

	//Setup the intermediate FBO if it is needed
	if (Window::getCurrentInstance()->getSettings().videoSamples > 0)
		intermediateFBO = new PostProcessor(true);

	//Get the lighting UBO
	shaderLightingUBO = Renderer::getShaderInterface()->getUBO(ShaderInterface::BLOCK_LIGHTING);
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
	if (intermediateFBO != NULL)
		delete intermediateFBO;
	delete postProcessor;
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

			//Render a wireframe instead if requested
			if (renderWireframe)
				utils_gl::enableWireframe();

			//Go through all of the objects in this scene
			for (unsigned int i = 0; i < batches.size(); i++) {
				//Ensure the geometry shader is used to render all of the objects in this batch
				batches[i].shader->useGeometryShader(true);

				//Get the current shader
				Shader* shader = batches[i].shader->getShader();
				//Use the shader
				shader->use();

				shaderLightingData.ue_numLights = 0;
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

			if (renderWireframe)
				utils_gl::disableWireframe();

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

			//Copy depth data to the default framebuffer so forward rendering is still possible after this scene was rendered
			gBuffer->outputDepthInfo();
		} else {
			//Forward rendering

			//Prepare for forward rendering
			forwardPreRender();

			//Go through all of the objects in this scene
			for (unsigned int i = 0; i < batches.size(); i++) {
				//Render the current batch
				renderLighting(batches[i].shader, i);
			}

			//Finish forward rendering
			forwardPostRender();
		}
	} else {
		//Don't bother with deferred rendering if lighting is disabled

		//Prepare for forward rendering
		forwardPreRender();

		//Go through and render all of the objects in this scene
		for (unsigned int i = 0; i < batches.size(); i++) {
			for (unsigned int j = 0; j < batches[i].objects.size(); j++)
				batches[i].objects[j]->render();
		}

		//Finish forward rendering
		forwardPostRender();
	}
}

void RenderScene3D::forwardPreRender() {
	if (intermediateFBO)
		//Render to the intermediate FBO
		intermediateFBO->start();
	else
		postProcessor->start();

	//Render a wireframe instead if requested
	if (renderWireframe)
		utils_gl::enableWireframe();
}

void RenderScene3D::forwardPostRender() {
	if (renderWireframe)
		utils_gl::disableWireframe();

	if (intermediateFBO) {
		//Stop rendering to the intermediate FBO
		intermediateFBO->stop();
		//Copy the colour data to the postprocessor
		intermediateFBO->copyToFramebuffer(postProcessor->getFBO(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	} else
		postProcessor->stop();

	//Render the output
	postProcessor->render();

	//Copy the depth data to the framebuffer
	postProcessor->copyToScreen(GL_DEPTH_BUFFER_BIT);
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
		shaderLightingData.ue_lightAmbient = Vector4f(ambientLight, 0.0f);
	}

	//Also check for deferred rendering
	if (deferredRendering) {
		//Bind the geometry buffer textures
		shader->setUniformi("PositionBuffer", Renderer::bindTexture(gBuffer->getPositionBuffer()));
		shader->setUniformi("NormalBuffer", Renderer::bindTexture(gBuffer->getNormalBuffer()));
		shader->setUniformi("AlbedoBuffer", Renderer::bindTexture(gBuffer->getAlbedoBuffer()));

		//Check if the metalness is also needed (for PBR)
		if (pbr)
			shader->setUniformi("MetalnessAOBuffer", Renderer::bindTexture(gBuffer->getMetalnessAOBuffer()));
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
				shaderLightingData.ue_lightAmbient = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

			//Setup blending
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
			glDepthMask(false);
			glDepthFunc(GL_LEQUAL);
		}

		//Assign the number of lights that will be rendered in this set
		shaderLightingData.ue_numLights = uniformNumLights;

		//The light number in the current set
		unsigned int lightIndexInSet;

		//Go through each light in the set
		for (unsigned int l = s; (l < s + NUM_LIGHTS_IN_SET) && (l < lights.size()); l++) {
			//Calculate the index of the current light as it will appear in the shader
			lightIndexInSet = l - s;
			//Assign the uniforms for the current light
			lights[l]->setUniforms(shaderLightingData.ue_lights[lightIndexInSet]);

			//Assign the data for shadow mapping
			if (lights[l]->hasDepthBuffer()) {
				shaderLightingData.ue_lightSpaceMatrix[lightIndexInSet] = lights[l]->getLightSpaceMatrix();
				if (lights[l]->getType() == Light::TYPE_POINT)
					shader->setUniformi("Light_ShadowCubemap[" + utils_string::str(lightIndexInSet) + "]", Renderer::bindTexture(lights[l]->getDepthBuffer()->getFramebufferStore(0))); //((Camera3D*)Renderer::getCamera())->getSkyBox()->getCubemap())
				else
					shader->setUniformi("Light_ShadowMap[" + utils_string::str(lightIndexInSet) + "]", Renderer::bindTexture(lights[l]->getDepthBuffer()->getFramebufferStore(0)));
				shaderLightingData.ue_lights[lightIndexInSet].useShadowMap = 1;
			} else
				shaderLightingData.ue_lights[lightIndexInSet].useShadowMap = 0;
		}

		shaderLightingUBO->update(&shaderLightingData, 0, sizeof(ShaderBlock_Lighting));

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

	//Setup the required values for rendering to the depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Assign the view port to match the shadow maps size
	glViewport(0, 0, light->getShadowMapSize(), light->getShadowMapSize());

	if (light->getType() == Light::TYPE_POINT) {
		shadowCubemapShader->use();

		shadowCubemapShader->setUniformVector3("LightPosition", light->getPosition());
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[0]", light->getLightShadowTransform(0));
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[1]", light->getLightShadowTransform(1));
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[2]", light->getLightShadowTransform(2));
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[3]", light->getLightShadowTransform(3));
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[4]", light->getLightShadowTransform(4));
		shadowCubemapShader->setUniformMatrix4("ShadowMatrices[5]", light->getLightShadowTransform(5));
	} else
		shadowMapShader->use();

	//Required for rendering the light, however needs to be post multiplied by objects model matrix
	Matrix4f lightSpaceMatrix = light->getLightSpaceMatrix();

	//Go through all of the objects in this scene
	for (unsigned int i = 0; i < batches.size(); i++) {
		//Go through all of the objects in the current batch
		for (unsigned int j = 0; j < batches[i].objects.size(); j++) {
			//Pointer to the current object
			GameObject3D* object = batches[i].objects[j];
			//Stores the previous value of whether culling was enabled on the objects mesh
			bool culling = object->getMesh()->isCullingEnabled();
			//If culling is enabled ensure the object is visible to the light
			if (! culling || ! object->shouldCull(light->getFrustum()) || light->getType() == Light::TYPE_POINT) {
				//Assign the required uniforms that could not have been done outside of the loop
				if (light->getType() == Light::TYPE_POINT) {
					shadowCubemapShader->setUniformMatrix4("ModelMatrix", object->getModelMatrix());
					//Ensure the object uses the shadow map shader to render
					object->getRenderShader()->addForwardShader(shadowCubemapShader);
				} else {
					shadowMapShader->setUniformMatrix4("LightSpaceMatrix", lightSpaceMatrix * object->getModelMatrix());
					//Ensure the object uses the shadow map shader to render
					object->getRenderShader()->addForwardShader(shadowMapShader);
				}

				//Ensure the object isn't culled just because it can't be seen by the camera
				object->getMesh()->setCullingEnabled(false);

				//Render the object with the shadow map shader
				object->render();

				if (light->getType() == Light::TYPE_POINT)
					object->getRenderShader()->removeForwardShader(shadowCubemapShader);
				else
					object->getRenderShader()->removeForwardShader(shadowMapShader);

				//Restore the original value
				object->getMesh()->setCullingEnabled(culling);
			}
		}
	}

	if (light->getType() == Light::TYPE_POINT)
		shadowCubemapShader->stopUsing();
	else
		shadowMapShader->stopUsing();

	//Stop drawing to the depth buffer
	depthBuffer->unbind();
}

void RenderScene3D::showDeferredBuffers() {
	if (deferredRendering) {
		//Copy the various buffers onto the default framebuffer
		int x;
		int y;
		int windowWidth  = Window::getCurrentInstance()->getSettings().windowWidth;
		int windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;
		int width = windowWidth / 4.0f;
		int height = windowHeight / 4.0f;

		unsigned int numBuffers = pbr ? 4 : 3;

		x = windowWidth - width;
		y = windowHeight - height;

		for (unsigned int i = 0; i < numBuffers; i++) {
			gBuffer->getFBO()->copyToScreen(i, x, y, width, height);

			y -= height;
		}
	}
}

void RenderScene3D::enableGammaCorrection() {
	Shader* shader = postProcessor->getShader();
	shader->use();
	shader->setUniformi("GammaCorrect", 1);
	shader->stopUsing();
}

void RenderScene3D::disableGammaCorrection() {
	Shader* shader = postProcessor->getShader();
	shader->use();
	shader->setUniformi("GammaCorrect", 0);
	shader->stopUsing();
}

void RenderScene3D::setExposure(float exposure) {
	Shader* shader = postProcessor->getShader();
	shader->use();
	shader->setUniformf("Exposure", exposure);
	shader->stopUsing();
}
