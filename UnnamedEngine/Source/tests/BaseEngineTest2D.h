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

#ifndef BASEENGINETEST2D_H_
#define BASEENGINETEST2D_H_

#include "../core/BaseEngine.h"
#include "../core/render/Camera.h"
#include "../core/render/Renderer.h"
#include "../core/gui/Font.h"
#include "../core/Sprite.h"
#include "../core/render/Tilemap.h"
#include "../utils/GLUtils.h"
#include "../core/ml/ML.h"
#include "../core/audio/SoundSystem.h"

class Test : public BaseEngine {
private:
	Camera2D* camera;
	GameObject2D* object;
	Font* font;
	Sprite* sprite;
	Tilemap* tilemap;
	SoundSystem* soundSystem;
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

	virtual void onMousePressed(int button) override;
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}
};

void Test::initialise() {
	getSettings().windowTitle = "Unnamed Engine " + Engine::Version;
	getSettings().videoVSync = 1;
//	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = 16;
	getSettings().videoMaxAnisotropicSamples = 16;
}

void Test::created() {
	TextureParameters::DEFAULT_CLAMP = GL_CLAMP_TO_EDGE;
	TextureParameters::DEFAULT_SHOULD_CLAMP = true;
	TextureParameters::DEFAULT_FILTER = GL_LINEAR;

	Texture* texture = Texture::loadTexture("C:/UnnamedEngine/textures/skybox1/front.png", TextureParameters().setFilter(GL_LINEAR_MIPMAP_LINEAR));
	object = new GameObject2D(new Mesh(MeshBuilder::createQuad(200, 200, texture)), Renderer::SHADER_MATERIAL);
	object->setSize(200, 200);
	object->setPosition(getSettings().windowWidth / 2 - 100, getSettings().windowHeight / 2 - 100);
	object->getMaterial()->diffuseTexture = texture;
	object->getMaterial()->diffuseColour = Colour::WHITE;
	object->update();

	sprite = new Sprite(texture, 100, 100);
	sprite->setPosition(400, 100);
	//Animation2D* animation = new TextureAnimation2D(sprite, new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/textures/ParticleAtlas3.png"), 8, 8, 64), 0.05f);
	sprite->addAnimation("Smoke", new TextureAnimation2D(new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/textures/smoke.png"), 7, 7, 46), 0.05f, true));
	//sprite->addAnimation("Smoke", new TextureAnimation2D(new TextureAtlas(Texture::loadTexture("C:/UnnamedEngine/textures/ParticleAtlas2.png"), 1, 1, 1), 0.05f, true));
	sprite->startAnimation("Smoke");

	camera = new Camera2D(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1);
	camera->update();

	Renderer::addCamera(camera);

	tilemap = Tilemap::loadTilemap("C:/UnnamedEngine/maps/", "Map3.tmx", true);

	soundSystem = new SoundSystem();
	soundSystem->createListener();
	soundSystem->addMusic("Music", AudioLoader::loadFile("C:/UnnamedEngine/maps/music.ogg"));
	soundSystem->getSource("Music")->setLoop(true);
	soundSystem->play("Music");
}

void Test::update() {
	if (Keyboard::isPressed(GLFW_KEY_W))
		camera->getTransform()->translate(0.0f, -256.0f * getDeltaSeconds());
	if (Keyboard::isPressed(GLFW_KEY_S))
		camera->getTransform()->translate(0.0f, 256.0f * getDeltaSeconds());
	if (Keyboard::isPressed(GLFW_KEY_A))
		camera->getTransform()->translate(-256.0f * getDeltaSeconds(), 0.0f);
	if (Keyboard::isPressed(GLFW_KEY_D))
		camera->getTransform()->translate(256.0f * getDeltaSeconds(), 0.0f);
	camera->update();

	object->getTransform()->rotate(60.0f * getDeltaSeconds());
	object->update();

	sprite->update(getDeltaSeconds());
}

void Test::onMousePressed(int button) {
	Vector3f cameraPos = camera->getPosition();
	Vector2d mousePos = Mouse::getPosition();
	float worldX = (float) mousePos.getX() + cameraPos.getX();
	float worldY = (float) mousePos.getY() + cameraPos.getY();
//	std::cout << tilemap->getLayers()[0]->getTileID(worldX, worldY) << std::endl;
	tilemap->getLayers()[0]->setTileID(worldX, worldY, 1);
}

void Test::render() {
	utils_gl::setupSimple2DView();

	object->render();

	sprite->render();

	tilemap->render();
}

void Test::destroy() {
	delete soundSystem;
	delete object;
	delete camera;
}

#endif /* BASEENGINETEST2D_H_ */
