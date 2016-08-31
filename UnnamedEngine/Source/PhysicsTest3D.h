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
#include "core/render/Scene.h"
#include "core/physics/PhysicsScene.h"

class Test : public BaseEngine {
private:
	Camera3D* camera;
	PhysicsObject3D* sphereA;
	PhysicsObject3D* sphereB;
	PhysicsScene3D* physicsScene;
	Scene* scene;

public:
	virtual ~Test() {}

	virtual void initialise() override;
	virtual void created() override;
	virtual void update() override;
	virtual void render() override;
	virtual void destroy() override;

	void onCollision(unsigned int indexA, unsigned int indexB) {
		std::cout << "COLLISION" << std::endl;
	}

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

	physicsScene = new PhysicsScene3D();
	scene = new Scene();

	std::vector<Mesh*> meshes1 = Model::loadModel("C:/UnnamedEngine/Models/", "sphere.obj");
	std::vector<Mesh*> meshes2 = Model::loadModel("C:/UnnamedEngine/Models/", "sphere.obj");

	sphereA = new PhysicsObject3D(meshes1, Renderer::getRenderShader("Material"));
	sphereA->setCollider(new SphereCollider(sphereA, 1.0f));
	sphereB = new PhysicsObject3D(meshes2, Renderer::getRenderShader("Material"));
	sphereB->setCollider(new SphereCollider(sphereB, 1.0f));
	sphereB->setPosition(3.0f, 0.0f, 0.0f);
	sphereB->setVelocity(-0.2f, 0.0f, 0.0f);

	scene->add(sphereA);
	physicsScene->add(sphereA);
	scene->add(sphereB);
	physicsScene->add(sphereB);

	physicsScene->setCollisionCallback(std::bind(onCollision, this, 0, 0));

	camera = new Camera3D(Matrix4f().initPerspective(80, ((float) getSettings().windowWidth) / ((float) getSettings().windowHeight), 1, 100));
	//camera->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox1/", "skybox0.png", "skybox1.png", "skybox2.png", "skybox3.png", "skybox4.png", "skybox5.png", 100));
	camera->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox2/", "front.jpg", "back.jpg", "left.jpg", "right.jpg", "top.jpg", "bottom.jpg", 100));

	camera->setFlying(true);
	camera->update();

	Renderer::addCamera(camera);
}

void Test::update() {
	if (getWindow()->isKeyPressed(GLFW_KEY_W)) {
		camera->moveForward(0.004f * getDelta());
		camera->update();
	} else if (getWindow()->isKeyPressed(GLFW_KEY_S)) {
		camera->moveBackward(0.004f * getDelta());
		camera->update();
	}
	if (getWindow()->isKeyPressed(GLFW_KEY_A)) {
		camera->moveLeft(0.004f * getDelta());
		camera->update();
	} else if (getWindow()->isKeyPressed(GLFW_KEY_D)) {
		camera->moveRight(0.004f * getDelta());
		camera->update();
	}

	if (getWindow()->isKeyPressed(GLFW_KEY_UP))
		sphereA->setAcceleration(0.0f, 0.0f, -1.0f);
	else if (getWindow()->isKeyPressed(GLFW_KEY_DOWN))
		sphereA->setAcceleration(0.0f, 0.0f, 1.0f);
	if (getWindow()->isKeyPressed(GLFW_KEY_LEFT))
		sphereA->setAcceleration(-1.0f, 0.0f, 0.0f);
	else if (getWindow()->isKeyPressed(GLFW_KEY_RIGHT))
		sphereA->setAcceleration(1.0f, 0.0f, 0.0f);

	physicsScene->update(((float) getDelta() / 1000.0f));
	scene->update();
}

void Test::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	camera->useView();

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	scene->render();
}

void Test::destroy() {
	delete physicsScene;
	delete scene;
	delete camera;
}

#endif /* UTILS_BASEENGINETEST3D_H_ */
