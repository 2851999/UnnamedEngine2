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

class Test : public BaseTest3D {
private:
	ParticleEmitter* particleEmitter;
	ParticleSystem* particleSystem;

	GameObject3D* teapot;
	float c = 0;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {

}
//MATERIAL WORKS, SO MUST BE BONE TRANSFORMATIONS
void Test::onCreated() {
	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg", 100.0f));
	camera->setFlying(true);

	//GameObject3D* teapot = new GameObject3D(resourceLoader.loadModel("teapot.obj"), "Lighting");
	teapot = new GameObject3D(resourceLoader.loadModel("bob/", "bob_lamp_update.md5mesh"), "Lighting");
	GameObject3D* plane = new GameObject3D(resourceLoader.loadModel("plane/", "plane.obj"), "Lighting");
	teapot->setPosition(0.0f, 0.8f, 0.0f);
	teapot->setRotation(90.0f, 180.0f, 0.0f);
	teapot->update();
	plane->update();
	renderScene->add(teapot);
	renderScene->add(plane);

	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, 0.0f, 0.2f);
	light0->update();
	renderScene->addLight(light0);

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
}

void Test::onUpdate() {
	particleSystem->update(getDeltaSeconds(), camera->getPosition());

	if (Keyboard::isPressed(GLFW_KEY_UP))
		particleEmitter->getTransform()->translate(Vector3f(0.0f, 0.0f, -0.008f * getDelta()));
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		particleEmitter->getTransform()->translate(Vector3f(0.0f, 0.0f, 0.008f * getDelta()));
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		particleEmitter->getTransform()->translate(Vector3f(-0.008f * getDelta(), 0.0f, 0.0f));
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		particleEmitter->getTransform()->translate(Vector3f(0.008f * getDelta(), 0.0f, 0.0f));

	c += getDeltaSeconds();
	teapot->getMesh()->boneTransform(c);
}

void Test::onRender() {
	GLUtils::setupSimple3DView(true);

	particleSystem->render();
}

void Test::onDestroy() {
	delete particleSystem;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
