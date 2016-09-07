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

Ship::Ship(SoundSystem* soundSystem, const ResourceLoader& loader) : PhysicsObject3D(NULL) {
	//Setup the lasers
	lasers = new Lasers(soundSystem, loader);
}

Ship::~Ship() {
	//Destroy created resources
	delete lasers;
}

void Ship::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	PhysicsObject3D::updatePhysics(deltaSeconds);
	//Update the lasers
	lasers->update(deltaSeconds, closestGroup);
}

void Ship::render() {
	//Render the lasers
	lasers->render();
}
