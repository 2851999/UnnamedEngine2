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
#include "../core/render/RenderScene.h"
#include "../core/render/MeshLoader.h"
#include "../utils/Utils.h"
#include "../utils/DebugCamera.h"
#include "../core/vulkan/Vulkan.h"
#include "../utils/Logging.h"
#include "../utils/GLUtils.h"

class Test : public BaseEngine {
private:
	unsigned int count = 0;
	DebugCamera* camera;
	RenderScene3D* renderScene;
	GameObject3D* model;
	GameObject3D* model2;
	Light* light0;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;

	virtual void onKeyPressed(int key) override;
};

void Test::initialise() {
	getSettings().debugVkValidationLayersEnabled = false;

	getSettings().videoVulkan = true;
	getSettings().videoMaxFPS = 0;
	getSettings().videoResolution = VideoResolution::RES_1080p;
	getSettings().videoVSync = 0;
	getSettings().debugShowInformation = true;
	getSettings().videoSamples = 8;
	getSettings().videoMaxAnisotropicSamples = 16;
}

void Test::created() {
//	Logger::startFileOutput("C:/UnnamedEngine/logs.txt");
//	Shader::compileEngineShaderToSPIRV("FontShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");
//	Shader::compileEngineShaderToSPIRV("MaterialShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");
//	Shader::compileEngineShaderToSPIRV("SkyBoxShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");
//	Shader::compileEngineShaderToSPIRV("VulkanLightingShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");
//	Shader::compileEngineShaderToSPIRV("lighting/LightingShader", "C:/VulkanSDK/1.1.70.1/Bin32/glslangValidator.exe");

	camera = new DebugCamera(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->setPosition(0.0f, 0.0f, 1.0f);
	camera->setFlying(true);
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/textures/skybox2/", ".jpg"));
	Renderer::addCamera(camera);
	getWindow()->disableCursor();

	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;
	MeshLoader::loadDiffuseTexturesAsSRGB = false;

	renderScene = new RenderScene3D();
	light0 = (new Light(Light::TYPE_POINT, Vector3f(0.5f, 2.0f, 2.0f), false))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	renderScene->addLight(light0);

	if (!getSettings().videoVulkan)
		//Ensure Vulkan and OpenGL apply same post processing (i.e. none as not supported for Vulkan currently)
		renderScene->disableGammaCorrection();

//	renderScene->disableLighting();

//	Texture* texture = Texture::loadTexture("resources/textures/texture.jpg");
//	Mesh* mesh = new Mesh(MeshBuilder::createQuad3D(Vector2f(-0.5f, -0.5f), Vector2f(0.5f, -0.5f), Vector2f(0.5f, 0.5f), Vector2f(-0.5f, 0.5f), texture));
//	mesh->getMaterial()->setDiffuse(texture);
	//mesh->getMaterial(1)->setDiffuse(texture);
//	mesh->getMaterial()->setDiffuse(Colour(1.0f, 0.0f, 0.0f, 1.0f));

	Mesh* mesh = MeshLoader::loadModel("C:/UnnamedEngine/models/crytek-sponza/", "sponza.obj");

	mesh->setCullingEnabled(false);
	model = new GameObject3D(mesh, Renderer::SHADER_VULKAN_LIGHTING);
	model->setScale(0.15f, 0.15f, 0.15f);
	model->update();
	renderScene->add(model);

//	Mesh* mesh2 = MeshLoader::loadModel("C:/UnnamedEngine/models/Sphere-Bot Basic/", "bot.dae");
//	mesh2->getSkeleton()->startAnimation("");

	Mesh* mesh2 = MeshLoader::loadModel("C:/UnnamedEngine/models/plane/", "plane2.obj"); //Assimp won't load this file
	//Mesh* mesh2 = new Mesh(MeshBuilder::createCube(10.0f, 10.0f, 10.0f));

	mesh2->setCullingEnabled(false);
	model2 = new GameObject3D(mesh2, Renderer::SHADER_VULKAN_LIGHTING);
	model2->setPosition(4.0f, 1.0f, 0.0f);
	model2->update();
	renderScene->add(model2);
}


void Test::update() {
	camera->update(getDeltaSeconds());

	if (Keyboard::isPressed(GLFW_KEY_UP))
		light0->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		light0->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		light0->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		light0->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);
	light0->update();

//	model2->getMesh()->updateAnimation(getDeltaSeconds());
}

void Test::render() {
	count++;
//	if (count == 4)
//		requestClose();
	if (! getSettings().videoVulkan) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		utils_gl::setupAlphaBlendingMSAA();
	}

	renderScene->render();

	camera->useView();
}

void Test::destroy() {
	//Logger::stopFileOutput();
	delete model;
	delete model2;
	delete camera;
}

void Test::onKeyPressed(int key) {
	if (key == GLFW_KEY_ESCAPE)
		requestClose();
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
