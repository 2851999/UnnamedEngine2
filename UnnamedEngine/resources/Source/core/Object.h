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

#include "Rectangle.h"
#include "Transform.h"
#include "Vector.h"
#include "render/Mesh.h"
#include "Frustum.h"

/*****************************************************************************
 * The GameObject class is the basis of any objects used in a Game, but does
 * not need to be renderable
 *****************************************************************************/

class GameObject {
private:
	/* Mesh instance associated with this object */
	Mesh* mesh;

	/* The RenderShader used when rendering */
	RenderShader* renderShader;
protected:
	/* Transform for this object */
	Transform* transform = new Transform();
public:
	/* The constructors */
	GameObject(Mesh* mesh = NULL, RenderShader* shader = NULL);

	/* The destructor */
	virtual ~GameObject();

	/* Overrideable methods to update and render the GameObject */
	virtual void update() {}
	virtual void render();

	/* Method used to set the parent of this object */
	inline void setParent(GameObject* parent) { transform->setParent(parent->getTransform()); }
	/* method used to remove a child object */
	inline void removeChild(GameObject* child) { transform->removeChild(child->getTransform()); }

	/* Setters and getters */
	inline void setModelMatrix(Matrix4f modelMatrix) { transform->setMatrix(modelMatrix); }
	inline Matrix4f& getModelMatrix() { return transform->getMatrix(); }

	inline void setRenderShader(RenderShader* renderShader) { this->renderShader = renderShader; }

	/* Used to set the internal mesh, previous will be deleted if there was one,
	 * if a shader is supplied, then setup will be called on it otherwise it is assumed
	 * it has already been setup */
	void setMesh(Mesh* mesh, RenderShader* shader = NULL);

	inline bool hasMesh() { return mesh; }
	inline Mesh* getMesh() { return mesh; }

	/* Returns the material a MeshRenderData has, should not be used unless
	 * the object has a mesh */
	inline bool hasMaterial() { return mesh->hasMaterial(); }
	inline Material* getMaterial() { return mesh->getMaterial(); }

	inline Transform* getTransform() { return transform; }
	inline RenderShader* getRenderShader() { return renderShader; }
	inline Shader* getShader() { return renderShader->getShader(); }
	virtual bool shouldCull() { return false; }
};

/*****************************************************************************
 * The GameObject2D class is an implementation of GameObject for 2D
 *****************************************************************************/

class GameObject2D : public GameObject {
protected:
	/* Various information about this object */
	Vector2f size;
public:
	/* The constructors */
	GameObject2D(float width = 0, float height = 0);
	GameObject2D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0) : GameObject(mesh, shader) { setSize(width, height); }
	GameObject2D(Mesh* mesh, unsigned int shaderID, float width = 0, float height = 0);

	/* The destructor */
	virtual ~GameObject2D() {}

	virtual void update() override;

	/* Setters and getters */
	inline void setPosition(Vector2f position) { transform->setPosition(position); }
	inline void setX(float x) { transform->setX(x); }
	inline void setY(float y) { transform->setY(y); }
	inline void setPosition(float x, float y)  { transform->setPosition(x, y); }
	inline void setRotation(Quaternion rotation) { transform->setRotation(rotation); }
	inline void setRotation(float rotation)    { transform->setRotation(rotation); }
	inline void setScale(Vector2f scale)       { transform->setScale(scale); }
	inline void setScale(float x, float y)     { transform->setScale(x, y); }
	inline void setSize(Vector2f size)         { this->size = size; }
	inline void setSize(float width, float height) { size = Vector2f(width, height); }
	inline void setWidth(float width)          { size.setX(width); }
	inline void setHeight(float height)        { size.setY(height); }

	inline Vector3f   getLocalPosition()      { return transform->getLocalPosition(); }
	inline Quaternion getLocalRotation()      { return transform->getLocalRotation(); }
	inline float      getLocalRotationEuler() { return transform->getLocalRotation().toEuler().getZ(); }
	inline Vector3f   getLocalScale()         { return transform->getLocalScale(); }

	inline Vector3f   getPosition()      { return transform->getPosition(); }
	inline Quaternion getRotation()      { return transform->getRotation(); }
	inline float      getRotationEuler() { return transform->getRotation().toEuler().getZ(); }
	inline Vector3f   getScale()         { return transform->getScale(); }
	Vector2f getSize();

	inline Vector2f& getLocalSize() { return size; }

	inline float getWidth() { return size.getX() * getScale().getX(); }
	inline float getLocalWidth() { return size.getX(); }
	inline float getHeight() { return size.getY() * getScale().getY(); }
	inline float getLocalHeight() { return size.getY(); }

	/* Returns the bounds of this object in the form of a Rectangle */
	inline Rect getBounds() { return Rect(Vector2f(getPosition().getX(), getPosition().getY()), Vector2f(getSize().getX(), getSize().getY())); }
};

/*****************************************************************************
 * The GameObject3D class is an implementation of GameObject for 3D
 *****************************************************************************/

class GameObject3D : public GameObject {
protected:
	/* Various information about this object */
	Vector3f size;

	/* The centre of this object used for culling purposes */
	Vector3f cullingCentre;
public:
	/* The constructors */
	GameObject3D(float width = 0, float height = 0, float depth = 0);
	GameObject3D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject(mesh, shader) { setSize(width, height, depth); }
	GameObject3D(Mesh* mesh, unsigned int shaderID, float width = 0, float height = 0, float depth = 0);

	/* The destructor */
	virtual ~GameObject3D() {}

	virtual void update() override;

	/* Setters and getters */
	inline void setPosition(Vector3f position) { transform->setPosition(position); }
	inline void setPosition(float x, float y, float z)  { transform->setPosition(x, y, z); }
	inline void setX(float x) { transform->setX(x); }
	inline void setY(float y) { transform->setY(y); }
	inline void setZ(float z) { transform->setZ(z); }
	inline void setRotation(Quaternion rotation) { transform->setRotation(rotation); }
	inline void setRotation(Vector3f rotation) { transform->setRotation(rotation); }
	inline void setRotation(float x, float y, float z) { transform->setRotation(x, y, z); }
	inline void setScale(Vector3f scale)       { transform->setScale(scale); }
	inline void setScale(float x, float y, float z) { transform->setScale(x, y, z); }
	inline void setSize(Vector3f size)         { this->size = size; }
	inline void setSize(float width, float height, float depth) { size = Vector3f(width, height, depth); }
	inline void setWidth(float width)          { size.setX(width); }
	inline void setHeight(float height)        { size.setY(height); }
	inline void setDepth(float depth)          { size.setZ(depth); }

	inline Vector3f   getLocalPosition()      { return transform->getLocalPosition(); }
	inline Quaternion getLocalRotation()      { return transform->getLocalRotation(); }
	inline Vector3f   getLocalRotationEuler() { return transform->getLocalRotation().toEuler(); }
	inline Vector3f   getLocalScale()         { return transform->getLocalScale(); }

	inline Vector3f   getPosition()      { return transform->getPosition(); }
	inline Quaternion getRotation()      { return transform->getRotation(); }
	inline Vector3f   getRotationEuler() { return transform->getRotation().toEuler(); }
	inline Vector3f   getScale()         { return transform->getScale(); }
	Vector3f getSize();

	inline Vector3f& getLocalSize() { return size; }

	inline float getWidth() { return size.getX() * getScale().getX(); }
	inline float getLocalWidth() { return size.getX(); }
	inline float getHeight() { return size.getY() * getScale().getY(); }
	inline float getLocalHeight() { return size.getY(); }
	inline float getDepth() { return size.getZ() * getScale().getZ(); }
	inline float getLocalDepth() { return size.getZ(); }

	bool shouldCull(Frustum& frustum);
	bool shouldCull();
};

