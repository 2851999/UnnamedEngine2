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
	Shader* rtShader;
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
	std::string glslangValidatorPath = "C:/VulkanSDK/1.3.204.1/Bin/glslangValidator.exe";

	//Renderer::compileEngineShadersToSPIRV(glslangValidatorPath);

	//Renderer::compileEngineShaderToSPIRV(Renderer::SHADER_MATERIAL, glslangValidatorPath);

	//Renderer::compileEngineShaderToSPIRV(Renderer::SHADER_TEXTURE_PASSTHROUGH, glslangValidatorPath);

	//Shader::compileEngineShaderToSPIRV("raytracing/material/", { "raygen.rgen", "miss.rmiss", "closesthit.rchit" }, glslangValidatorPath);
	Shader::compileEngineShaderToSPIRV("raytracing/pathtracing/", { "raygen.rgen", "miss.rmiss", "closesthit.rchit" }, glslangValidatorPath);
	//Shader::compileEngineShaderToSPIRV("raytracing/basicpbr/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit" }, glslangValidatorPath);
	//Shader::compileEngineShaderToSPIRV("raytracing/pbr/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit" }, glslangValidatorPath);

	//Shader::compileEngineShaderToSPIRV("raytracing/7/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit" }, glslangValidatorPath);

	//rtScene = new RaytracedScene(true);
	rtScene = new RaytracedScene(false);

	//Mesh* mesh1 = resourceLoader.loadModel("", "cube.obj");
	Mesh* mesh1 = resourceLoader.loadPBRModel("crytek-sponza/", "sponza.obj");
	//Mesh* mesh1 = resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere2.obj");
	//Mesh* mesh1 = resourceLoader.loadModel("bob/", "bob_lamp_update.blend");
	//Mesh* mesh1 = resourceLoader.loadModel("", "buddha.obj");
	//Mesh* mesh1 = resourceLoader.loadPBRModel("box/", "CornellBox-Glossy.obj");
	//Mesh* mesh1 = resourceLoader.loadPBRModel("box/", "CornellBox-test.obj");
	//Mesh* mesh1 = resourceLoader.loadModel("", "cube-coloured.obj");

	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/", "cube-coloured.obj", false);
	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/box/", "CornellBox-Glossy.obj", false);
	//modelObjects = MeshLoader::loadAssimpModelSeparate("C:/UnnamedEngine/models/crytek-sponza/", "sponza.obj", false);

	mesh1->enableRaytracing();

	model1 = new GameObject3D(mesh1, Renderer::SHADER_MATERIAL);
	model1->setScale(0.15f, 0.15f, 0.15f);
	model1->update();

	//Mesh* mesh2 = resourceLoader.loadModel("", "cube-coloured.obj");
	Mesh* mesh2 = resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere3.obj");
	mesh2->enableRaytracing();

	model2 = new GameObject3D(mesh2, Renderer::SHADER_MATERIAL);
	model2->setScale(3.0f, 3.0f, 3.0f);
	model2->setPosition(10.0f, 1.0f, 0.0f);
	model2->update();

	rtScene->add(model1);
	rtScene->add(model2);

	for (int i = 0; i < 16; ++i) {
		Mesh* mesh = resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj");
		mesh->enableRaytracing();
		GameObject3D* sphere = new GameObject3D(mesh, Renderer::SHADER_MATERIAL);
		Material* material = sphere->getMesh()->getMaterial(1);

		int x = i % 4;
		int y = (int) (i / 4.0f);

		sphere->setPosition(x * 2, y * 2, -0.5f);

		material->setAlbedo(Colour(0.5f, 0.0f, 0.0f));
		material->setMetalness(x * (1.0f / 3.0f));
		material->setRoughness(utils_maths::clamp(y * (1.0f / 3.0f), 0.05f, 1.0f));
		material->update();

		sphere->update();

		rtScene->add(sphere);
	}

	//rtScene->addLight((new Light(Light::TYPE_POINT, Vector3f(0.5f, 5.0f, 2.0f)))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f) * 10.0f));
	//rtScene->addLight((new Light(Light::TYPE_DIRECTIONAL, Vector3f()))->setDirection(0, -1.0f, 0.1f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f)));
	//rtScene->addLight((new Light(Light::TYPE_SPOT, Vector3f(1.0f, 3.0f, 0.0f)))->setDirection(0.0f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f)));

	//for (unsigned int i = 0; i < 15; ++i) {
	//	rtScene->addLight((new Light(Light::TYPE_POINT, Vector3f(utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f)), false))->setDiffuseColour(Colour(utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f))));
	//}

	//for (unsigned int i = 0; i < 20; ++i) {
	//	Mesh* newMesh = resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj");
	//	newMesh->getMaterial(1)->setEmissive(Colour(utils_random::randomFloat(0.0f, 100.0f), utils_random::randomFloat(0.0f, 100.0f), utils_random::randomFloat(0.0f, 100.0f)));
	//	newMesh->enableRaytracing();
	//	GameObject3D* newModel = new GameObject3D(newMesh, Renderer::SHADER_MATERIAL);
	//	newModel->setPosition(utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f));
	//	newModel->update();

	//	rtScene->add(newModel);
	//}

	PBREnvironment* pbrEnvironment = PBREnvironment::loadAndGenerate(resourceLoader.getAbsPathTextures() + "PBR/Milkyway_small.hdr"); //Milkyway_small

	//camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg"));
	camera->setSkyBox(new SkyBox(pbrEnvironment->getEnvironmentCubemap()));
	camera->setFlying(true);
	camera->update(getDeltaSeconds());

	//rtShader = Shader::loadEngineShaderNames("raytracing/7/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit" });
	//rtShader = Shader::loadEngineShaderNames("raytracing/material/", { "raygen.rgen", "miss.rmiss", "closesthit.rchit" });
	rtShader = Shader::loadEngineShaderNames("raytracing/pathtracing/", { "raygen.rgen", "miss.rmiss", "closesthit.rchit" });
	//rtShader = Shader::loadEngineShaderNames("raytracing/basicpbr/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit"});
	//rtShader = Shader::loadEngineShaderNames("raytracing/pbr/", { "raygen.rgen", "miss.rmiss", "shadow.rmiss", "closesthit.rchit" });

	//rtShader = new Shader();
	//rtShader->attach(Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/7/raygen.rgen.spv")), VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	//rtShader->attach(Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/7/miss.rmiss.spv")), VK_SHADER_STAGE_MISS_BIT_KHR);
	//rtShader->attach(Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/7/shadow.rmiss.spv")), VK_SHADER_STAGE_MISS_BIT_KHR);
	//rtShader->attach(Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/7/closesthit.rchit.spv")), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);

	rtScene->setup(rtShader, camera);
	//rtScene->setup(rtShader, camera, pbrEnvironment);

	//Shader::outputCompleteShaderFiles("resources/shaders/raytracing/7/", "resources/shaders/raytracing/7/", { "raygen.rgen" });
	//Shader::compileToSPIRV("resources/shaders/raytracing/7/", "resources/shaders/raytracing/7/", { "raygen.rgen" }, glslangValidatorPath);
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
	delete rtShader;
}

void Test::onKeyPressed(int key) {
	BaseTest3D::onKeyPressed(key);
}