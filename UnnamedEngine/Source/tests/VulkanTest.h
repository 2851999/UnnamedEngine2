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

	std::vector<float> vertices = {
			-0.5f, -0.5f,     1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
			 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
			 0.5f,  0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
			0, 1, 2, 2, 3, 0
	};

	VBO<float>* vertexBuffer;
	VBO<unsigned int>* indexBuffer;
	RenderData* quad;

	Shader* shader;
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
	quad = new RenderData(GL_TRIANGLES, 6);
	vertexBuffer = new VBO<float>(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
	vertexBuffer->addAttribute(0, 2);
	vertexBuffer->addAttribute(1, 3);
	vertexBuffer->addAttribute(2, 2);
	indexBuffer  = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices, GL_STATIC_DRAW);
	quad->addVBO(vertexBuffer);
	quad->setIndicesVBO(indexBuffer);
	quad->setup();

	if (! getSettings().videoVulkan) {
		std::string path = "resources/shaders/vulkan/shader";
		shader = Shader::createShader(Shader::loadShaderSource(path + ".vert"), Shader::loadShaderSource(path + ".frag"));
	}
}

void Test::update() {
	if (utils_time::getSeconds() - lastTime > 0.5f) {
		lastTime = utils_time::getSeconds();
		std::cout << getFPS() << std::endl;
	}
}

void Test::render() {
	if (! getSettings().videoVulkan) {
		glClear(GL_COLOR_BUFFER_BIT);
		shader->use();
	}
	quad->render();
	if (! getSettings().videoVulkan)
		shader->stopUsing();
}

void Test::destroy() {
	delete quad;
	delete indexBuffer;
	delete vertexBuffer;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
