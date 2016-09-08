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

#include "AsteroidGroup.h"

/*****************************************************************************
 * The AsteroidGroup class
 *****************************************************************************/

AsteroidGroup::AsteroidGroup(AsteroidsRenderer* renderer, unsigned int rendererStartIndex, Vector3f position) :
	renderer(renderer), rendererStartIndex(rendererStartIndex), position(position) {

}

AsteroidGroup::~AsteroidGroup() {

}

void AsteroidGroup::generateAsteroids(unsigned int number, AsteroidsRenderer* renderer) {
	//Go through and generate the requested number of asteroids
	for (unsigned int i = 0; i < number; i++) {
		GameObject3D* asteroid = new GameObject3D();

		objects.push_back(asteroid);
		renderer->addAsteroid(asteroid);
	}
}

void AsteroidGroup::setup() {
	//Go through all of the asteroids
	for (unsigned int i = 0; i < objects.size(); i++) {
		//Setup the current asteroid
		objects[i]->setPosition(position + Vector3f(RandomUtils::randomFloat(-20, 20), RandomUtils::randomFloat(-20, 20), RandomUtils::randomFloat(-20, 20)));
		float s = RandomUtils::randomFloat(0.2f, 1.0f);
		objects[i]->setScale(s * RandomUtils::randomFloat(1.0f, 1.5f), s * RandomUtils::randomFloat(1.0f, 1.5f), s * RandomUtils::randomFloat(1.0f, 1.5f));
		objects[i]->setRotation(RandomUtils::randomFloat(0, 360), RandomUtils::randomFloat(0, 360), RandomUtils::randomFloat(0, 360));
	}
}

