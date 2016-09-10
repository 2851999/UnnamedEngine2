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

#ifndef EXAMPLES_ASTEROIDS_ENEMY_H_
#define EXAMPLES_ASTEROIDS_ENEMY_H_

#include "Ship.h"
#include "Player.h"

/*****************************************************************************
 * The Enemy class handles enemy movement and shooting
 *****************************************************************************/

class Enemy : public Ship {
private:
	/* The player instance to target */
	Player* player;
public:
	/* The constructor */
	Enemy(AsteroidsGame* game, Player* player);

	/* The destructor */
	virtual ~Enemy();

	/* Method called to reset this ship/enemy */
	void reset() override;

	/* Method used to update this enemy */
	void update(float deltaSeconds, AsteroidGroup& closestGroup) override;

	/* Method used to check whether a laser has collided with anything */
	virtual bool checkCollision(PhysicsObject3D* laser) override;
};

#endif /* EXAMPLES_ASTEROIDS_ENEMY_H_ */
