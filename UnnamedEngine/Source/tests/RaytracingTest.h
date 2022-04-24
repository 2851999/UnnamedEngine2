/*****************************************************************************
 *
 *   Copyright 2022 Joel Davies
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

#include <cassert>

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"
#include "../core/render/raytracing/RaytracedScene.h"
#include "../core/vulkan/VulkanExtensions.h"
#include "../utils/GLUtils.h"
#include "../utils/VulkanUtils.h"

//Made folowing
//https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/
//and with reference to
//https://github.com/SaschaWillems/Vulkan/blob/master/examples/raytracingbasic/raytracingbasic.cpp

class Test : public BaseTest3D {
private:
	GameObject3D* model1;
	GameObject3D* model2;

	RaytracedScene* rtScene;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRenderOffscreen() override;
	virtual void onRender() override;
	virtual void onDestroy() override;

	virtual void onKeyPressed(int key) override;
};

void Test::onInitialise() {
	getSettings().videoVSync = true;
	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = 0;
	getSettings().videoVulkan = true;
	getSettings().videoRaytracing = true;
	getSettings().debugShowInformation = true;
	getSettings().debugVkValidationLayersEnabled = true;
}

void Test::onCreated() {
	std::string glslandValidatorPath = "C:/VulkanSDK/1.3.204.1/Bin/glslangValidator.exe";

	//Renderer::compileEngineShadersToSPIRV(glslandValidatorPath);

	//Renderer::compileEngineShaderToSPIRV(Renderer::SHADER_MATERIAL, glslandValidatorPath);

	//Renderer::compileEngineShaderToSPIRV(Renderer::SHADER_TEXTURE_PASSTHROUGH, glslandValidatorPath);

	rtScene = new RaytracedScene();

	//Mesh* mesh1 = resourceLoader.loadModel("", "cube.obj");
	//Mesh* mesh1 = resourceLoader.loadPBRModel("crytek-sponza/", "sponza.obj");
	//Mesh* mesh1 = resourceLoader.loadModel("bob/", "bob_lamp_update.blend");
	//Mesh* mesh1 = resourceLoader.loadModel("", "buddha.obj");
	//Mesh* mesh1 = resourceLoader.loadPBRModel("box/", "CornellBox-Glossy.obj");
	Mesh* mesh1 = resourceLoader.loadPBRModel("box/", "CornellBox-test.obj");
	//Mesh* mesh1 = resourceLoader.loadModel("", "cube-coloured.obj");

	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/", "cube-coloured.obj", false);
	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/box/", "CornellBox-Glossy.obj", false);
	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/crytek-sponza/", "sponza.obj", false);

	mesh1->enableRaytracing();

	model1 = new GameObject3D(mesh1, Renderer::SHADER_MATERIAL);
	model1->update();

	Mesh* mesh2 = resourceLoader.loadModel("", "cube-coloured.obj");
	mesh2->enableRaytracing();

	model2 = new GameObject3D(mesh2, Renderer::SHADER_MATERIAL);
	model2->setPosition(3.0f, 0.0f, 0.0f);
	model2->update();

	rtScene->add(model1);
	rtScene->add(model2);

	camera->setFlying(true);
	camera->setPosition(Vector3f(1.0f, 2.0f, 4.0f));
	camera->update(getDeltaSeconds());

	VkShaderModule raygenShader = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/6/raygen.rgen.spv"));
	VkShaderModule missShader = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/6/miss.rmiss.spv"));
	VkShaderModule shadowMissShader = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/6/shadow.rmiss.spv"));
	VkShaderModule closestHitShader = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/6/closesthit.rchit.spv"));

	rtScene->setup(raygenShader, { missShader, shadowMissShader }, closestHitShader);

	//Shader modules not needed anymore
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), raygenShader, nullptr);
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), missShader, nullptr);
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), shadowMissShader, nullptr);
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), closestHitShader, nullptr);
}

void Test::onUpdate() {
	//std::cout << camera->getPosition().toString() << std::endl;
}

void Test::onRenderOffscreen() {
	rtScene->raytrace(camera);
}

void Test::onRender() {
	rtScene->renderOutput();
}

void Test::onDestroy() {
	delete rtScene;
	delete model1;
	delete model2;
}

void Test::onKeyPressed(int key) {
	BaseTest3D::onKeyPressed(key);
}