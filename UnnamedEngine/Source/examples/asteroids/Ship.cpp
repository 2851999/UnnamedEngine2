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

#include "Ship.h"

/*****************************************************************************
 * The Ship class
 *****************************************************************************/

Ship::Ship(AsteroidsGame* game) : PhysicsObject3D(NULL) {
	//Setup the lasers
	lasers = new Lasers(game, this);
	//Set the default health and shield
	health = 1;
	shield = 1;
	shieldMax = 1;
	shieldRegenRate = 10;
	//Setup the movement speed
	movementSpeed = 10.0f;
	maximumSpeed = 15.0f;
}

Ship::~Ship() {
	//Destroy created resources
	delete lasers;
}

void Ship::reset() {
	setPosition(0, 0, 0);
	setRotation(0, 0, 0);
	setVelocity(0, 0, 0);
	setAcceleration(0, 0, 0);
	setAngularVelocity(0, 0, 0);
	setAngularAcceleration(0, 0, 0);
	lasers->reset();
	health = 1;
	shieldTimer.restart();
}

void Ship::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	//Ensure this ship is alive
	if (isAlive()) {
		PhysicsObject3D::updatePhysics(deltaSeconds);
		//Get the speed
		float speed = getVelocity().length();
		//Clamp the speed
		if (speed > maximumSpeed)
			getRelVelocity() *= (maximumSpeed / speed);

		//Check whether the shield should be increased
		if (shieldMax > 0 && shield < shieldMax && shieldTimer.hasTimePassedSeconds(shieldRegenRate)) {
			shield ++;
			shieldTimer.restart();
		}
	}
	//Update the lasers
	lasers->update(deltaSeconds, closestGroup);
}

void Ship::render() {
	lasers->render();
}

void Ship::removeHealth(unsigned int amount) {
	//Need to remove shield instead of health if there is some
	if (shield > 0) {
		//Record the previous amount of shield
		unsigned int old = shield;
		//Remove the amount from the shield, ensuring it does not go below 0
		shield = MathsUtils::max(shield - amount, 0u);
		//Take away the amount that the shiled has taken of the damage so it can be taken
		//away from the health
		amount -= (old - shield);
	}
	//Check whether anything needs to be taken off of the health of the ship
	if (amount > 0) {
		//Take it away from the health, but ensure it doesn't go below 0
		if (health > amount)
			health -= amount;
		else
			health = 0;
	}
	//Since the player has just taken damage, the timer for the shield regeneration need
	//to be reset
	shieldTimer.restart();
}
