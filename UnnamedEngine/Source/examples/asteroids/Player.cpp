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

	//Setup the lasers
	lasers = new Lasers(game->getResourceLoader());

	game->getWindow()->getInputManager()->addListener(this);
}

Player::~Player() {
	//Destroy created resources
	delete camera;
	delete lasers;
}

void Player::update(AsteroidGroup& closestAsteroids) {
	//Update the camera
	camera->update(game->getDeltaSeconds());
	//Update the lasers
	lasers->update(game->getDeltaSeconds(), closestAsteroids);
}

void Player::render() {
	//Use then camera's view
	camera->useView();

	//Render the lasers
	lasers->render();
}

void Player::onMousePressed(int button) {
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		//Fire the laser
		lasers->fire(this);
}
