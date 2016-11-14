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

#ifndef CORE_PHYSICS_PHYSICSOBJECT_H_
#define CORE_PHYSICS_PHYSICSOBJECT_H_

#include "Collider.h"
#include "../Object.h"

/*****************************************************************************
 * The PhysicsObject class is the basis of any physics object
 *****************************************************************************/

class PhysicsObject {
private:
	/* The mass of this object */
	float mass = 1.0f;

	/* The coefficient of restitution of this object */
	float restitution = 1.0f;
public:
	/* The constructor */
	PhysicsObject() {}

	/* The destructive */
	virtual ~PhysicsObject() {}

	/* The method used to update this physics object given the time
	 * between the current and last frames measured in seconds */
	virtual void updatePhysics(float delta) {}

	/* Setters and getters */
	inline void setMass(float mass) { this->mass = mass; }
	inline void setRestitution(float restitution) { this->restitution = restitution; }

	inline float getMass() { return mass; }
	inline float getRestitution() { return restitution; }
};

/*****************************************************************************
 * The PhysicsObject2D class is an implementation of the PhysicsObject that
 * inherits position data from GameObject2D
 *****************************************************************************/

class PhysicsObject2D : public PhysicsObject, public GameObject2D {
private:
	/* The velocity of this object */
	Vector2f velocity;

	/* The acceleration of this object */
	Vector2f acceleration;

	/* The angular velocity */
	float angularVelocity = 0;

	/* The angular acceleration */
	float angularAcceleration = 0;

	/* The collider (can be NULL) */
	Collider2D* collider = NULL;
public:
	/* The constructor - Takes the GameObject that this will be controlling, once
	 * this is done, the position/rotation of the child object should only be
	 * modified by changing the value in this instance */
	PhysicsObject2D(GameObject2D* child) {
		if (child)
			addChild(child);
	}

	PhysicsObject2D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0) : GameObject2D(mesh, shader, width, height) {}
	PhysicsObject2D(std::vector<Mesh*> meshes, RenderShader* shader, float width = 0, float height = 0) : GameObject2D(meshes, shader, width, height) {}
	PhysicsObject2D(Mesh* mesh, std::string shaderId, float width = 0, float height = 0) : GameObject2D(mesh, shaderId, width, height) {}
	PhysicsObject2D(std::vector<Mesh*> meshes, std::string shaderId, float width = 0, float height = 0) : GameObject2D(meshes, shaderId, width, height) {}

	/* The destructor */
	virtual ~PhysicsObject2D() {}

	/* The update method inherited from PhysicsObject */
	virtual void updatePhysics(float delta) override;

	/* Setters and getters */
	void setVelocity(Vector2f velocity) { this->velocity = velocity; }
	void setVelocity(float x, float y) { velocity = Vector2f(x, y); }
	void setAcceleration(Vector2f acceleration) { this->acceleration = acceleration; }
	void setAcceleration(float x, float y) { acceleration = Vector2f(x, y); }
	void setAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }
	void setAngularAcceleration(float angularAcceleration) { this->angularAcceleration = angularAcceleration; }

	inline void setCollider(Collider2D* collider) { this->collider = collider; }

	Vector2f getVelocity() { return velocity; }
	Vector2f& getRelVelocity() { return velocity; }
	Vector2f getAcceleration() { return acceleration; }
	Vector2f& getRelAcceleration() { return acceleration; }
	float getAngularVelocity() { return angularVelocity; }
	float getAngularAcceleration() { return angularAcceleration; }

	inline Collider2D* getCollider() { return collider; }
	inline bool hasCollider() { return collider; }
};

/*****************************************************************************
 * The PhysicsObject3D class is an implementation of the PhysicsObject that
 * inherits position data from GameObject3D
 *****************************************************************************/

class PhysicsObject3D : public PhysicsObject, public GameObject3D {
private:
	/* The velocity of this object */
	Vector3f velocity;

	/* The acceleration of this object */
	Vector3f acceleration;

	/* The angular velocity */
	Vector3f angularVelocity;

	/* The angular acceleration */
	Vector3f angularAcceleration;

	/* The collider (can be NULL) */
	Collider3D* collider = NULL;
public:
	/* The constructor - Takes the GameObject that this will be controlling, once
	 * this is done, the position/rotation of the child object should only be
	 * modified by changing the value in this instance */
	PhysicsObject3D(GameObject3D* child) {
		if (child)
			addChild(child);
	}
	PhysicsObject3D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject3D(mesh, shader, width, height, depth) {}
	PhysicsObject3D(std::vector<Mesh*> meshes, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject3D(meshes, shader, width, height, depth) {}
	PhysicsObject3D(Mesh* mesh, std::string shaderId, float width = 0, float height = 0, float depth = 0) : GameObject3D(mesh, shaderId, width, height, depth) {}
	PhysicsObject3D(std::vector<Mesh*> meshes, std::string shaderId, float width = 0, float height = 0, float depth = 0) : GameObject3D(meshes, shaderId, width, height, depth) {}

	/* The destructor */
	virtual ~PhysicsObject3D() {}

	/* The update method inherited from PhysicsObject */
	virtual void updatePhysics(float delta) override;

	/* Setters and getters */
	void setVelocity(Vector3f velocity) { this->velocity = velocity; }
	void setVelocity(float x, float y, float z) { velocity = Vector3f(x, y, z); }
	void setAcceleration(Vector3f acceleration) { this->acceleration = acceleration; }
	void setAcceleration(float x, float y, float z) { acceleration = Vector3f(x, y, z); }
	void setAngularVelocity(Vector3f angularVelocity) { this->angularVelocity = angularVelocity; }
	void setAngularVelocity(float x, float y, float z) { angularVelocity = Vector3f(x, y, z); }
	void setAngularAcceleration(Vector3f angularAcceleration) { this->angularAcceleration = angularAcceleration; }
	void setAngularAcceleration(float x, float y, float z) { angularAcceleration = Vector3f(x, y, z); }

	inline void setCollider(Collider3D* collider) { this->collider = collider; }

	Vector3f getVelocity() { return velocity; }
	Vector3f& getRelVelocity() { return velocity; }
	Vector3f getAcceleration() { return acceleration; }
	Vector3f& getRelAcceleration() { return acceleration; }
	Vector3f getAngularVelocity() { return angularVelocity; }
	Vector3f& getRelAngularVelocity() { return angularVelocity; }
	Vector3f getAngularAcceleration() { return angularAcceleration; }
	Vector3f& getRelAngularAcceleration() { return angularAcceleration; }

	inline Collider3D* getCollider() { return collider; }
	inline bool hasCollider() { return collider; }
};

#endif /* CORE_PHYSICS_PHYSICSOBJECT_H_ */
