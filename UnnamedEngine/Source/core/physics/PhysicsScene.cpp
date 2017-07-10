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
	for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->updatePhysics(delta);

		//Check for a collider
		if (objects[i]->hasCollider()) {
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

void PhysicsScene3D::resolveCollision(CollisionData3D& data, PhysicsObject3D* objectA, PhysicsObject3D* objectB) {
	//Calculate the relative velocity of the two objects
	Vector3f relativeVelocity = objectB->getVelocity() - objectA->getVelocity();

	//Now calculate the proportion of that velocity along the normal of the collision
	float velAlongNormal = relativeVelocity.dot(data.getNormal());

	//Return if nothing should happen
	if (velAlongNormal > 0)
		return;

	//Pick the coefficient of restitution from one of the objects
	float e = utils_maths::min(objectA->getRestitution(), objectB->getRestitution());

	//Calculate the inverse of each mass
	float invMassA = 1.0f / objectA->getMass();
	float invMassB = 1.0f / objectB->getMass();

	//Calculate the magnitude of the impulse
	float j = (-(1.0f + e) * velAlongNormal) / (invMassA + invMassB);

	//Calculate the impulse to be applied to both objects
	Vector3f impulse = data.getNormal() * j;

	//Apply the impulse depending on each object's mass
	objectA->setVelocity(objectA->getVelocity() - impulse * invMassA);
	objectB->setVelocity(objectB->getVelocity() + impulse * invMassB);
}
