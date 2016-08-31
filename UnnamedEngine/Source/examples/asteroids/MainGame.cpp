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

#include <random>
#include <time.h>

#include "Asteroids.h"
#include "MainGame.h"

#include "../../core/Model.h"

MainGame::MainGame(Asteroids* asteroids, float windowWidth, float windowHeight) : asteroids(asteroids) {
	camera3D = new Camera3D(Matrix4f().initPerspective(80.0f, windowWidth / windowHeight, 1.0f, 100.0f));
	//camera3D->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox1/", "skybox0.png", "skybox1.png", "skybox2.png", "skybox3.png", "skybox4.png", "skybox5.png", 100));
	camera3D->setSkyBox(new SkyBox("C:/UnnamedEngine/skybox4/", "skyboxBK.png", "skyboxFT.png", "skyboxLF.png", "skyboxRT.png", "skyboxUP.png", "skyboxDN.png", 100));
	camera3D->setFlying(true);
	camera3D->update();

	player = new PhysicsObject3D(camera3D);
	player->setCollider(new SphereCollider(player, 1.0f));
	player->setMass(10);

	scene = new Scene();

	scene->addLight((new Light(Light::TYPE_DIRECTIONAL))->setDirection(0.0f, -1.0f, 0.5f)
												  ->setDiffuseColour(Colour::WHITE)
												  ->setSpecularColour(Colour::WHITE));

	physicsScene = new PhysicsScene3D();
	physicsScene->add(player);

	//The asteroid mesh
	//std::vector<Mesh*> asteroidMesh = Model::loadModel("C:/UnnamedEngine/examples/asteroids/", "asteroid.obj");
	std::vector<Mesh*> asteroidMesh = Model::loadModel("H:/Storage/Users/Joel/Downloads/asteroid_model1/asteroid_model/", "asteroid_model.obj");

	srand(time(NULL));

	//Add asteroids
	for (unsigned int i = 0; i < 100; i++) {
		//Create the asteroid
		PhysicsObject3D* asteroid = new PhysicsObject3D(asteroidMesh, Renderer::getRenderShader("Lighting"));

		asteroid->setPosition(RandomUtils::randomFloat(-40, 40), RandomUtils::randomFloat(-40, 40), RandomUtils::randomFloat(-40, 40));
		float s = RandomUtils::randomFloat(0.1f, 1);
		asteroid->setScale(s, s, s);
		asteroid->setAngularVelocity(RandomUtils::randomFloat(0, 10), RandomUtils::randomFloat(0, 10), RandomUtils::randomFloat(0, 10));
		asteroid->setCollider(new SphereCollider(asteroid, 3.0f * s));
		asteroid->setMass(10 * s);

		scene->add(asteroid);
		physicsScene->add(asteroid);
	}
}

MainGame::~MainGame() {
	delete camera3D;
}

void MainGame::startGame() {
	Renderer::addCamera(camera3D);
	asteroids->getWindow()->getInputManager()->addListener(this);
}

void MainGame::update() {
	camera3D->update();
	scene->update();
	physicsScene->update((float) asteroids->getDelta() / 1000.0f);

	if (asteroids->getWindow()->isKeyPressed(GLFW_KEY_W)) {
		float amount = 0.5f * asteroids->getDelta();
		player->setVelocity(camera3D->getFront() * amount);
	} else if (asteroids->getWindow()->isKeyPressed(GLFW_KEY_S)) {
		float amount = 0.5f * asteroids->getDelta();
		player->setVelocity(camera3D->getFront() * -amount);
	} else
		player->setVelocity(0, 0, 0);
}

void MainGame::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera3D->useView();

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	scene->render();
	glDisable(GL_CULL_FACE);
}

void MainGame::onMouseMoved(double x, double y, double dx, double dy) {
	player->getRelRotation() += Vector3f(-dy, dx, 0) * asteroids->getDelta() * 0.01f;
	player->getRelRotation().setX(MathsUtils::clamp(player->getRotation().getX(), -89, 89));
	player->update();
}
