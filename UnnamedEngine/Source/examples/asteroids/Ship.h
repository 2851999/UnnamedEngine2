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

#ifndef EXAMPLES_ASTEROIDS_SHIP_H_
#define EXAMPLES_ASTEROIDS_SHIP_H_

#include "Lasers.h"

/*****************************************************************************
 * The Ship class provides the basis for any ship
 *****************************************************************************/

class Ship : public PhysicsObject3D {
private:
	/* The lasers for this ship */
	Lasers* lasers;

	/* The health of this ship */
	unsigned int health;

	/* The (strength of the) shield left of this ship */
	unsigned int shield;
	unsigned int shieldMax;

	/* The regeneration rate of the shield */
	float shieldRegenRate;

	/* The movement speed for this ship */
	float movementSpeed;

	/* The maximum speed for this ship */
	float maximumSpeed;

	/* Timer for regenerating the shield */
	Timer shieldTimer;
public:
	/* The constructor */
	Ship(AsteroidsGame* game);

	/* The destructor */
	virtual ~Ship();

	/* Method called to reset this ship */
	virtual void reset();

	/* Update and render methods */
	virtual void update(float deltaSeconds, AsteroidGroup& closestGroup);
	void render();

	/* Method used to remove health from this ship */
	void removeHealth(unsigned int amount);

	/* Called when an asteroid has been destroyed by this ship's lasers */
	virtual void onAsteroidDestroyed(GameObject3D* asteroid) {}

	/* Method used to check whether a laser has collided with anything */
	virtual bool checkCollision(PhysicsObject3D* laser) { return false; }

	/* Method called to fire this ship's lasers */
	inline void fireLasers(Vector3f front) { lasers->fire(getPosition(), getRotation(), front, getVelocity()); }

	/* Method called to thrust in a particular direction */
	inline void thrust(Vector3f direction) { setAcceleration(direction * movementSpeed); }

	/* Setters and getters */
	inline void setHealth(unsigned int health) { this->health = health; }
	inline void setShield(unsigned int shield) { this->shield = shield; }
	inline void setShieldMax(unsigned int shieldMax) { this->shieldMax = shieldMax; }
	inline void setShieldRegenRate(float shieldRegenRate) { this->shieldRegenRate = shieldRegenRate; }
	inline void setMovementSpeed(float speed) { movementSpeed = speed; }
	inline void setMaximumSpeed(float speed) { maximumSpeed = speed; }
	inline Lasers* getLasers() { return lasers; }
	inline unsigned int getHealth() { return health; }
	inline unsigned int getShield() { return shield; }
	inline unsigned int getShieldMax() { return shieldMax; }
	inline bool isAlive() { return health > 0; }
	inline float getMovementSpeed() { return movementSpeed; }
	inline float getMaximumSpeed() { return maximumSpeed; }
};


#endif /* EXAMPLES_ASTEROIDS_SHIP_H_ */
