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

#include "Player.h"
#include "AsteroidsGame.h"

/*****************************************************************************
 * The Player class
 *****************************************************************************/

Player::Player(AsteroidsGame* game) : game(game) {
	//Setup the camera
	camera = new DebugCamera(Matrix4f().initPerspective(110.0f, game->getSettings().windowAspectRatio, 0.1f, 100.0f));
	camera->setSkyBox(new SkyBox(game->getResourceLoader().getPath() + "skybox/", "skyboxBK.png", "skyboxFT.png", "skyboxLF.png", "skyboxRT.png", "skyboxUP.png", "skyboxDN.png", 100.0f));
	camera->setFlying(true);

	//Setup the lasers renderer
	lasersRenderer = new LasersRenderer(game->getResourceLoader(), 20);

	for (unsigned int i = 0; i < 20; i++) {
		GameObject3D* laser = new GameObject3D();

		lasers.push_back(new PhysicsObject3D(laser));
		lasersRenderer->addLaser(laser);
	}

	lasersRenderer->update();

	game->getWindow()->getInputManager()->addListener(this);
	nextLaser = 0;
}

Player::~Player() {
	delete camera;
}

void Player::update() {
	camera->update(game->getDeltaSeconds());
	lasersRenderer->update();

	for (unsigned int i = 0; i < lasers.size(); i++)
		lasers[i]->updatePhysics(game->getDeltaSeconds());
}

void Player::useView() {
	camera->useView();

	lasersRenderer->render();
}

void Player::onMousePressed(int button) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		lasers[nextLaser]->setPosition(camera->getPosition());
		lasers[nextLaser]->setVelocity(camera->getFront() * 10.0f);
		lasers[nextLaser]->setRotation(camera->getRotation() * Vector3f(0.0f, -1.0f, 0.0f));
		//std::cout << lasers[nextLaser]->getRotation().toString() << std::endl;
		nextLaser++;
		if (nextLaser >= lasers.size())
			nextLaser = 0;
	}
}
