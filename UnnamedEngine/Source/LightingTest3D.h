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

#ifndef BASEENGINETEST3D_H_
#define BASEENGINETEST3D_H_

#include "utils/Logging.h"
#include "core/BaseEngine.h"
#include "core/Object.h"
#include "core/render/Renderer.h"
#include "core/render/Mesh.h"
#include "utils/Utils.h"
#include "core/Model.h"
#include "core/render/RenderScene.h"
#include "utils/DebugCamera.h"

class Test : public BaseEngine {
private:
	DebugCamera* camera;
	GameObject3D* object;
	GameObject3D* object2;
	Scene* scene;
public:
	virtual ~Test() {}

	virtual void initialise() override;
	virtual void created() override;
	virtual void update() override;
	virtual void render() override;
	virtual void destroy() override;

	/* Input methods */
	virtual void onKeyPressed(int key) override {
		if (key == GLFW_KEY_ESCAPE)
			requestClose();
	}
};

void Test::initialise() {
	getSettings().windowTitle = "Unnamed Engine " + Engine::Version;
	getSettings().videoVSync = true;
	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = 16;
	getSettings().videoMaxAnisotropicSamples = 16;
	getSettings().windowFullscreen = false;
	getSettings().videoResolution = VideoResolution::RES_720P;
}

void Test::created() {
	getWindow()->toggleCursor();

	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;

	scene = new Scene();
	//std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/mitsuba/", "mitsuba-sphere-blue.obj");
	//std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/crytek-sponza/", "sponza.obj");
	//std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/nanosuit/", "nanosuit.obj");
	std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/plane/", "plane.obj");
	std::vector<Mesh*> meshes2 = Model::loadModel("C:/UnnamedEngine/Models/plane/", "plane2.obj");
	//std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/cyborg/", "cyborg.obj");
	//1std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/head/", "head.obj");
	object = new GameObject3D(meshes1, Renderer::getRenderShader("Lighting"));
	//object->setRotationX(90.0f);
	//object->getMaterial()->setShininess(128.0f);
	object->setScale(0.5f, 0.5f, 0.5f);

	object2 = new GameObject3D(meshes2, Renderer::getRenderShader("Lighting"));
	object2->setPosition(0.0f, -1.0f, 0.0f);

	scene->add(object);
	scene->add(object2);

	camera = new DebugCamera(Matrix4f().initPerspective(110, ((float) getSettings().windowWidth) / ((float) getSettings().windowHeight), 0.1f, 100));
	//camera->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox1/", "skybox0.png", "skybox1.png", "skybox2.png", "skybox3.png", "skybox4.png", "skybox5.png", 100));
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox2/", "front.jpg", "back.jpg", "left.jpg", "right.jpg", "top.jpg", "bottom.jpg", 100));
	camera->setFlying(true);

	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), true))->setDirection(0.0f, -1.0f, 0.00001f)
																			   ->setDiffuseColour(Colour::WHITE)
																			   ->setSpecularColour(Colour::WHITE);
	light0->update();
	scene->addLight(light0);

//	scene->addLight(Light(Light::TYPE_SPOT, Vector3f(2.0f, 0.8f, 0.5f)).setDirection(0.0f, -1.0f, 0.0f)
//																	   .setCutoff(cos(MathsUtils::toRadians(30.0f)))
//																	   .setOuterCutoff(cos(MathsUtils::toRadians(38.0f)))
//																	   .setDiffuseColour(Colour::RED)
//																	   .setSpecularColour(Colour::WHITE));
//
//	scene->addLight(Light(Light::TYPE_SPOT, Vector3f(2.0f, 0.8f, 0.5f)).setDirection(0.0f, -1.0f, 0.0f)
//																	   .setCutoff(cos(MathsUtils::toRadians(30.0f)))
//																	   .setOuterCutoff(cos(MathsUtils::toRadians(38.0f)))
//																	   .setDiffuseColour(Colour::RED)
//																	   .setSpecularColour(Colour::WHITE));
//
//	scene->addLight(Light(Light::TYPE_SPOT, Vector3f(2.0f, 0.8f, 0.5f)).setDirection(0.0f, -1.0f, 0.0f)
//																	   .setCutoff(cos(MathsUtils::toRadians(30.0f)))
//																	   .setOuterCutoff(cos(MathsUtils::toRadians(38.0f)))
//																	   .setDiffuseColour(Colour::RED)
//																	   .setSpecularColour(Colour::WHITE));
//
//	scene->addLight(Light(Light::TYPE_SPOT, Vector3f(2.0f, 0.8f, 0.5f)).setDirection(0.0f, -1.0f, 0.0f)
//																	   .setCutoff(cos(MathsUtils::toRadians(30.0f)))
//																	   .setOuterCutoff(cos(MathsUtils::toRadians(38.0f)))
//																	   .setDiffuseColour(Colour::RED)
//																	   .setSpecularColour(Colour::WHITE));
//
//	scene->addLight(Light(Light::TYPE_SPOT, Vector3f(2.0f, 0.8f, 0.5f)).setDirection(0.0f, -1.0f, 0.0f)
//																	   .setCutoff(cos(MathsUtils::toRadians(30.0f)))
//																	   .setOuterCutoff(cos(MathsUtils::toRadians(38.0f)))
//																	   .setDiffuseColour(Colour::RED)
//																	   .setSpecularColour(Colour::WHITE));

//	scene->addLight(Light(Light::TYPE_POINT, Vector3f(0.0f, 0.8f, 0.5f)).setConstantAttenuation(1.0f)
//																   .setLinearAttenuation(0.09f)
//																   .setQuadraticAttenuation(0.032f)
//																   .setDiffuseColour(Colour::GREEN)
//																   .setSpecularColour(Colour::WHITE));

	Renderer::addCamera(camera);
}

void Test::update() {
	camera->update(((float) getDelta() / 1000.0f));

	scene->update();
}

void Test::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	camera->useView();

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	scene->render();
}

void Test::destroy() {
	delete scene;
	delete camera;
}

#endif /* UTILS_BASEENGINETEST3D_H_ */
