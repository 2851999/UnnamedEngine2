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

#ifndef TESTS_BASEENGINETEST3D_H_
#define TESTS_BASEENGINETEST3D_H_

#include "../core/BaseEngine.h"

#include "../core/render/MeshLoader.h"
#include "../utils/Utils.h"

class Test : public BaseEngine {
private:
	float lastTime = 0;

	Camera3D* camera;
	GameObject3D* quad;
	Texture* texture;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;
};

void Test::initialise() {
	getSettings().videoVulkan = true; //Validation layers have quite large effect on performance
	getSettings().videoMaxFPS = 0;
	getSettings().debugShowInformation = false;
}

void Test::created() {
	Shader::compileEngineShaderToSPIRV("VulkanShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");

	camera = new Camera3D(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->setPosition(0.0f, 0.0f, 1.0f);
	camera->update();
	Renderer::addCamera(camera);

	texture = Texture::loadTexture("resources/textures/texture.jpg");
	Mesh* mesh = new Mesh(MeshBuilder::createQuad3D(Vector2f(-0.5f, 0.5f), Vector2f(0.5f, 0.5f), Vector2f(0.5f, -0.5f), Vector2f(-0.5f, -0.5f), texture));
//	mesh->getMaterial()->setDiffuse(texture);
//	mesh->getMaterial()->setDiffuse(Colour(1.0f, 0.0f, 0.0f, 1.0f));
//	Mesh* mesh = MeshLoader::loadModel("C:/UnnamedEngine/models/SimpleSphere/", "SimpleSphere.obj", false);
	mesh->setCullingEnabled(false);
	quad = new GameObject3D(mesh, Renderer::SHADER_VULKAN);
	quad->update();
}


void Test::update() {
	if (utils_time::getSeconds() - lastTime > 0.5f) {
		lastTime = utils_time::getSeconds();
		std::cout << getFPS() << std::endl;
	}
}

void Test::render() {
	if (! getSettings().videoVulkan)
		glClear(GL_COLOR_BUFFER_BIT);
	quad->render();
}

void Test::destroy() {
	delete quad;
	delete camera;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
