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

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"

#include "../core/terrain/CDLODTerrain.h"
#include "../core/terrain/HeightMapGenerator.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

class Test : public BaseTest3D {
private:
	RenderScene3D* scene;
	CDLODTerrain* terrain;
	CDLODHeightMap* heightMap;

	GameObject3D* model1;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {
	getSettings().videoVSync = false;
	getSettings().videoMaxFPS = 0;
	//getSettings().windowFullscreen = true;
	//getSettings().videoResolution = VideoResolution::RES_1920x1080;
}

void Test::onCreated() {
	camera->setProjectionMatrix(Matrix4f().initPerspective(80.0f, getSettings().windowAspectRatio, 0.01f, 1000.0f));
	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg"));
	camera->setFlying(true);

	camera->setMovementSpeed(5.0f);

	//terrain = new CDLODTerrain(resourceLoader.getAbsPathTextures() + "heightmap.jpg");
	HeightMapGenerator generator;
	heightMap = new CDLODHeightMap(generator.generate(256, 256), 1, 256, 256, GL_RED, GL_RED);
	//heightMap = new CDLODHeightMap("F:/Storage/Users/Joel/Desktop/test.jpg");
	terrain = new CDLODTerrain(heightMap, 4, 16.0f);
	terrain->getMaterial()->shininess = 1.0f;
	terrain->update();

	scene = new RenderScene3D();
	scene->add(terrain);

	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(1.0f, -1.0f, 0.0001f);
	light0->update();
	scene->addLight(light0);

	Light* light1 = (new Light(Light::TYPE_POINT, Vector3f(1.0f, terrain->getHeight(0, 0) + 0.5f, 0.0f), false))->setDiffuseColour(Colour::RED);
	light1->update();
	scene->addLight(light1);

	model1 = new GameObject3D(resourceLoader.loadModel("teapot.model"), Renderer::SHADER_LIGHTING);
	model1->setPosition(0.0f, terrain->getHeight(0.0f, 4.0f) + 1.0f, 6.0f);
	model1->update();

	scene->add(model1);

	//stbi_write_bmp("D:/Storage/Users/Joel/Desktop/heightmapgen.bmp", 512, 512, 1, generator.generate(512, 512));
	//std::cout << glfwGetJoystickName(0) << std::endl;

	//scene->enableDeferred();
	//scene->enableGammaCorrection();

	//scene->enableWireframe();
}

void Test::onUpdate() {
	if (Keyboard::isPressed(GLFW_KEY_LEFT_SHIFT)) {
		camera->setMovementSpeed(50.0f);
	} else {
		camera->setMovementSpeed(5.0f);
	}
	Vector3f pos = camera->getPosition();
	camera->setY(heightMap->getHeight(pos.getX(), pos.getZ()) + 1.5f);

	//std::cout << heightMap->getSteepness(pos.getX(), pos.getZ(), camera->getTransform()->getRotation().getForward()) << std::endl;

	//terrain->getTransform()->rotate(terrain->getTransform()->getRotation().getUp(), 0.1f * getDelta());
	//terrain->setScale(10.0f, 10.0f, 10.0f);
	//terrain->update();
}

void Test::onRender() {
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	scene->render();
	glDisable(GL_CULL_FACE);
}

void Test::onDestroy() {
	//delete terrain;
	delete scene;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
