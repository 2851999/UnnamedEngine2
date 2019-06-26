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

#include "Collider.h"

/*****************************************************************************
 * The SphereCollider class
 *****************************************************************************/

CollisionData3D SphereCollider::intersects(Collider3D* collider) {
	//Check the other collider's type
	if (collider->getType() == TYPE_SPHERE) {
		//Cast the other collider to a SphereCollider
		SphereCollider* other = (SphereCollider*) collider;

		//Get the relative position between the two spheres
		Vector3f relPos = other->getPosition() - getPosition();

		//Get the distance between the two spheres
		float distanceBetween = relPos.length();

		//Return the result
		return CollisionData3D(distanceBetween < (radius + other->getRadius()), relPos.normalise());
	} else if (collider->getType() == TYPE_PLANE) {
		//Cast the other collider to a PlaneCollider3D
		PlaneCollider3D* other = (PlaneCollider3D*) collider;

		//Check for an intersection and return the result
		return other->intersects(this);
	}

	return CollisionData3D(false, 0);
}

/*****************************************************************************
 * The PlaneCollider3D class
 *****************************************************************************/

CollisionData3D PlaneCollider3D::intersects(Collider3D* collider) {
	//Check the other collider's type
	if (collider->getType() == TYPE_SPHERE) {
		//Cast the other collider to a SphereCollider
		SphereCollider* other = (SphereCollider*) collider;

		//Get the normal correctly rotated
		Vector3f norm = Quaternion::rotate(normal, getRotation());
		norm.normalise();

		//Get the relative position between the two spheres
		Vector3f relPos = other->getPosition() - getPosition();

		//Calculate perpendicular distance between the centre of the sphere
		//and the plane (using vector projection, and that the normal should
		//be normalised)
		float lambda = relPos.dot(norm) / norm.dot(norm);

		//Return the result
		return CollisionData3D(lambda < other->getRadius(), norm);
	}

	return CollisionData3D(false, 0);
}
