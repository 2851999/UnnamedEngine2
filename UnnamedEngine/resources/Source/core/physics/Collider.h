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

#include "../Object.h"

/*****************************************************************************
 * The CollisionData2D class stores some information about a collision
 * between two PhysicsObject2D objects
 *****************************************************************************/

class CollisionData2D {
private:
	/* States whether a collision occurred, and to colliders
	 * are intersecting each other */
	bool doesIntersect;
public:
	/* The constructor */
	CollisionData2D(bool doesIntersect) : doesIntersect(doesIntersect) {}

	/* Getters */
	inline bool getDoesIntersect() { return doesIntersect; }
};

/*****************************************************************************
 * The CollisionData3D class stores some information about a collision
 * between two PhysicsObject3D objects
 *****************************************************************************/

class CollisionData3D {
private:
	/* States whether a collision occurred, and two colliders
	 * are intersecting each other */
	bool doesIntersect;

	/* The normal of the collision */
	Vector3f normal;
public:
	/* The constructor */
	CollisionData3D(bool doesIntersect, Vector3f normal) : doesIntersect(doesIntersect), normal(normal) {}

	/* Getters */
	inline bool getDoesIntersect() { return doesIntersect; }
	inline Vector3f getNormal() { return normal; }
};

/*****************************************************************************
 * The Collider class forms the base of a collider for a PhysicsObject2D
 *****************************************************************************/

class Collider2D {
private:
	unsigned int type;
public:
	/* Various collider types */
	static const unsigned int TYPE_SPHERE = 1;

	/* The constructor */
	Collider2D(unsigned int type) : type(type) {}

	/* The destructor */
	virtual ~Collider2D() {}

	/* The method check whether this collider intersects another collider */
	virtual CollisionData2D intersects(Collider2D* collider) { return CollisionData2D(false); }

	/* Getters */
	unsigned int getType() { return type; }
};

/*****************************************************************************
 * The Collider class forms the base of a collider for a PhysicsObject3D
 *****************************************************************************/

class Collider3D {
private:
	unsigned int type;
public:
	/* Various collider types */
	static const unsigned int TYPE_SPHERE = 1;
	static const unsigned int TYPE_PLANE  = 2;

	/* The constructor */
	Collider3D(unsigned int type) : type(type) {}

	/* The destructor */
	virtual ~Collider3D() {}

	/* The method check whether this collider intersects another collider */
	virtual CollisionData3D intersects(Collider3D* collider) { return CollisionData3D(false, 0); }

	/* Getters */
	unsigned int getType() { return type; }
};

/*****************************************************************************
 * The SphereCollider class is an implementation of a collider for a sphere
 *****************************************************************************/

class SphereCollider : public Collider3D, public GameObject3D {
private:
	/* The radius of the sphere */
	float radius;
public:
	/* The constructor */
	SphereCollider(GameObject3D* object, float radius) : Collider3D(TYPE_SPHERE), radius(radius) { setParent(object); }

	/* The destructor */
	virtual ~SphereCollider() {}

	/* The method used to check whether this collider intersects another collider */
	virtual CollisionData3D intersects(Collider3D* collider);

	/* Getters */
	float getRadius() { return radius; }
};

/*****************************************************************************
 * The PlaneCollider3D class is an implementation of a collider for a plane
 * in 3D
 *****************************************************************************/

class PlaneCollider3D : public Collider3D, GameObject3D {
private:
	/* The normal of this plane (should be normalised when given) */
	Vector3f normal;
public:
	/* The constructor */
	PlaneCollider3D(GameObject3D* object, Vector3f normal) : Collider3D(TYPE_PLANE), normal(normal) { setParent(object); }

	/* The destructor */
	virtual ~PlaneCollider3D() {}

	/* The method used to check whether this collider intersects another collider */
	virtual CollisionData3D intersects(Collider3D* collider);

	/* Getters */
	Vector3f getNormal() { return normal; }
};

