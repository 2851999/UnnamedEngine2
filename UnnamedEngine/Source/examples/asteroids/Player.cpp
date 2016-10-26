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
#include "Enemy.h"

/*****************************************************************************
 * The Player class
 *****************************************************************************/

Player::Player(AsteroidsGame* game, std::vector<Enemy*>& enemies) : Ship(game), game(game), enemies(enemies) {
	//Setup input
	InputBindings* inputBindings = game->getInputBindings();
	axisForward = inputBindings->getAxisBinding("Forward");
	lookX = inputBindings->getAxisBinding("LookX");
	lookY = inputBindings->getAxisBinding("LookY");
	buttonShoot = inputBindings->getButtonBinding("Shoot");

	score = 0;
	currentDelta = 0.0f;

	//Setup the camera
	camera = new Camera3D(110.0f, game->getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->setSkyBox(new SkyBox(game->getResourceLoader().getPath() + "skybox/", "skyboxBK.png", "skyboxFT.png", "skyboxLF.png", "skyboxRT.png", "skyboxUP.png", "skyboxDN.png", 100.0f));
	camera->setFlying(true);
	camera->setParent(this);

	game->getWindow()->getInputManager()->addListener(this);

	setHealth(5);
	setShield(2);

	setMass(100.0f);
	setRestitution(1.0f);
}

Player::~Player() {
	//Destroy created resources
	delete camera;
}

void Player::reset() {
	Ship::reset();

	score = 0;
	setHealth(5);
	setShield(2);
	setShieldMax(2);
	setShieldRegenRate(4.0f);
	setMaximumSpeed(10.0f);
	getLasers()->setCooldown(2.0f);
}

void Player::update(float deltaSeconds, AsteroidGroup& closestAsteroids) {
	if (isAlive()) {
		//Get the current delta
		currentDelta = deltaSeconds;

		//Orientate the camera
		getRelRotation() += Vector3f(lookY->getValue(), -lookX->getValue(), 0) * 80.0f * currentDelta;
		getRelRotation().setX(MathsUtils::clamp(camera->getRotation().getX(), -89.0, 89.0));

		//Move the player
		if (axisForward->getValue() != 0)
			thrust(camera->getFront() * axisForward->getValue());
		else
			setAcceleration(getVelocity() * -12.0f * currentDelta);

		camera->update();

		//Check whether the player is shooting
		if (buttonShoot->isPressed())
			//FIRE THE LASERS!!!
			fireLasers(camera->getFront());
	}
	//Update the lasers
	Ship::update(currentDelta, closestAsteroids);
}

void Player::render() {
	//Use then camera's view
	camera->useView();

	//Render the lasers
	Ship::render();
}

/* Called when an asteroid has been destroyed by this ship's lasers */
void Player::onAsteroidDestroyed(GameObject3D* asteroid) {
	addPoints(100 * asteroid->getScale().max());
}

/* Method used to check whether a laser has collided with anything */
bool Player::checkCollision(PhysicsObject3D* laser) {
	//Go through the enemies
	for (unsigned int i = 0; i < enemies.size(); i++) {
		if (enemies[i]->isAlive()) {
			//Get the distance between the current laser object and the current enemy object
			float distance = (laser->getPosition() - enemies[i]->getPosition()).length();

			//Check for an intersection with the asteroid
			if (distance < 1.0f) {
				//Remove health
				enemies[i]->removeHealth(1);
				if (! enemies[i]->isAlive()) {
					//Create an explosion
					getLasers()->explode(enemies[i]->getPosition(), 2.0f);
					//Add to the score
					addPoints(2000);
				}
				return true;
			}
		}
	}
	return false;
}

void Player::onMouseMoved(double x, double y, double dx, double dy) {
	if (game->getCurrentState() == AsteroidsGame::GAME_PLAYING && ! game->getMainGame()->showingUpgrades()) {
		//Orientate the camera
		getRelRotation() += Vector3f(-dy, dx, 0) * 10.0f * currentDelta;
		getRelRotation().setX(MathsUtils::clamp(camera->getRotation().getX(), -89.0, 89.0));
	}
}

void Player::removeScore(unsigned int points) {
	if (points > score)
		score = 0;
	else
		score -= points;
}
