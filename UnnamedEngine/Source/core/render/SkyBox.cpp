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

#include "../BaseEngine.h"
#include "Renderer.h"
#include "SkyBox.h"

/*****************************************************************************
 * The SkyBox class
 *****************************************************************************/

SkyBox::SkyBox(Cubemap* cubemap) {
	//Assign the cubemap
	this->cubemap = cubemap;
	//Create the skybox
	Mesh* mesh = new Mesh(MeshBuilder::createCube(1.0f, 1.0f, 1.0f));
	mesh->getMaterial()->setDiffuse(cubemap);
	box = new GameObject3D(mesh, Renderer::getRenderShader(Renderer::SHADER_SKY_BOX));
}

void SkyBox::update(Vector3f cameraPosition) {
	//Set the position of the box and update its model matrix
	box->setPosition(cameraPosition);
	box->update();
}

void SkyBox::render() {
	Shader* shader = NULL;
	if (! BaseEngine::usingVulkan()) {
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(false); //Should be applied by GraphicsState
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		//To work the skybox must be drawn before anything else
		shader = box->getShader();
		shader->use();
	}

	Renderer::getShaderBlock_Core().ue_viewMatrix = Renderer::getCamera()->getViewMatrix();
	Renderer::getShaderBlock_Core().ue_projectionMatrix =  Renderer::getCamera()->getProjectionMatrix();

	box->render();

	if (! BaseEngine::usingVulkan())
		shader->stopUsing();
}

void SkyBox::destroy() {
	//Destroy created resources
	delete box;
}
