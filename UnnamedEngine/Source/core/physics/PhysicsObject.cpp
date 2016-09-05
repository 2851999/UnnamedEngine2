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

#include "PhysicsObject.h"

/*****************************************************************************
 * The PhysicsObject2D class
 *****************************************************************************/

void PhysicsObject2D::updatePhysics(float delta) {
	//Update the velocity by applying the acceleration
	velocity += acceleration * delta;
	//Update the angular velocity in the same way
	angularVelocity += angularAcceleration * delta;

	//Update the position and rotation
	position += velocity * delta;
	rotation += angularVelocity * delta;
}

/*****************************************************************************
 * The PhysicsObject3D class
 *****************************************************************************/

void PhysicsObject3D::updatePhysics(float delta) {
	//Update the velocity by applying the acceleration
	velocity += acceleration * delta;
	//Update the angular velocity in the same way
	angularVelocity += angularAcceleration * delta;

	//Update the position and rotation
	position += velocity * delta;
	rotation += angularVelocity * delta;

	//Update the model view matrix if the mesh exists
	if (hasMesh())
		update();
}
