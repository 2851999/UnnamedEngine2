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
	setMovementSpeed(RandomUtils::randomFloat(4.0f, 8.0f));
}

Enemy::~Enemy() {

}

void Enemy::reset() {
	//Reset this ship
	Ship::reset();

	//Assign a random position
	setPosition(RandomUtils::randomFloat(-10, 10), RandomUtils::randomFloat(-10, 10), RandomUtils::randomFloat(-10, 10));
}

void Enemy::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	Ship::update(deltaSeconds, closestGroup);

	if (isAlive()) {
		//AI stuff
		Vector3f directionToPlayer = (player->getPosition() - getPosition()).normalise();

		thrust(directionToPlayer);

		float laserSpeed = 20.0f;
		float currentDistance = (player->getPosition() - getPosition()).length();
		float timeToReach = currentDistance / laserSpeed;

		Vector3f newPosition = player->getPosition() + ((player->getVelocity() - getVelocity()) * timeToReach);

		fireLasers((newPosition - getPosition()).normalise());

		//Since rotation is a bit hard without quaternions at the moment, enemies are SPHERES OF DEATH
	}
}
