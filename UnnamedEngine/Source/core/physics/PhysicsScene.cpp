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

#include "PhysicsScene.h"

/*****************************************************************************
 * The PhysicsScene3D class
 *****************************************************************************/

PhysicsScene3D::~PhysicsScene3D() {
//	for (unsigned int i = 0; i < objects.size(); i++)
//		delete objects[i];
//	objects.clear();
}

void PhysicsScene3D::update(float delta) {
	bool collision = false;
	for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->updatePhysics(delta);

		//Check for a collider
		if (objects[i]->hasCollider() && ! collision) {
			//Go through all of the other objects after the current one,
			//ensuring the same two objects are not collision tested twice
			for (unsigned int j = i + 1; j < objects.size(); j++) {
				//Ensure the other object also has a collider
				if (objects[j]->hasCollider()) {
					//Get the collision data
					CollisionData3D data = objects[i]->getCollider()->intersects(objects[j]->getCollider());
					//Check for an intersection
					if (data.getDoesIntersect()) {
						//Check to see if the callback has been assigned
						if (collisionCallback)
							//Call the callback
							collisionCallback(i, j);
						//Resolve the collision
						resolveCollision(data, objects[i], objects[j]);
					}
				}
			}
		}
	}
}

void PhysicsScene3D::resolveCollision(CollisionData3D data, PhysicsObject3D* objectA, PhysicsObject3D* objectB) {
	Vector3f relativeVelocity = objectB->getVelocity() - objectA->getVelocity();

	float velAlongNormal = relativeVelocity.dot(data.getNormal());

	if (velAlongNormal > 0)
		return;

	float e = MathsUtils::min(objectA->getRestitution(), objectB->getRestitution());

	float j = (-(1.0f + e) * velAlongNormal) / (1.0f / objectA->getMass() + 1.0f / objectB->getMass());

	Vector3f impulse = data.getNormal() * j;
	objectA->setVelocity(objectA->getVelocity() - impulse * (1.0f / objectA->getMass()));
	objectB->setVelocity(objectB->getVelocity() + impulse * (1.0f / objectB->getMass()));
}
