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

#ifndef CORE_OBJECT_H_
#define CORE_OBJECT_H_

#include "Vector.h"
#include "Rectangle.h"
#include "render/Mesh.h"

/*****************************************************************************
 * The GameObject class is the basis of any objects used in a Game, but does
 * not need to be renderable
 *****************************************************************************/

class GameObject {
private:
	/* Mesh instances associated with this object */
	std::vector<Mesh*> meshes;

	/* The model matrix used when rendering this object */
	Matrix4f modelMatrix;

	/* The RenderShader used when rendering */
	RenderShader* renderShader;
public:
	/* The constructors */
	GameObject(Mesh* mesh = NULL, RenderShader* shader = NULL) : renderShader(shader) {
		if (mesh) {
			mesh->setup(shader);
			meshes.push_back(mesh);
		}
	}
	GameObject(std::vector<Mesh*> meshes, RenderShader* shader) : renderShader(shader) {
		this->meshes = meshes;

		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i]->setup(shader);
	}

	/* The destructor */
	virtual ~GameObject() {
		if (hasMesh()) {
			for (unsigned int i = 0; i < meshes.size(); i++)
				delete meshes[i];
			meshes.clear();
		}
	}

	/* Overrideable methods to update and render the GameObject */
	virtual void update() {}
	virtual void render(bool overrideShader = false);

	/* Method used to add a mesh */
	void addMesh(Mesh* mesh) {
		mesh->setup(renderShader);
		meshes.push_back(mesh);
	}

	/* Setters and getters */
	void setModelMatrix(Matrix4f modelMatrix) { this->modelMatrix = modelMatrix; }
	Matrix4f& getModelMatrix() { return modelMatrix; }

	inline void setRenderShader(RenderShader* renderShader) { this->renderShader = renderShader; }

	bool hasMesh() { return meshes.size() > 0; }
	std::vector<Mesh*>& getMeshes() { return meshes; }

	/* Returns the material a MeshRenderData has, should not be used unless
	 * the object has a Material */
	inline Material* getMaterial() { return meshes[0]->getMaterial(); }

	inline RenderShader* getRenderShader() { return renderShader; }
	inline Shader* getShader() { return renderShader->getShader(); }
};

/*****************************************************************************
 * The GameObject2D class is an implementation of GameObject for 2D
 *****************************************************************************/

class GameObject2D : public GameObject {
protected:
	/* Various information about this object */
	Vector2f position;
	float    rotation;
	Vector2f scale;
	Vector2f size;

	/* Allows objects to be linked together and have their positions relative
	 * to each other */
	GameObject2D* parent = NULL;
public:
	/* The constructors */
	GameObject2D(float width = 0, float height = 0) : GameObject(NULL, NULL) {
		rotation = 0;
		scale = Vector2f(1.0f, 1.0f);
		size = Vector2f(width, height);
	}

	GameObject2D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0) : GameObject(mesh, shader) {
		rotation = 0;
		scale = Vector2f(1.0f, 1.0f);
		size = Vector2f(width, height);
	}

	/* The destructor */
	virtual ~GameObject2D() {}

	virtual void update() override;

	/* Setters and getters */
	void setPosition(Vector2f position) { this->position = position; }
	void setPosition(float x, float y) { position = Vector2f(x, y); }
	void setRotation(float rotation) { this->rotation = rotation; }
	void setScale(Vector2f scale) { this->scale = scale; }
	void setScale(float x, float y) { scale = Vector2f(x, y); }
	void setSize(Vector2f size) { this->size = size; }
	void setSize(float width, float height) { size = Vector2f(width, height); }
	void setWidth(float width) { size.setX(width); }
	void setHeight(float height) { size.setY(height); }

	Vector2f& getRelPosition() { return position; }
	float& getRelRotation() { return rotation; }
	Vector2f& getRelScale() { return scale; }

	Vector2f getPosition() {
		if (parent)
			return position + parent->getPosition();
		else
			return position;
	}

	float getRotation() {
		if (parent)
			return rotation + parent->getRotation();
		else
			return rotation;
	}

	Vector2f getScale() {
		if (parent)
			return scale * parent->getScale();
		else
			return scale;
	}

	Vector2f getSize() {
		return size * getScale();
	}

	float getWidth() { return size.getX() * getScale().getX(); }
	float getHeight() { return size.getY() * getScale().getY(); }

	/* Returns the bounds of this object in the form of a Rectangle */
	Rect getBounds() { return Rect(getPosition(), getSize()); }

	void setParent(GameObject2D* parent) { this->parent = parent; }
	bool hasParent() { return parent; }
	GameObject2D* getParent() { return parent; }
};

/*****************************************************************************
 * The GameObject3D class is an implementation of GameObject for 3D
 *****************************************************************************/

class GameObject3D : public GameObject {
protected:
	/* Various information about this object */
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;
	Vector3f size;

	/* Allows objects to be linked together and have their positions relative
	 * to each other */
	GameObject3D* parent = NULL;
public:
	/* The constructors */
	GameObject3D(float width = 0, float height = 0, float depth = 0) : GameObject(NULL, NULL) {
		rotation = 0;
		scale = Vector3f(1.0f, 1.0f, 1.0f);
		size = Vector3f(width, height, depth);
	}

	GameObject3D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject(mesh, shader) {
		rotation = 0;
		scale = Vector3f(1.0f, 1.0f, 1.0f);
		size = Vector3f(width, height, depth);
	}

	GameObject3D(std::vector<Mesh*> meshes, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject(meshes, shader) {
		rotation = 0;
		scale = Vector3f(1.0f, 1.0f, 1.0f);
		size = Vector3f(width, height, depth);
	}

	/* The destructor */
	virtual ~GameObject3D() {}

	virtual void update() override;

	/* Setters and getters */
	void setPosition(Vector3f position) { this->position = position; }
	void setPosition(float x, float y, float z) { position = Vector3f(x, y, z); }
	void setRotation(Vector3f rotation) { this->rotation = rotation; }
	void setRotation(float x, float y, float z) { rotation = Vector3f(x, y, z); }
	void setScale(Vector3f scale) { this->scale = scale; }
	void setScale(float x, float y, float z) { scale = Vector3f(x, y, z); }
	void setSize(Vector3f size) { this->size = size; }
	void setSize(float width, float height, float depth) { size = Vector3f(width, height, depth); }
	void setWidth(float width) { size.setX(width); }
	void setHeight(float height) { size.setY(height); }
	void setDepth(float depth) { size.setZ(depth); }

	Vector3f& getRelPosition() { return position; }
	Vector3f& getRelRotation() { return rotation; }
	Vector3f& getRelScale() { return scale; }

	Vector3f getPosition() {
		if (parent)
			return position + parent->getPosition();
		else
			return position;
	}

	Vector3f getRotation() {
		if (parent)
			return rotation + parent->getRotation();
		else
			return rotation;
	}

	Vector3f getScale() {
		if (parent)
			return scale * parent->getScale();
		else
			return scale;
	}

	Vector3f getSize() {
		return size * getScale();
	}

	float getWidth() { return size.getX() * getScale().getX(); }
	float getHeight() { return size.getY() * getScale().getY(); }
	float getDepth() { return size.getZ() * getScale().getZ(); }

	void setParent(GameObject3D* parent) { this->parent = parent; }
	bool hasParent() { return parent; }
	GameObject3D* getParent() { return parent; }
};

#endif /* CORE_OBJECT_H_ */
