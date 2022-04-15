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

#pragma once

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"
#include "../utils/GLUtils.h"
#include "../core/render/pbr/PBREnvironment.h"

class Test : public BaseTest3D {
private:
	//	Texture* albedo;
	//	Texture* normal;
	//	Texture* metallic;
	//	Texture* roughness;
	//	Texture* ao;

	RenderScene* scene;
	RenderShader* pbrRenderShader;
	RenderShader* pbrRenderShaderSkinning;
	PBREnvironment* environment;

	Light* light0;

	GameObject3D* mit1;

	bool deferred = true;
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
	getSettings().videoVSync = false;
	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = deferred ? 0 : 16;
	getSettings().videoResolution = VideoResolution::RES_1080p;
	getSettings().videoVulkan = false;
	getSettings().debugVkValidationLayersEnabled = true;
	//getSettings().videoRefreshRate = 144;
	//getSettings().windowFullscreen = true;

	//Logger::startFileOutput("C:/UnnamedEngine/logs.txt");

	//Should not be here if not using offscreen rendering in RenderShader
	VulkanSwapChain::clearDefaultDepthBufferOnLoad = false;
}

void Test::onCreated() {
	//Shader::compileEngineShaderToSPIRV("SkyBoxShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("postprocessing/SSRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/GenEquiToCube", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/GenIrradianceMap", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/GenPrefilterMap", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/GenBRDFIntegrationMap", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/PBRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/PBRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("pbr/PBRDeferredGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY" });
	//Shader::compileEngineShaderToSPIRV("pbr/PBRDeferredGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });

	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRDeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("pbr/PBRDeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");

	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRDeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_BLOOM" });
	//Shader::compileEngineShaderToSPIRV("pbr/PBRDeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_BLOOM" });
	//Shader::compileEngineShaderToSPIRV("postprocessing/GaussianBlur", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("postprocessing/BloomShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");

	//Logger::stopFileOutput();

//	GLint num;
//	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &num);
//	std::cout << num << std::endl;

//	GLint num;
//	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &num);
//	std::cout << num << std::endl;
	MeshLoader::loadDiffuseTexturesAsSRGB = true;

	//MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "SimpleSphere.obj");

	environment = PBREnvironment::loadAndGenerate(resourceLoader.getAbsPathTextures() + "PBR/WinterForest_Ref.hdr"); //Milkyway_small
	//EquiToCube::generateCubemapAndIrradiance(resourceLoader.getAbsPathTextures() + "PBR/Theatre-Center_2k.hdr", envMap, irMap, prefilMap, brdfLUTMap);

	camera->setSkyBox(new SkyBox(environment->getEnvironmentCubemap()));
	camera->setFlying(true);

	pbrRenderShader = Renderer::getRenderShader(Renderer::SHADER_LIGHTING);
	pbrRenderShaderSkinning = Renderer::getRenderShader(Renderer::SHADER_LIGHTING_SKINNING);

	scene = new RenderScene(deferred, true, true, true, true, environment);
	scene->setPostProcessingParameters(true, true, 0.5f);

	light0 = (new Light(Light::TYPE_POINT, Vector3f(0.5f, 5.0f, 2.0f), true))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));

	//light0 = (new Light(Light::TYPE_SPOT, Vector3f(0.5f, 5.0f, 2.0f), true))->setDirection(0.1f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));

	utils_random::initialise();
	/*
	for (unsigned int i = 0; i < 10; ++i)
		scene->addLight((new Light(Light::TYPE_POINT, Vector3f(utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f), utils_random::randomFloat(-10.0f, 10.0f)), false))->setDiffuseColour(Colour(utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f))));
	*/

	//camera->setProjectionMatrix(light0->getLightProjectionMatrix());

	//Light* light1 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, -1.0f, 0.0001f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	//light1->update();
	scene->addLight(light0);
	//	scene->addLight(light1);
	//	scene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));
	//	scene->addLight((new Light(Light::TYPE_SPOT, Vector3f(0.0f, 3.0f, 0.0f), false))->setDirection(0.0f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f))); //Sphere appears off-centre
	//	scene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));

		//std::string path = "C:/UnnamedEngine/textures/PBR/";

	//	GameObject3D* plane = new GameObject3D(resourceLoader.loadModel("plane/", "plane2.model"), pbrRenderShader);
	//	plane->setPosition(0.0f, -2.0f, 0.0f);
	//	plane->update();
	//	scene->add(plane);

	for (int i = 0; i < 16; ++i) {
		GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj"), pbrRenderShader);
		Material* material = sphere->getMesh()->getMaterial(1);

		int x = i % 4;
		int y = (int)(i / 4.0f);

		sphere->setPosition(x * 2, y * 2, -0.5f);

		material->setAlbedo(Colour(0.5f, 0.0f, 0.0f));
		material->setMetalness(x * (1.0f / 3.0f));
		material->setRoughness(utils_maths::clamp(y * (1.0f / 3.0f), 0.05f, 1.0f));
		material->update();

		sphere->update();

		scene->add(sphere);
	}

	//resourceLoader.loadModel("", "SimpleSphere.model") //Normals not smooth????
	//MeshLoader::loadModel("resources/objects/", "plain_sphere.model")

	//GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj"), pbrRenderShader);
	GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("crytek-sponza/", "sponza.obj"), pbrRenderShader);
	sphere->setScale(0.15f, 0.15f, 0.15f);
	sphere->update();
	scene->add(sphere);

	GameObject3D* sphere2 = new GameObject3D(resourceLoader.loadPBRModel("box/", "CornellBox-Glossy.obj"), pbrRenderShader);
	sphere2->setPosition(15.0f, 1.8f, 0.0f);
	sphere2->setScale(2.0f, 2.0f, 2.0f);
	sphere2->update();
	scene->add(sphere2);

	//mitsuba-sphere.obj
	mit1 = new GameObject3D(resourceLoader.loadPBRModel("Sphere-Bot Basic/", "bot.dae"), pbrRenderShaderSkinning);
	mit1->getMesh()->getSkeleton()->startAnimation("");

	//std::cout << mit1->getMesh()->getMaterial(2)->diffuseTexture->getPath() << std::endl;

	mit1->getMesh()->getMaterial(2)->setShininess(Texture::loadTexture(resourceLoader.getAbsPathModels() + "Sphere-Bot Basic/Sphere_Bot_rough.jpg"));
	mit1->getMesh()->getMaterial(2)->setNormalMap(Texture::loadTexture(resourceLoader.getAbsPathModels() + "Sphere-Bot Basic/Sphere_Bot_nmap_1.jpg"));
	mit1->getMesh()->getMaterial(2)->update();

	//mit1->setScale(0.5f, 0.5f, 0.5f);
	mit1->setPosition(10.0f, 1.0f, 0.0f);
	mit1->update();
	scene->add(mit1);

	//Mesh* mesh2 = MeshLoader::loadModel("C:/UnnamedEngine/models/plane/", "plane2.obj");

	//GameObject3D* model2 = new GameObject3D(mesh2, pbrRenderShader);
	//model2->setPosition(0.0f, 1.5f, 0.0f);
	//model2->update();
	//model2->getMesh()->getMaterial(1)->setDiffuse(environment->getBRDFLUTTexture());
	//model2->getMesh()->getMaterial(1)->update();
	//scene->add(model2);


	//	GameObject3D* testObject = new GameObject3D(resourceLoader.loadPBRModel("pbr/", "Cerberus_LP.FBX"), pbrRenderShader);
	//	testObject->setScale(0.05f, 0.05f, 0.05f);
	//	testObject->setPosition(0.0f, -2.0f, 0.0f);
	//	testObject->update();
	//	Material* mat = testObject->getMesh()->getMaterial(0);
	//
	//	mat->setAlbedo(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_A.tga", TextureParameters().setSRGB(true)));
	//	mat->setMetalness(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_M.tga"));
	//	mat->setRoughness(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_R.tga"));
	//	mat->setNormalMap(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_N.tga"));
	//	mat->setAlbedo(Colour(1.0f, 1.0f, 1.0f));
	//	mat->setRoughness(1.0f);
	//	mat->setMetalness(1.0f);
	//	mat->setAO(1.0f);

		//testObject->getMesh()->getMaterials()[1] = mat;

	//	scene->add(testObject);

	camera->setMovementSpeed(5.0f);
}

void Test::onUpdate() {
	if (Keyboard::isPressed(GLFW_KEY_UP))
		light0->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		light0->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		light0->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		light0->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);
	light0->update();

	mit1->getMesh()->updateAnimation(getDeltaSeconds());

	//camera->setViewMatrix(light0->getLightShadowTransform(1));
}

void Test::onRenderOffscreen() {
	scene->renderOffscreen();
}

void Test::onRender() {
	scene->render();

	camera->render();
}

void Test::onDestroy() {
	delete scene;
}

void Test::onKeyPressed(int key) {
	BaseTest3D::onKeyPressed(key);
	if (key == GLFW_KEY_Q)
		scene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));
	//else if (key == GLFW_KEY_1)
	//	scene->setExposure(0.25f);
	//else if (key == GLFW_KEY_2)
	//	scene->setExposure(0.5f);
	//else if (key == GLFW_KEY_3)
	//	scene->setExposure(1.0f);
	//else if (key == GLFW_KEY_4)
	//	scene->setExposure(2.0f);
	//else if (key == GLFW_KEY_5)
	//	scene->setExposure(4.0f);
	//else if (key == GLFW_KEY_6)
	//	scene->setExposure(8.0f);
	//else if (key == GLFW_KEY_9)
	//	scene->enableFXAA();
	//else if (key == GLFW_KEY_0)
	//	scene->disableFXAA();
}
