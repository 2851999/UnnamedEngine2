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
	//Set the default health
	health = 1;
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
	}
	//Update the lasers
	lasers->update(deltaSeconds, closestGroup);
}

void Ship::render() {
	//Render the lasers
	lasers->render();
}

void Ship::removeHealth(unsigned int amount) {
	//Remove health if possible
	if (health > amount)
		health -= amount;
	else
		health = 0;
}
