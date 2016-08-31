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

#include "Scene.h"

#include "Renderer.h"
#include "../Window.h"

#include "../../utils/Utils.h"

/*****************************************************************************
 * The Scene class
 *****************************************************************************/

Scene::~Scene() {
	for (unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();
}

void Scene::update() {
	for (unsigned int i = 0; i < objects.size(); i++)
		objects[i]->update();
}

void Scene::render() {
	//Check for lighting
	if (lights.size() > 0) {
		for (unsigned int i = 0; i < lights.size(); i++) {
			if (lights[i]->hasDepthBuffer()) {
				FBO* depthBuffer = lights[i]->getDepthBuffer();
				Shader* shadowMapShader = Renderer::getRenderShader("ShadowMap")->getShader();

				depthBuffer->bind();
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, 1024, 1024);

				shadowMapShader->use();

				for (unsigned int j = 0; j < objects.size(); j++) {
					shadowMapShader->setUniformMatrix4("LightSpaceMatrix", (lights[i]->getLightSpaceMatrix() * objects[j]->getModelMatrix()));

					objects[i]->render(true);
				}

				shadowMapShader->stopUsing();
				depthBuffer->unbind();
			}
		}

		glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);

		//Setup for lighting
		Shader* shader = Renderer::getRenderShader("Lighting")->getShader();
		//MAKE SO ONLY OCCURS WHEN > 6 LIGHTS
		/*
		 * To try and make forward lighting more efficient, the shader is able to
		 * render multiple lights with a for loop, but as this has a limit to the
		 * maximum possible light blending will be used to combine lighting
		 * 'batches' each with up to the maximum number of lights supported by
		 * the shader
		 */

		shader->use();
		shader->setUniformi("NumLights", 0);
		shader->setUniformColourRGB("Light_Ambient", Colour(0.1f, 0.1f, 0.1f));
		shader->setUniformVector3("Camera_Position", ((Camera3D*) Renderer::getCamera())->getPosition());

		shader->setUniformi("EnvironmentMap", Renderer::bindTexture(((Camera3D*) Renderer::getCamera())->getSkyBox()->getCubemap()));
		shader->setUniformi("UseEnvironmentMap", 0);
		for (unsigned int i = 0; i < objects.size(); i++) {
			//if (((Camera3D*) Renderer::getCamera())->getFrustum().testSphere(((GameObject3D*) objects[i])->getPosition(), 1.0f))
				objects[i]->render(true);
		}

		//Setup blending
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
		glDepthMask(false);
		glDepthFunc(GL_LEQUAL);

		shader->setUniformColourRGB("Light_Ambient", Colour(0.0f, 0.0f, 0.0f));

		//Go through each batch of lights
		for (unsigned int b = 0; b < lights.size(); b += 6) {
			shader->setUniformi("NumLights", MathsUtils::min(6u, lights.size() - b));

			//Go through the lights in this batch
			for (unsigned int l = b; (l < b + 6) && (l < lights.size()); l++)
				lights[l]->setUniforms(shader, "[" + StrUtils::str(l - b) + "]");

			if (lights[b]->hasDepthBuffer()) {
				shader->setUniformMatrix4("LightSpaceMatrix", lights[b]->getLightSpaceMatrix());
				shader->setUniformi("ShadowMap", Renderer::bindTexture(lights[b]->getDepthBuffer()->getFramebufferTexture(0)));
				shader->setUniformi("UseShadowMap", 1);
			} else
				shader->setUniformi("UseShadowMap", 0);

			//Go through the objects in the scene
			for (unsigned int o = 0; o < objects.size(); o++) {
				//if (((Camera3D*) Renderer::getCamera())->getFrustum().testSphere(((GameObject3D*) objects[o])->getPosition(), 1.0f)) {
					Matrix4f modelMatrix = objects[o]->getModelMatrix();

					shader->setUniformMatrix4("ModelMatrix", modelMatrix);
					shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

					objects[o]->render(true);
				//}
			}

			if (lights[b]->hasDepthBuffer())
				Renderer::unbindTexture();
		}

		Renderer::unbindTexture();

		shader->stopUsing();

		//Disable blending
		glDepthFunc(GL_LESS);
		glDepthMask(true);
		glDisable(GL_BLEND);

//		Renderer::render(lights[0]->getDepthBuffer()->getFramebufferTexture(0), NULL);
	} else {
		for (unsigned int i = 0; i < objects.size(); i++)
			objects[i]->render();
	}
}
