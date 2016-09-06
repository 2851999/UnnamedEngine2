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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDGROUP_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDGROUP_H_

#include "AsteroidsRenderer.h"

/*****************************************************************************
 * The AsteroidGroup class manages a group of asteroids, allowing collision
 * testing to occur only on the closest group to the player
 *****************************************************************************/

class AsteroidGroup {
private:
	/* The position of this group */
	Vector3f position;

	/* The asteroids in the group */
	std::vector<GameObject3D*> objects;
public:
	/* The constructor */
	AsteroidGroup(Vector3f position);

	/* The destructor */
	virtual ~AsteroidGroup();

	/* The method called to generate asteroids in this group */
	void generateAsteroids(unsigned int number, AsteroidsRenderer* renderer);

	/* Setters and getters */
	Vector3f getPosition() { return position; }
	std::vector<GameObject3D*>& getObjects() { return objects; }
};


#endif /* EXAMPLES_ASTEROIDS_ASTEROIDGROUP_H_ */
