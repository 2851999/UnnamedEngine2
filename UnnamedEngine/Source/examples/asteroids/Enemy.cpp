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

#include "Enemy.h"

/*****************************************************************************
 * The Enemy class
 *****************************************************************************/

Enemy::Enemy(AsteroidsGame* game, Player* player) : Ship(game), player(player) {

}

Enemy::~Enemy() {

}

void Enemy::reset() {
	//Reset this ship
	Ship::reset();

	setHealth(1);
	setShield(0);
	setShieldMax(0);
	setMovementSpeed(RandomUtils::randomFloat(4.0f, 8.0f));
	getLasers()->setCooldown(0.75f);

	//Assign a random position
	setPosition(RandomUtils::randomFloat(-100, 100), RandomUtils::randomFloat(-100, 100), RandomUtils::randomFloat(-100, 100));
}

void Enemy::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	if (isAlive()) {
		//AI stuff
//		Vector3f directionToPlayer = (player->getPosition() - getPosition()).normalise();
//
//		thrust(directionToPlayer);
//
//		float laserSpeed = 20.0f;
//		float currentDistance = (player->getPosition() - getPosition()).length();
//		float timeToReach = currentDistance / laserSpeed;
//
//		Vector3f newPosition = player->getPosition() + ((player->getVelocity() - getVelocity()) * timeToReach);
//
//		fireLasers((newPosition - getPosition()).normalise());

		//Since rotation is a bit hard without quaternions at the moment, enemies are SPHERES OF DEATH

		Vector3f relPos = player->getPosition() - getPosition();
		thrust(relPos.normalise());
		if (getLasers()->canFire()) {
			float laserSpeed = 20.0f + getVelocity().length();
			float currentDistance = relPos.length();
			float timeToReach = currentDistance / laserSpeed;
			Vector3f direction = ((player->getPosition() + player->getVelocity() * timeToReach) - (getPosition() + getVelocity() * timeToReach)).normalise();
			fireLasers(direction);
		}
	}

	//Update the ship afterwards as the lasers renderer should be updated after firing
	Ship::update(deltaSeconds, closestGroup);
}

/* Method used to check whether a laser has collided with anything */
bool Enemy::checkCollision(PhysicsObject3D* laser) {
	//Check the player
	if (player->isAlive()) {
		//Get the distance between the current laser object and the current enemy object
		float distance = (laser->getPosition() - player->getPosition()).length();

		//Check for an intersection with the player
		if (distance < 2.0f) {
			//Remove health
			player->removeHealth(1);
			if (! player->isAlive())
				//Create an explosion
				getLasers()->explode(player->getPosition(), 1.0f);
			return true;
		}
	}
	return false;
}
