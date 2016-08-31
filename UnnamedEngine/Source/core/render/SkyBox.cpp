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

#include "SkyBox.h"
#include "Renderer.h"

/*****************************************************************************
 * The SkyBox class
 *****************************************************************************/

SkyBox::SkyBox(std::string path, std::string front, std::string back, std::string left, std::string right, std::string top, std::string bottom, float size) {
	box = new GameObject3D(new Mesh(MeshBuilder::createCube(size, size, size)), Renderer::getRenderShader("SkyBox"));
	cubemap = new Cubemap(path, { right, left, top, bottom, back, front });
	box->getMaterial()->setDiffuseTexture(cubemap);
}

void SkyBox::update(Vector3f cameraPosition) {
	box->setPosition(cameraPosition);
	box->update();
}

void SkyBox::render() {
	glDepthMask(false);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	box->render();
	glDepthMask(true);
}

void SkyBox::destroy() {
	delete box;
}
