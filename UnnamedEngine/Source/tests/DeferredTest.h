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
#include "../core/render/RenderScene.h"
#include "../core/render/MeshLoader.h"
#include "../utils/GLUtils.h"
#include "../core/vulkan/Vulkan.h"

class Test : public BaseEngine {
private:
	DebugCamera* camera;
	RenderScene* renderScene;
	GameObject3D* model;
	GameObject3D* model2;
	GameObject3D* mit1;
	Light* light;
	Light* lightDir;
	std::vector<Light*> manyLights;
	float currentExposure;
	float exposureChangeDir;
	float minExposure = 0.1f;
	float maxExposure = 2.0f;
public:
	static bool useVulkan;

	void initialise() override;
	void created() override;
	void update() override;
	void renderOffscreen() override;
	void render() override;
	void destroy() override;

	virtual void onKeyPressed(int key) override;
};

bool Test::useVulkan = false;

void Test::initialise() {
	getSettings().videoVulkan = useVulkan;
	getSettings().videoMaxFPS = 0;
	getSettings().videoResolution = VideoResolution::RES_1080p;
	getSettings().windowFullscreen = false;
	getSettings().videoVSync = 0;
	getSettings().videoSamples = 0;
	getSettings().videoMaxAnisotropicSamples = 16;
	getSettings().debugShowInformation = true;

	getSettings().debugVkValidationLayersEnabled = false;

	VulkanSwapChain::clearDefaultDepthBufferOnLoad = false;
}

void Test::created() {
	//Shader::compileEngineShaderToSPIRV("FontShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("FontSDFShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("MaterialShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("SkyBoxShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/LightingShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/LightingShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("FramebufferShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/ShadowMapShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/ShadowMapShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("lighting/ShadowCubemapShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/ShadowCubemapShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("postprocessing/GammaCorrectionFXAAShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("lighting/DeferredLightingGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY" });
	//Shader::compileEngineShaderToSPIRV("lighting/DeferredLightingGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("lighting/DeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRDeferredGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY" });
	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRDeferredGeometry", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });
	//Shader::compileEngineShaderToSPIRV("basicpbr/PBRDeferredLighting", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");
	//Shader::compileEngineShaderToSPIRV("postprocessing/SSRShader", "C:/VulkanSDK/1.2.141.0/Bin/glslangValidator.exe");

	InputBindings* bindings = new InputBindings();
	bindings->load("C:/UnnamedEngine/config/Controller.xml", getWindow()->getInputManager());
	camera = new DebugCamera(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f, bindings);

	//camera = new DebugCamera(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->setPosition(0.0f, 4.0f, 3.0f);
	camera->setRotation(Vector3f(-20.0f, -45.0f, 0.0f));
	camera->setFlying(true);
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/textures/skybox2/", ".jpg"));
	Renderer::addCamera(camera);
	getWindow()->disableCursor();

	TextureParameters::DEFAULT_FILTER = TextureParameters::Filter::LINEAR_MIPMAP_LINEAR;
	MeshLoader::loadDiffuseTexturesAsSRGB = true;

	unsigned int shader = Renderer::SHADER_LIGHTING;
	unsigned int shaderSkinning = Renderer::SHADER_LIGHTING_SKINNING;

	currentExposure = 0.5f;
	exposureChangeDir = 1.0f;
	renderScene = new RenderScene(true, true, true, true);
	renderScene->setPostProcessingParameters(true, true, currentExposure);

	//renderScene->disableLighting();

	utils_random::initialise();

	//renderScene->addLight((new Light(Light::TYPE_DIRECTIONAL))->setDirection(0.0f, -1.0f, 0.4f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f)));
	for (unsigned int i = 0; i < 10; ++i) {
		Light* light = (new Light(Light::TYPE_POINT, Vector3f(utils_random::randomFloat(-30.0f, 30.0f), utils_random::randomFloat(0.0f, 18.0f), utils_random::randomFloat(-12.0f, 12.0f)), true))->setDiffuseColour(Colour(utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f), utils_random::randomFloat(10.0f, 30.0f)));
		renderScene->addLight(light);
		manyLights.push_back(light);
	}
	//light = (new Light(Light::TYPE_POINT, Vector3f(1.5f, 1.2f, 2.0f), false))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	//renderScene->addLight(light);
	//renderScene->addLight((new Light(Light::TYPE_SPOT, Vector3f(0.5f, 5.0f, 2.0f), true))->setDirection(0.1f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f)));

	//lightDir = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), true))->setDirection(0.0f, -1.0f, 0.0001f);
	light = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), true, 35.0f))->setDirection(0.1f, -1.0f, 0.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	//light = (new Light(Light::TYPE_SPOT, Vector3f(10.0f, 8.0f, 0.0f), true))->setDirection(0.1f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	//light = (new Light(Light::TYPE_POINT, Vector3f(0.5f, 5.0f, 2.0f), true))->setDiffuseColour(Colour(203.47f, 201.31f, 200.79f));
	lightDir = (new Light(Light::TYPE_SPOT, Vector3f(0.0f, 5.0f, 0.0f), true))->setDirection(0.1f, -1.0f, 0.0f)->setInnerCutoffDegrees(25.0f)->setOuterCutoffDegrees(35.0f)->setDiffuseColour(Colour(203.47f, 201.31f, 200.79f));
	renderScene->addLight(light);
	renderScene->addLight(lightDir);

	//camera->getSkyBox()->getBox()->getMaterial()->setDiffuse(light->getShadowMapRenderPass()->getFBO()->getAttachment(0));
	//camera->getSkyBox()->getBox()->getMaterial()->update();

	Mesh* mesh = MeshLoader::loadModel("C:/UnnamedEngine/models/crytek-sponza/", "sponza.obj");
	//mesh->setCullingEnabled(false);
	//Mesh* mesh = MeshLoader::loadModel("C:/UnnamedEngine/models/", "teapot.obj");

	model = new GameObject3D(mesh, shader);
	model->setScale(0.15f, 0.15f, 0.15f);
	model->update();
	renderScene->add(model);

	Mesh* mesh2 = MeshLoader::loadModel("C:/UnnamedEngine/models/plane/", "plane2.obj");

	model2 = new GameObject3D(mesh2, shader);
	model2->setPosition(0.0f, 1.5f, 0.0f);
	model2->update();
	renderScene->add(model2);

	//mitsuba-sphere.obj
	//mit1 = new GameObject3D(MeshLoader::loadModel("C:/UnnamedEngine/models/mitsuba/", "mitsuba-sphere.obj"), shader);
	mit1 = new GameObject3D(MeshLoader::loadModel("C:/UnnamedEngine/models/Sphere-Bot Basic/", "bot.dae"), shaderSkinning);
	mit1->getMesh()->getSkeleton()->startAnimation("");
	mit1->setPosition(10.0f, 1.0f, 0.0f);
	mit1->getMesh()->getMaterial(2)->setShininess(Texture::loadTexture("C:/UnnamedEngine/models/Sphere-Bot Basic/Sphere_Bot_rough.jpg")); //Need to be loaded separately for some reason
	mit1->getMesh()->getMaterial(2)->setNormalMap(Texture::loadTexture("C:/UnnamedEngine/models/Sphere-Bot Basic/Sphere_Bot_nmap_1.jpg"));
	mit1->getMesh()->getMaterial(2)->update();
	mit1->update();
	renderScene->add(mit1);

	//model2->getMesh()->getMaterial(1)->setDiffuse(Renderer::getBlankTexture());
	//model2->getMesh()->getMaterial(1)->update();

	//Make OpenGL's depth values behave like Vulkan
	//https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_clip_control.txt
	//glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
}

void Test::update() {
	camera->update(getDeltaSeconds());

	//model2->getMesh()->getMaterial(1)->setDiffuse(lightDir->getShadowMapRenderPass()->getFBO()->getAttachment(0));
	//model2->getMesh()->getMaterial(1)->setDiffuse(Renderer::getBlankTexture());
	//model2->getMesh()->getMaterial(1)->update();

	if (Keyboard::isPressed(GLFW_KEY_UP))
		lightDir->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		lightDir->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		lightDir->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		lightDir->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);

	mit1->getMesh()->updateAnimation(getDeltaSeconds());

	light->update();
	lightDir->update();

	for (Light* light : manyLights) {
		light->getTransform()->translate(Vector3f(sin(utils_time::getSeconds()), 0.0f, cos(utils_time::getSeconds())) * 0.04f);
		light->update();
	}

	//currentExposure += exposureChangeDir * (1.0f * getDeltaSeconds());
	//if (currentExposure <= minExposure) {
	//	currentExposure = minExposure;
	//	exposureChangeDir *= -1;
	//} else if (currentExposure >= maxExposure) {
	//	currentExposure = maxExposure;
	//	exposureChangeDir *= -1;
	//}
	//renderScene->setPostProcessingParameters(true, true, currentExposure);

	if (Keyboard::isPressed(GLFW_KEY_HOME)) {
		currentExposure += (10.0f * getDeltaSeconds());
		renderScene->setPostProcessingParameters(true, true, currentExposure);
	} else if (Keyboard::isPressed(GLFW_KEY_END)) {
		currentExposure -= (10.0f * getDeltaSeconds());
		renderScene->setPostProcessingParameters(true, true, currentExposure);
	}
}

void Test::renderOffscreen() {
	renderScene->renderOffscreen();
}

void Test::render() {
	renderScene->render();

	camera->render();

	//camera->useView();
}

void Test::destroy() {
	delete renderScene;
	delete camera;
}

void Test::onKeyPressed(int key) {
	if (key == GLFW_KEY_ESCAPE)
		requestClose();
}