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
#include "core/audio/SoundSystem.h"
#include "experimental/Billboard.h"
#include "core/particles/ParticleEmitter.h"
#include "core/particles/ParticleEffect.h"
#include "core/render/Scene.h"
#include "core/physics/PhysicsScene.h"
#include "core/input/Controller.h"
#include "core/ResourceLoader.h"
#include "core/input/InputBindings.h"

class Test : public BaseEngine, public InputBindingsListener {
private:
	SoundSystem* soundSystem;
	Camera3D* camera;
	PhysicsObject3D* object;
	ParticleEmitter* particleEmitter;
	ParticleSystem* particleSystem;
	PhysicsScene3D* physicsScene;
	Scene* scene;
	Controller* controller;

	InputBindings* bindings;
	InputBindingAxis* axis0;
	InputBindingAxis* axis1;

	InputBindingAxis* axis2;
	InputBindingAxis* axis3;

	//Model* object2;
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
	virtual void onKeyReleased(int key) override {}
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override {}
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {
		camera->getRelRotation() += Vector3f(-dy, dx, 0) * getDelta() * 0.01f;
		camera->getRelRotation().setX(MathsUtils::clamp(camera->getRotation().getX(), -89, 89));
		camera->update();
	}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) override {}
	virtual void onControllerButtonPressed(Controller* controller, int index) override {}
	virtual void onControllerButtonReleased(Controller* controller, int index) override {}

	virtual void onButtonPressed(InputBindingButton* button) override {

	}

	virtual void onButtonReleased(InputBindingButton* button) override {

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
	//object = Model::loadModel("C:/UnnamedEngine/Models/earth2/", "earth1.obj");
	//object->setScale(0.2f, 0.2f, 0.2f);
	//object->setPosition(0, 0, 0);
	//object->update();

	physicsScene = new PhysicsScene3D();
	scene = new Scene();

	object = new PhysicsObject3D(ResourceLoader::sLoadModel("C:/UnnamedEngine/Models/", "teapot.obj"), Renderer::getRenderShader("Material"));

	scene->add(object);
	physicsScene->add(object);

//	object2 = Model::loadModel("C:/UnnamedEngine/Models/earth2/", "earth2.obj");
//	object2->setPosition(0, 0, 0);
//	object2->update();

	camera = new Camera3D(Matrix4f().initPerspective(110, ((float) getSettings().windowWidth) / ((float) getSettings().windowHeight), 0.1f, 100));
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox1/", "skybox0.png", "skybox1.png", "skybox2.png", "skybox3.png", "skybox4.png", "skybox5.png", 100.0f));
	camera->setFlying(true);
	//camera->setPosition(0.0f, 0.0f, -500.0f);
	camera->update();

	Billboard* billboard = new Billboard(1.0f, 1.0f);
	billboard->setPosition(0, 1.0f, 0);

	scene->add(billboard);

	particleEmitter = new SphericalParticleEmitter(1.0f);
	particleEmitter->particleSpawnRate = 120;
	particleEmitter->particleLifeSpan = 2.0f;
	particleEmitter->particleColour = Colour::WHITE;
	particleEmitter->particleSize = 2.0f;

	particleSystem = new ParticleSystem(particleEmitter, 5000);
	particleSystem->acceleration = Vector3f(0.0f, 5.0f, 0.0f);
	particleSystem->effect = new ParticleEffectColourChange(Colour::WHITE, Colour(0.3f, 0.3f, 0.3f, 0.6f));

	//particleSystem->textureAtlas = new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/ParticleAtlas.png"), 8, 8, 64);
	//particleSystem->textureAtlas = new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/ParticleAtlas2.png"), 1, 1, 1);
	//particleSystem->textureAtlas = new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/firetexture.png"), 8, 8, 32);
	particleSystem->textureAtlas = new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/smoke.png"), 7, 7, 46);

	controller = new Controller(GLFW_JOYSTICK_2);
	getWindow()->getInputManager()->addController(controller);

	soundSystem = new SoundSystem();
	soundSystem->createListener(camera);
	soundSystem->addSoundEffect("Sound1", ResourceLoader::sLoadAudio("C:/UnnamedEngine/Sound.wav"), particleEmitter);
	soundSystem->addSoundEffect("Sound2", ResourceLoader::sLoadAudio("C:/UnnamedEngine/Sound2.wav"), object);
	soundSystem->addMusic("Sound3", ResourceLoader::sLoadAudio("C:/UnnamedEngine/Sound.ogg"));
	soundSystem->play("Sound1");

	bindings = new InputBindings();
	bindings->addListener(this);
	axis0 = bindings->createAxisBinding("Axis0");
	axis0->assignKeys(GLFW_KEY_W, GLFW_KEY_S);
	axis0->assignControllerAxis(controller, 1);
	axis1 = bindings->createAxisBinding("Axis1");
	axis1->assignKeys(GLFW_KEY_A, GLFW_KEY_D);
	axis1->assignControllerAxis(controller, 0);

	axis2 = bindings->createAxisBinding("Axis2");
	axis2->assignControllerAxis(controller, 4);
	axis3 = bindings->createAxisBinding("Axis3");
	axis3->assignControllerAxis(controller, 3);

	Renderer::addCamera(camera);
}

void Test::update() {
	camera->moveForward(0.004f * axis0->getValue() * getDelta());
	camera->moveLeft(0.004f * axis1->getValue() * getDelta());

	camera->getRelRotation() += Vector3f(axis2->getValue(), -axis3->getValue(), 0) * getDelta() * 0.1f;
	camera->getRelRotation().setX(MathsUtils::clamp(camera->getRotation().getX(), -89, 89));

	camera->update();

	if (getWindow()->isKeyPressed(GLFW_KEY_UP))
		particleEmitter->getRelPosition() += Vector3f(0.0f, 0.0f, -0.008f * getDelta());
	else if (getWindow()->isKeyPressed(GLFW_KEY_DOWN))
		particleEmitter->getRelPosition() += Vector3f(0.0f, 0.0f, 0.008f * getDelta());
	if (getWindow()->isKeyPressed(GLFW_KEY_LEFT))
		particleEmitter->getRelPosition() += Vector3f(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (getWindow()->isKeyPressed(GLFW_KEY_RIGHT))
		particleEmitter->getRelPosition() += Vector3f(0.008f * getDelta(), 0.0f, 0.0f);

	particleSystem->update(((float) getDelta() / 1000.0f), camera->getPosition());

	soundSystem->update();

	physicsScene->update(((float) getDelta() / 1000.0f));
	scene->update();
}

void Test::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	camera->useView();

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	scene->render();

	particleSystem->render();

	//object2->render();
}

void Test::destroy() {
	delete scene;
	delete physicsScene;
	delete camera;
	delete particleSystem;
	delete bindings;
}

#endif /* UTILS_BASEENGINETEST3D_H_ */
