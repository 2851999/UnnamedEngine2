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

#include "../utils/Utils.h"

class Test : public BaseEngine {
private:
	float lastTime = 0;

	struct UBOData {
		Matrix4f mvpMatrix;
	};

	UBO* ubo;
	Shader* shader;
	RenderShader* renderShader;
	MeshRenderData* quad;
	UBOData uboData;
	Texture* texture;
	TextureSet* textureSet;
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
	//Shader::compileEngineShaderToSPIRV("MaterialShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");

	texture = Texture::loadTexture("resources/textures/texture.jpg");

	ubo = new UBO(NULL, sizeof(UBOData), GL_DYNAMIC_DRAW, 0);

	MeshData* data = MeshBuilder::createQuad(Vector2f(-0.5f, -0.5f), Vector2f(0.5f, -0.5f), Vector2f(0.5f, 0.5f), Vector2f(-0.5f, 0.5f), texture);

	shader = Shader::loadShader("resources/shaders/vulkan/shader");
	renderShader = new RenderShader(100, shader, NULL);

	quad = new MeshRenderData(data, renderShader);
	quad->getRenderData()->add(100, ubo);
	textureSet = new TextureSet();
	textureSet->add(1, texture);
	quad->getRenderData()->addTextureSet(textureSet);
	std::vector<Material*> materials;
	quad->setup(data, materials);
}


void Test::update() {
	if (utils_time::getSeconds() - lastTime > 0.5f) {
		lastTime = utils_time::getSeconds();
		std::cout << getFPS() << std::endl;
	}
	//Update the UBO
	uboData.mvpMatrix = Matrix4f().initOrthographic(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);

	ubo->update(&uboData, 0, sizeof(UBOData));
}

void Test::render() {
	if (! getSettings().videoVulkan) {
		glClear(GL_COLOR_BUFFER_BIT);
		shader->use();
		quad->getRenderData()->getTextureSet(0)->bindGLTextures();
	}
	quad->render();
	if (! getSettings().videoVulkan)
		shader->stopUsing();
}

void Test::destroy() {
	delete textureSet;
	delete quad;

	delete renderShader;
	delete ubo;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
