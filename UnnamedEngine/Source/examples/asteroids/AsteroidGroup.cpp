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
#include "Player.h"

/*****************************************************************************
 * The AsteroidGroup class
 *****************************************************************************/

AsteroidGroup::AsteroidGroup(GameRenderer* renderer, unsigned int rendererStartIndex, Vector3f position) :
	renderer(renderer), rendererStartIndex(rendererStartIndex), position(position) {

}

AsteroidGroup::~AsteroidGroup() {

}

void AsteroidGroup::generateAsteroids(unsigned int number, GameRenderer* renderer) {
	//Go through and generate the requested number of asteroids
	for (unsigned int i = 0; i < number; i++) {
		PhysicsObject3D* asteroid = new PhysicsObject3D(NULL);

		objects.push_back(asteroid);
		renderer->add(asteroid);
	}
}

void AsteroidGroup::setup() {
	//Go through all of the asteroids
	for (unsigned int i = 0; i < objects.size(); i++) {
		//Setup the current asteroid
		objects[i]->setPosition(position + Vector3f(utils_random::randomFloat(-20, 20), utils_random::randomFloat(-20, 20), utils_random::randomFloat(-20, 20)));
		float s = utils_random::randomFloat(0.2f, 1.0f);
		objects[i]->setScale(s * utils_random::randomFloat(1.0f, 1.5f), s * utils_random::randomFloat(1.0f, 1.5f), s * utils_random::randomFloat(1.0f, 1.5f));
		objects[i]->setRotation(utils_random::randomFloat(0, 360), utils_random::randomFloat(0, 360), utils_random::randomFloat(0, 360));
		objects[i]->setAngularVelocity(utils_random::randomFloat(0.0f, 20.0f), utils_random::randomFloat(0.0f, 20.0f), utils_random::randomFloat(0.0f, 20.0f));
		objects[i]->setMass(100.0f * s);
		objects[i]->setRestitution(1.0f);
	}
}

void AsteroidGroup::update(float deltaSeconds, Player* player) {
	//Go through and update all of the asteroids in this group
	for (unsigned int i = 0; i < objects.size(); i++) {
		//Only perform collision testing if the object is visible
		if (renderer->isVisible(rendererStartIndex + i)) {
			//Calculate the relative position between the player and the current asteroid
			Vector3f relPos = player->getPosition() - objects[i]->getPosition();
			//Calculate the distance between them
			float distance = relPos.length();
			//Check for a collision
			if (distance < (9.21f / 2.0f * objects[i]->getScale().max()) + 0.5f) {
				//Calculate the normal of the collision
				Vector3f normal = relPos.normalise();

				//Calculate the relative velocity between the player and the asteroid
				Vector3f relativeVelocity = player->getVelocity() - objects[i]->getVelocity();

				//Calculate the relative velocity along the normal
				float velAlongNormal = relativeVelocity.dot(normal);

				if (velAlongNormal < 0) {
					//Get a value for the restitution of the collision
					float e = utils_maths::min(player->getRestitution(), objects[i]->getRestitution());

					float playerInvMass = 1.0f / player->getMass();
					float asteroidInvMass = 1.0f / objects[i]->getMass();

					//Calculate the value of the impulse
					float j = (-(1.0f + e) * velAlongNormal) / (playerInvMass + asteroidInvMass);

					//Calculate the impulse
					Vector3f impulse = normal * j;

					//Apply the impulse to the player and the asteroid - the greater the object's mass
					//the less the impulse should effect it, so multiply by the inverse of the mass
					player->setVelocity(player->getVelocity() + impulse * playerInvMass);
					objects[i]->setVelocity(objects[i]->getVelocity() - impulse * asteroidInvMass);
				}
			}

			//Update the physics of the asteroid
			objects[i]->updatePhysics(deltaSeconds);

			//If the velocity is not <= 0, slow it down by dividing by a value
			if (objects[i]->getVelocity().length() > 0.0f)
				objects[i]->setVelocity(objects[i]->getVelocity() / 1.01f);
		}
	}
	//Update the renderer
	renderer->update(getRendererStartIndex(), getRendererEndIndex());
}

