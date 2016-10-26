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
	PhysicsObject3D* sphereA;
	PhysicsObject3D* sphereB;
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
	std::vector<Mesh*> meshes1 = resourceLoader.loadModel("sphere.obj");
	std::vector<Mesh*> meshes2 = resourceLoader.loadModel("sphere.obj");

	sphereA = new PhysicsObject3D(meshes1, "Material");
	sphereA->setCollider(new SphereCollider(sphereA, 1.0f));
	sphereB = new PhysicsObject3D(meshes2, "Material");
	sphereB->setCollider(new SphereCollider(sphereB, 1.0f));
	sphereB->setPosition(3.0f, 0.0f, 0.0f);
	sphereB->setVelocity(-0.2f, 0.0f, 0.0f);

	renderScene->add(sphereA);
	physicsScene->add(sphereA);
	renderScene->add(sphereB);
	physicsScene->add(sphereB);

	physicsScene->setCollisionCallback(std::bind(onCollision, this, 0, 0));

	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg", 100.0f));
	camera->setFlying(true);
}

void Test::onUpdate() {
	if (Keyboard::isPressed(GLFW_KEY_UP))
		sphereA->setAcceleration(0.0f, 0.0f, -1.0f);
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		sphereA->setAcceleration(0.0f, 0.0f, 1.0f);
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		sphereA->setAcceleration(-1.0f, 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		sphereA->setAcceleration(1.0f, 0.0f, 0.0f);
}

void Test::onRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
}

void Test::onDestroy() {

}

#endif /* UTILS_BASEENGINETEST3D_H_ */
