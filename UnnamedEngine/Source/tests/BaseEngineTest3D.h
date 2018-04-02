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

#include "../core/particles/ParticleSystem.h"
#include "../core/particles/ParticleEmitter.h"
#include "../core/particles/ParticleEffect.h"
#include "../core/render/Renderer.h"
#include "../utils/GLUtils.h"

#include "../experimental/Billboard.h"
//#include "../experimental/terrain/HeightMapTerrain.h"

class Test : public BaseTest3D {
private:
	ParticleEmitter* particleEmitter;
	ParticleSystem* particleSystem;

	GameObject3D* plane;
	GameObject3D* model1;
	GameObject3D* model2;
	GameObject3D* model3;
//	GameObject3D* box;

	Font* font;

//	HeightMapTerrain* terrain;

//	bool test = false;
//	HeightMapTerrain* terrain2;
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
}

void Test::onCreated() {
	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg"));
	camera->setFlying(true);

//	MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels() + "plane/", "plane.obj");
//	MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels() + "bob/", "bob_lamp_update.md5mesh");
//	MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "teapot.obj");
//	MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "gingerbreadman.dae");

	plane = new GameObject3D(resourceLoader.loadModel("plane/", "plane2.model"), Renderer::SHADER_LIGHTING);
	//plane = new GameObject3D(resourceLoader.loadModel("crytek-sponza/", "sponza.obj"), Renderer::SHADER_LIGHTING);
	//plane->setScale(0.15f, 0.15f, 0.15f);
	plane->update();

	model1 = new GameObject3D(resourceLoader.loadModel("bob/", "bob_lamp_update.model"), Renderer::SHADER_LIGHTING);
	model1->setPosition(-2.0f, 0.8f, 0.0f);
	model1->update();

	model1->getMesh()->getSkeleton()->startAnimation("");
	//model1->getMesh()->getSkeleton()->stopAnimation();

	model2 = new GameObject3D(resourceLoader.loadModel("teapot.model"), Renderer::SHADER_LIGHTING);
	model2->setPosition(0.0f, 0.8f, 2.0f);
	model2->update();

//	model2->getMesh()->getSkeleton()->startAnimation("");
	//model2->getMesh()->getSkeleton()->stopAnimation();

	model3 = new GameObject3D(resourceLoader.loadModel("deformablesphere.dae"), Renderer::SHADER_LIGHTING);
	model3->setPosition(2.0f, 0.8f, 0.0f);
	model3->update();

	model3->getMesh()->getSkeleton()->startAnimation("");
	//model3->getMesh()->getSkeleton()->stopAnimation();

	renderScene->add(plane);
	renderScene->add(model1);
	renderScene->add(model2);
	renderScene->add(model3);

	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), true))->setDirection(0, -1.0f, 0.0001f); //->setDiffuseColour(Colour(200.0f, 200.0f, 200.0f));
	Light* light1 = (new Light(Light::TYPE_POINT, Vector3f(0.0f, 1.0f, 0.0f), false))->setDiffuseColour(Colour::RED);
	Light* light2 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), true))->setDirection(0.5f, -1.0f, 0.0001f);
	//plane->getMesh()->getMaterial(1)->diffuseTexture = light0->getDepthBuffer()->getFramebufferTexture(0);
	light0->update();
	light1->update();
	light2->update();
	renderScene->addLight(light0);
	renderScene->addLight(light1);
	renderScene->addLight(light2);

	//renderScene->enableDeferred();

	particleEmitter = new SphericalParticleEmitter(1.0f);
	particleEmitter->particleSpawnRate = 120;
	particleEmitter->particleLifeSpan = 2.0f;
	particleEmitter->particleColour = Colour::WHITE;
	particleEmitter->particleSize = 3.0f;
	particleEmitter->setActive(true);

	particleSystem = new ParticleSystem(particleEmitter, 5000);
	particleSystem->acceleration = Vector3f(0.0f, 5.0f, 0.0f);
	particleSystem->effect = new ParticleEffectColourChange(Colour::WHITE, Colour(0.3f, 0.3f, 0.3f, 0.6f));
	particleSystem->textureAtlas = new TextureAtlas(resourceLoader.loadTexture("smoke.png"), 7, 7, 46);

	soundSystem->playAsMusic("Music", resourceLoader.loadAudio("Sound.ogg"));
	soundSystem->playAsSoundEffect("SoundEffect", resourceLoader.loadAudio("Sound.wav"), particleEmitter);

	font = new Font("resources/fonts/CONSOLA.TTF", 64, Colour::WHITE, true, TextureParameters().setShouldClamp(true).setFilter(GL_LINEAR));
	font->update("Hello World!", Vector3f(0.0f, 2.0f, 0.0f));

//	terrain = new HeightMapTerrain();
//	terrain->setup("D:/Storage/Users/Joel/Desktop/heightmap.jpg", 8);
//	terrain->setScale(Vector3f(0.1f, 0.1f, 0.1f));
//	terrain->update();

//	terrain2 = new HeightMapTerrain();
//	terrain2->setup("H:/Storage/Users/Joel/Desktop/heightmap.png", 8);
//	terrain2->setScale(Vector3f(0.1f, 0.1f, 0.1f));
//	terrain2->getMaterial()->diffuseColour = Colour::RED;
//	terrain2->update();

//	renderScene->add(terrain);

//	box = new GameObject3D(new Mesh(MeshBuilder::createCube(1.0f, 1.0f, 1.0f)), Renderer::SHADER_MATERIAL);
//	box->getMaterial()->diffuseColour = Colour::BLUE;
//	//box->getMesh()->setBoundingSphere(box->getMesh()->getData()->calculateBoundingSphere());
//	box->update();

	camera->setMovementSpeed(5.0f);
}

void Test::onUpdate() {
	particleSystem->update(getDeltaSeconds(), camera->getPosition());

	if (Keyboard::isPressed(GLFW_KEY_UP))
		particleEmitter->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		particleEmitter->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		particleEmitter->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		particleEmitter->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);

	model1->getMesh()->updateAnimation(getDeltaSeconds());
	//model2->getMesh()->getSkeleton()->update(getDeltaSeconds());
	model3->getMesh()->updateAnimation(getDeltaSeconds());

	model2->getTransform()->rotate(model2->getTransform()->getRotation().getUp(), 0.1f * getDelta());
	model2->update();
}

void Test::onRender() {
	particleSystem->render();
	font->render();
//	terrain->render();

//	box->render();

//	if (camera->getFrustum().AABBInFrustum(Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, 0.5f))) {
//		box->render();
//		if (test) {
//			std::cout << "In view" << std::endl;
//			test = false;
//		}
//	} else {
//		if (! test) {
//			std::cout << "Out of view" << std::endl;
//			test = true;
//		}
//	}

//	terrain2->render();
}

void Test::onDestroy() {
	delete particleSystem;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
