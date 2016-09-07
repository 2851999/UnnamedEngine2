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
	//Setup input
	InputBindings* inputBindings = game->getInputBindings();
	axisForward = inputBindings->getAxisBinding("Forward");
	axisSideways = inputBindings->getAxisBinding("Sideways");
	buttonShoot = inputBindings->getButtonBinding("Shoot");

	currentDelta = 0.0f;

	//Setup the camera
	camera = new Camera3D(Matrix4f().initPerspective(110.0f, game->getSettings().windowAspectRatio, 0.1f, 100.0f));
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
	//Get the current delta
	currentDelta = game->getDeltaSeconds();
	//Update the camera
	camera->moveForward(axisForward->getValue() * 5.0f * currentDelta);
	camera->moveLeft(axisSideways->getValue() * 5.0f * currentDelta);

	camera->update();

	//Check whether the player is shooting
	if (buttonShoot->isPressed()) {
		//FIRE THE LASERS!!!
		lasers->fire(this);
	}
	//Update the lasers
	lasers->update(currentDelta, closestAsteroids);
}

void Player::render() {
	//Use then camera's view
	camera->useView();

	//Render the lasers
	lasers->render();
}

void Player::onMouseMoved(double x, double y, double dx, double dy) {
	//Orientate the camera
	camera->getRelRotation() += Vector3f(-dy * 10.0f, dx * 10.0f, 0) * currentDelta;
	camera->getRelRotation().setX(MathsUtils::clamp(camera->getRotation().getX(), -89.0, 89.0));
}
