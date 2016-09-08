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
public:
	/* The constructor */
	Ship(SoundSystem* soundSystem, const ResourceLoader& loader);

	/* The destructor */
	virtual ~Ship();

	/* Method called to reset this ship */
	void reset();

	/* Update and render methods */
	void update(float deltaSeconds, AsteroidGroup& closestGroup);
	void render();

	/* Method called to fire this ship's lasers */
	inline void fireLasers(Vector3f front) { lasers->fire(getPosition(), getRotation(), front); }
};


#endif /* EXAMPLES_ASTEROIDS_SHIP_H_ */
