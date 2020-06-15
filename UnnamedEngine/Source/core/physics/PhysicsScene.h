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

#pragma once

#include <functional>

#include "PhysicsObject.h"

/*****************************************************************************
 * The PhysicsScene3D class is similar to Scene for rendering; it manages
 * PhysicsObjects and can update them all in one go
 *****************************************************************************/

class PhysicsScene3D {
private:
	/* The PhysicsObjects in this scene */
	std::vector<PhysicsObject3D*> objects;

	/* Collision callback */
	std::function<void(unsigned int, unsigned int)> collisionCallback;
public:
	/* The constructor */
	PhysicsScene3D() {}

	/* The destructor */
	virtual ~PhysicsScene3D();

	/* The method used to update all of the physics objects in the scene */
	void update(float delta);

	/* Resolves a collision between two physics objects */
	void resolveCollision(CollisionData3D& data, PhysicsObject3D* objectA, PhysicsObject3D* objectB);

	/* Used to assign the collision callback */
	void setCollisionCallback(std::function<void(unsigned int, unsigned int)> callback) { collisionCallback = callback; }

	/* Used to add a PhysicsObject to the scene */
	inline void add(PhysicsObject3D* object) { objects.push_back(object); }

	/* Used to get a PhysicsObject from the scene */
	inline PhysicsObject3D* get(unsigned int index) { return objects[index]; }

	/* Used to remove a PhysicsObject from the scene */
	inline void remove(unsigned int index) { objects.erase(objects.begin() + index); }
};

