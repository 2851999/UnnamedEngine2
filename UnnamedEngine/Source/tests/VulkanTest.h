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
#include "../core/vulkan/VulkanRenderShader.h"

#include "../utils/Utils.h"

class Test : public BaseEngine {
private:
	float lastTime = 0;

	UBO* ubo;
	VulkanRenderShader* renderShader;
	MeshRenderData* quad;
	VulkanRenderShader::UBOData uboData;
	Texture* texture;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;
};

void Test::initialise() {
	getSettings().videoVulkan = true; //Validation layer have quite large effect on performance
	getSettings().videoMaxFPS = 0;
	getSettings().debugShowInformation = false;
}

void Test::created() {
	texture = Texture::loadTexture("resources/textures/texture.jpg");

	ubo = new UBO(NULL, sizeof(VulkanRenderShader::UBOData), GL_DYNAMIC_DRAW, 0);

	renderShader = new VulkanRenderShader(Shader::loadShader("resources/shaders/vulkan/shader"));
	renderShader->add(ubo);
	renderShader->add(texture, 1);
	renderShader->setup();

	MeshData* data = MeshBuilder::createQuad(Vector2f(-0.5f, -0.5f), Vector2f(0.5f, -0.5f), Vector2f(0.5f, 0.5f), Vector2f(-0.5f, 0.5f), texture);
	quad = new MeshRenderData(data, new RenderShader("Shader", renderShader));
}

void Test::update() {
	if (utils_time::getSeconds() - lastTime > 0.5f) {
		lastTime = utils_time::getSeconds();
		std::cout << getFPS() << std::endl;
	}
	//Update the UBO
	uboData.mvpMatrix = Matrix4f().initOrthographic(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);

	renderShader->getUBO(0)->update(&uboData, 0, sizeof(VulkanRenderShader::UBOData));
}

void Test::render() {
//	if (! getSettings().videoVulkan) {
//		glClear(GL_COLOR_BUFFER_BIT);
//		shader->use();
//	}
//	quad->render();
//	if (! getSettings().videoVulkan)
//		shader->stopUsing();

	quad->render();
}

void Test::destroy() {
	delete quad;

	//delete texture; Handled by Resource
	delete renderShader;
	delete ubo;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
