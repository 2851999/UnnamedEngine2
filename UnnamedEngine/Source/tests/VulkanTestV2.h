/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#pragma once

#include "../core/BaseEngine.h"
#include "../utils/DebugCamera.h"
#include "../experimental/RenderSceneV2.h"
#include "../core/render/MeshLoader.h"
#include "../utils/GLUtils.h"

class Test : public BaseEngine {
private:
	DebugCamera* camera;
	RenderSceneV2* renderScene;
	GameObject3D* model;
	GameObject3D* model2;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;

	virtual void onKeyPressed(int key) override;
};

void Test::initialise() {
	getSettings().videoVulkan = true;
	getSettings().videoMaxFPS = 0;
	getSettings().videoResolution = VideoResolution::RES_1080p;
	getSettings().videoVSync = 0;
	getSettings().videoSamples = 8;
	getSettings().videoMaxAnisotropicSamples = 16;
	getSettings().debugShowInformation = true;

	getSettings().debugVkValidationLayersEnabled = true;
}

void Test::created() {
	//Shader::compileEngineShaderToSPIRV("FontShader", "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("MaterialShader", "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("SkyBoxShader", "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("VulkanLightingShader", "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/LightingShader", "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe");


	camera = new DebugCamera(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->setPosition(0.0f, 0.0f, 1.0f);
	camera->setFlying(true);
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/textures/skybox2/", ".jpg"));
	Renderer::addCamera(camera);
	getWindow()->disableCursor();

	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;
	MeshLoader::loadDiffuseTexturesAsSRGB = false;

	renderScene = new RenderSceneV2();
	renderScene->disableLighting();
	//renderScene->addLight((new Light(Light::TYPE_POINT, Vector3f(0.5f, 2.0f, 2.0f), false))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f)));

	Mesh* mesh = MeshLoader::loadModel("C:/UnnamedEngine/models/crytek-sponza/", "sponza.obj");

	model = new GameObject3D(mesh, Renderer::SHADER_MATERIAL);
	model->setScale(0.15f, 0.15f, 0.15f);
	model->update();
	renderScene->add(model);

	Mesh* mesh2 = MeshLoader::loadModel("C:/UnnamedEngine/models/plane/", "plane2.obj");

	model2 = new GameObject3D(mesh2, Renderer::SHADER_MATERIAL);
	model2->setPosition(4.0f, 1.0f, 0.0f);
	model2->update();
	renderScene->add(model2);
}

void Test::update() {
	camera->update(getDeltaSeconds());

	//model2->getMesh()->getMaterial(1)->setDiffuse(Renderer::getBlankTexture());
	//model2->getMesh()->getMaterial(1)->update();

	if (Keyboard::isPressed(GLFW_KEY_UP))
		model2->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		model2->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		model2->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		model2->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);
	model2->update();
}

void Test::render() {
	if (!getSettings().videoVulkan) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		utils_gl::setupAlphaBlendingMSAA();
	}

	renderScene->render();

	camera->useView();
}

void Test::destroy() {
	delete renderScene;
	delete camera;
}

void Test::onKeyPressed(int key) {
	if (key == GLFW_KEY_ESCAPE)
		requestClose();
}