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

#include "Matrix.h"
#include "Quaternion.h"

#include <algorithm>
#include <iostream>

/*****************************************************************************
 * The Transform class takes care of transformations and produces a resultant
 * model matrix
 *****************************************************************************/

class Transform {
private:
	/* The position vector */
	Vector3f position;
	/* The rotation (stored as a quaternion) */
	Quaternion rotation;
	/* The scale vector */
	Vector3f m_scale = Vector3f(1.0f, 1.0f, 1.0f);

	/* The local data of the above */
	Vector3f   localPosition;
	Quaternion localRotation;
	Vector3f   localScale = Vector3f(1.0f, 1.0f, 1.0f);

	/* The matrix of this transform */
	Matrix4f matrix;

	/* Pointer to the parent transform (if there is one) */
	Transform* parent = NULL;

	/* Child transforms attached to this one */
	std::vector<Transform*> children;

	/* Boolean value that states whether anything has been changed and so needs to be recalculated */
	bool changed = true;
public:
	/* The constructors */
	Transform() {}
	Transform(Vector3f& position)                                          { setPosition(position); }
	Transform(Vector3f& position, Quaternion& rotation)                    { setPosition(position); setRotation(rotation); }
	Transform(Vector3f& position, Quaternion& rotation, Vector3f& scale)   { setPosition(position); setRotation(rotation); setScale(scale); }
	Transform(Vector3f& position, Vector3f& rotationEuler)                 { setPosition(position); setRotation(rotationEuler); }
	Transform(Vector3f& position, Vector3f& rotationEuler, Vector3f scale) { setPosition(position); setRotation(rotationEuler); setScale(scale); }
	Transform(Vector2f& position)                                          { setPosition(position); }
	Transform(Vector2f& position, float rotation)                          { setPosition(position); setRotation(rotation); }
	Transform(Vector2f& position, float rotation, Vector2f scale)          { setPosition(position); setRotation(rotation); setScale(scale); }

	/* The destructor */
	virtual ~Transform() {}

//	/* Method used to add a child transform */
//	inline void addChild(Transform* child) {
//		//Assign the parent of the child
//		child->setParent(this);
//		child->setPosition(child->getLocalPosition());
//		child->setRotation(child->getLocalRotation());
//		child->setScale(child->getLocalScale());
//		//Add the child
//		children.push_back(child);
//	}

	/* Method used to remove a child transform */
	inline void removeChild(Transform* child) {
		//Remove the child from the children vector
		children.erase(std::remove(children.begin(), children.end(), child), children.end());
	}

	/* Method used to remove all child transforms */
	inline void removeChildren() {
		//Go through each child transform
		for (unsigned int i = 0; i < children.size(); i++)
			//Assign their parent
			children[i]->setParent(NULL);
		//Remove all child transforms
		children.clear();
	}

	/* Method used to calculate this transform's matrix given an offset to do rotations */
	void calculateMatrix(Vector3f offset);
	/* Method used to calculate this transform's matrix */
	void calculateMatrix();

	/* Methods used to translate the position of this transform */
	inline void translate(const Vector3f& amount) { setPosition(this->localPosition + amount); }
	inline void translate(const Vector2f& amount) { translate(Vector3f(amount)); }
	inline void translate(float x, float y, float z = 0) { translate(Vector3f(x, y, z)); }

	/* Methods used to rotate the rotation vector by some amount */
	void rotate(const Quaternion& angle);
	inline void rotate(const Vector3f& axis, float angle) { rotate(Quaternion(axis, angle)); }
	inline void rotate(const Vector3f& angles) { rotate(Quaternion(angles)); }
	inline void rotate(const float& angle) { rotate(Quaternion(Vector3f(0.0f, 0.0f, angle))); }

	/* Methods used to scale this transform */
	inline void scale(const Vector3f& amount) { setScale(this->localScale * amount); }
	inline void scale(const Vector2f& amount) { scale(Vector3f(amount)); }
	inline void scale(const float& amount) { scale(Vector3f(amount)); }
	inline void scale(float x, float y, float z = 0.0f) { scale(Vector3f(x, y, z)); }

	/* Various getters and setters */
	inline void setPosition(Vector3f position) {
		this->localPosition = position;
		this->position = localPosition;
		//Check whether there is a parent
		if (parent)
			this->position += parent->getPosition();
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setPosition(children[i]->getLocalPosition());
		changed = true;
	}
	inline void setPosition(Vector2f position)                { setPosition(Vector3f(position)); }
	inline void setPosition(float x, float y, float z = 0.0f) { setPosition(Vector3f(x, y, z)); }
	inline void setX(float x) { setPosition(x, localPosition.getY(), localPosition.getZ()); }
	inline void setY(float y) { setPosition(localPosition.getX(), y, localPosition.getZ()); }
	inline void setZ(float z) { setPosition(localPosition.getX(), localPosition.getY(), z); }

	inline void setRotation(Quaternion rotation) {
		this->localRotation = rotation;
		this->rotation = localRotation;
		//Check whether there is a parent
		if (parent)
			this->rotation = parent->getRotation() * this->rotation;
		//Assign the child rotations
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setRotation(children[i]->getLocalRotation());
		changed = true;
	}
	inline void setRotation(Vector3f rotation)         { setRotation(Quaternion(rotation)); }
	inline void setRotation(float x, float y, float z) { setRotation(Vector3f(x, y, z)); }
	inline void setRotation(float rotation)            { setRotation(Quaternion(Vector3f(0.0f, 0.0f, 1.0f), rotation)); }

	inline void setScale(Vector3f scale) {
		this->localScale = scale;
		this->m_scale = localScale;
		//Check whether there is a parent
		if (parent)
			this->m_scale *= parent->getScale();
		//Assign the child scales
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setScale(children[i]->getLocalScale());
		changed = true;
	}
	inline void setScale(Vector2f scale)                   { setScale(Vector3f(scale)); }
	inline void setScale(float x, float y, float z = 0.0f) { setScale(Vector3f(x, y, z)); }

	inline void setMatrix(Matrix4f matrix)   { this->matrix = matrix; }
	inline void setParent(Transform* parent) {
		//Check whether there is a parent currently
		if (this->parent)
			//Remove this child from the parent
			this->parent->removeChild(this);
		//Add this child to the parent
		if (parent)
			parent->getChildren().push_back(this);
		//Assign the parent of this transform
		this->parent = parent;

		//Assign this transforms position, rotation and scale based on the new parent (if there is one)
		this->position = this->localPosition;
		this->rotation = this->localRotation;
		this->m_scale = this->localScale;
		if (parent) {
			this->position += parent->getPosition();
			this->rotation = parent->getRotation() * this->rotation;
			this->m_scale = localScale * parent->getScale();
		}

		//Update this transform
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setPosition(children[i]->getLocalPosition());
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setRotation(children[i]->getLocalRotation());
		for (unsigned int i = 0; i < children.size(); i++)
			children[i]->setScale(children[i]->getLocalScale());
		changed = true;
	}

	inline void setHasChanged(bool hasChanged) { this->changed = hasChanged; }

	inline Vector3f   getLocalPosition() { return localPosition; }
	inline Quaternion getLocalRotation() { return localRotation; }
	inline Vector3f   getLocalScale()    { return localScale; }

	inline Vector3f    getPosition() { return position; }
	inline Quaternion  getRotation() { return rotation; }
	inline Vector3f    getScale()    { return m_scale; }

	inline Matrix4f&  getMatrix()  { return matrix; }
	inline Transform* getParent()  { return parent; }
	inline Transform* hasParent()  { return parent; }
	inline bool       hasChanged() { return changed; }
	inline std::vector<Transform*>& getChildren() { return children; }
	inline unsigned int getChildrenCount() { return children.size(); }
	inline bool hasChildren() { return children.size() > 0; }
	inline Transform* getChild(unsigned int i) { return children.at(i); }
};

