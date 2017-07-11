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

#include "Renderer.h"
#include "SkyBox.h"

/*****************************************************************************
 * The SkyBox class
 *****************************************************************************/

SkyBox::SkyBox(std::string path, std::string front, std::string back, std::string left, std::string right, std::string top, std::string bottom, float size) {
	//Create the skybox
	box = new GameObject3D({ new Mesh(MeshBuilder::createCube(size, size, size)) }, Renderer::getRenderShader("SkyBox"));
	//Load the texture
	cubemap = new Cubemap(path, { right, left, top, bottom, back, front });
	//Assign the texture in the skybox
	box->getMaterial()->diffuseTexture = cubemap;
}

void SkyBox::update(Vector3f cameraPosition) {
	//Set the position of the box and update its model matrix
	box->setPosition(cameraPosition);
	box->update();
}

void SkyBox::render() {
	//To work the skybox must be drawn before anything else
	glDepthMask(false);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	box->render();
	glDepthMask(true);
}

void SkyBox::destroy() {
	//Destroy created resources
	delete box;
}
