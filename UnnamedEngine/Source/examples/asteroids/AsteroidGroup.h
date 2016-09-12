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
#include "../../core/physics/PhysicsObject.h"

/*****************************************************************************
 * The AsteroidGroup class manages a group of asteroids, allowing collision
 * testing to occur only on the closest group to the player
 *****************************************************************************/

class Player;

class AsteroidGroup {
private:
	/* Pointer to the renderer of this group (And others) */
	AsteroidsRenderer* renderer;

	/* The index of the start of this groups asteroids in the renderer */
	unsigned int rendererStartIndex;

	/* The position of this group */
	Vector3f position;

	/* The asteroids in the group */
	std::vector<PhysicsObject3D*> objects;
public:
	/* The constructor */
	AsteroidGroup(AsteroidsRenderer* renderer, unsigned int rendererStartIndex, Vector3f position);

	/* The destructor */
	virtual ~AsteroidGroup();

	/* The method called to generate asteroids in this group */
	void generateAsteroids(unsigned int number, AsteroidsRenderer* renderer);

	/* Method called to setup the asteroids */
	void setup();

	/* Method called to update this group of asteroids */
	void update(float deltaSeconds, Player* player);

	/* Method called to hide an asteroid in this group, given its index within
	 * the objects */
	inline void hideAsteroid(unsigned int index) { renderer->hideAsteroid(rendererStartIndex + index); }

	/* Method called to check whether an asteroid is currently visible */
	inline bool isAsteroidVisible(unsigned int index) { return renderer->isAsteroidVisible(rendererStartIndex + index); }

	/* Setters and getters */
	inline unsigned int getRendererStartIndex() const  { return rendererStartIndex; }
	inline unsigned int getRendererEndIndex() const { return rendererStartIndex + objects.size() - 1; }
	inline Vector3f getPosition() { return position; }
	inline std::vector<PhysicsObject3D*>& getObjects() { return objects; }
};


#endif /* EXAMPLES_ASTEROIDS_ASTEROIDGROUP_H_ */
