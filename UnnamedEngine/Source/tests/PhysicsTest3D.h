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

#include "BaseTest3D.h"

#include <iostream>

#include "../core/render/Renderer.h"

class Test : public BaseTest3D {
private:
	PhysicsObject3D* objectA;
	PhysicsObject3D* objectB;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;

	void onCollision(unsigned int indexA, unsigned int indexB) {
		std::cout << "COLLISION" << std::endl;
	}
};

void Test::onInitialise() {

}

void Test::onCreated() {
//	Mesh* modelA = resourceLoader.loadModel("sphere.obj");
	Mesh* modelA = resourceLoader.loadModel("plane.obj");
	Mesh* modelB = resourceLoader.loadModel("sphere.obj");

	objectA = new PhysicsObject3D(modelA, Renderer::SHADER_MATERIAL);
//	objectA->setCollider(new SphereCollider(objectA, modelA->getBoundingSphereRadius()));
	objectA->setCollider(new PlaneCollider3D(objectA, Vector3f(1.0f, 0.0f, 0.0f)));
	objectA->setRotation(0.0f, 0.0f, 45.0f);
	objectB = new PhysicsObject3D(modelB, Renderer::SHADER_MATERIAL);
	objectB->setCollider(new SphereCollider(objectB, modelB->getBoundingSphereRadius()));
	objectB->setPosition(3.0f, 0.0f, 0.0f);
	objectB->setVelocity(-0.2f, 0.0f, 0.0f);

	renderScene->add(objectA);
	physicsScene->add(objectA);
	renderScene->add(objectB);
	physicsScene->add(objectB);

	renderScene->disableLighting();

	physicsScene->setCollisionCallback(std::bind(onCollision, this, 0, 0));

	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg"));
	camera->setFlying(true);
}

void Test::onUpdate() {
	if (Keyboard::isPressed(GLFW_KEY_UP))
		objectA->setAcceleration(0.0f, 0.0f, -1.0f);
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		objectA->setAcceleration(0.0f, 0.0f, 1.0f);
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		objectA->setAcceleration(-1.0f, 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		objectA->setAcceleration(1.0f, 0.0f, 0.0f);
}

void Test::onRender() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_DEPTH_TEST);
//
//	glEnable(GL_MULTISAMPLE_ARB);
//	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
}

void Test::onDestroy() {

}

#endif /* UTILS_BASEENGINETEST3D_H_ */
