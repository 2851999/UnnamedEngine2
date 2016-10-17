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

#include "Rectangle.h"
#include "Vector.h"
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
	GameObject(Mesh* mesh = NULL, RenderShader* shader = NULL);
	GameObject(std::vector<Mesh*> meshes, RenderShader* shader);

	/* The destructor */
	virtual ~GameObject();

	/* Overrideable methods to update and render the GameObject */
	virtual void update() {}
	virtual void render();

	/* Method used to add a mesh */
	void addMesh(Mesh* mesh);

	/* Setters and getters */
	inline void setModelMatrix(Matrix4f modelMatrix) { this->modelMatrix = modelMatrix; }
	inline Matrix4f& getModelMatrix() { return modelMatrix; }

	inline void setRenderShader(RenderShader* renderShader) { this->renderShader = renderShader; }

	inline bool hasMesh() { return meshes.size() > 0; }
	inline std::vector<Mesh*>& getMeshes() { return meshes; }

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
	GameObject2D(float width = 0, float height = 0);
	GameObject2D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0) : GameObject2D(std::vector<Mesh*> { mesh }, shader, width, height) {}
	GameObject2D(std::vector<Mesh*> meshes, RenderShader* shader, float width = 0, float height = 0);

	/* The destructor */
	virtual ~GameObject2D() {}

	virtual void update() override;

	/* Setters and getters */
	inline void setPosition(Vector2f position) { this->position = position; }
	inline void setPosition(float x, float y) { position = Vector2f(x, y); }
	inline void setRotation(float rotation) { this->rotation = rotation; }
	inline void setScale(Vector2f scale) { this->scale = scale; }
	inline void setScale(float x, float y) { scale = Vector2f(x, y); }
	inline void setSize(Vector2f size) { this->size = size; }
	inline void setSize(float width, float height) { size = Vector2f(width, height); }
	inline void setWidth(float width) { size.setX(width); }
	inline void setHeight(float height) { size.setY(height); }

	inline Vector2f& getRelPosition() { return position; }
	inline float& getRelRotation() { return rotation; }
	inline Vector2f& getRelScale() { return scale; }

	Vector2f getPosition();
	float getRotation();
	Vector2f getScale();
	Vector2f getSize();

	inline Vector2f& getRelSize() { return size; }

	inline float getWidth() { return size.getX() * getScale().getX(); }
	inline float getRelWidth() { return size.getX(); }
	inline float getHeight() { return size.getY() * getScale().getY(); }
	inline float getRelHeight() { return size.getY(); }

	/* Returns the bounds of this object in the form of a Rectangle */
	inline Rect getBounds() { return Rect(getPosition(), getSize()); }

	inline void setParent(GameObject2D* parent) { this->parent = parent; }
	inline bool hasParent() { return parent; }
	inline GameObject2D* getParent() { return parent; }
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
	GameObject3D(float width = 0, float height = 0, float depth = 0);
	GameObject3D(Mesh* mesh, RenderShader* shader, float width = 0, float height = 0, float depth = 0) : GameObject3D(std::vector<Mesh*> { mesh }, shader, width, height, depth) {}
	GameObject3D(std::vector<Mesh*> meshes, RenderShader* shader, float width = 0, float height = 0, float depth = 0);

	/* The destructor */
	virtual ~GameObject3D() {}

	virtual void update() override;

	/* Setters and getters */
	inline void setPosition(Vector3f position) { this->position = position; }
	inline void setPosition(float x, float y, float z) { position = Vector3f(x, y, z); }
	inline void setRotation(Vector3f rotation) { this->rotation = rotation; }
	inline void setRotation(float x, float y, float z) { rotation = Vector3f(x, y, z); }
	inline void setScale(Vector3f scale) { this->scale = scale; }
	inline void setScale(float x, float y, float z) { scale = Vector3f(x, y, z); }
	inline void setSize(Vector3f size) { this->size = size; }
	inline void setSize(float width, float height, float depth) { size = Vector3f(width, height, depth); }
	inline void setWidth(float width) { size.setX(width); }
	inline void setHeight(float height) { size.setY(height); }
	inline void setDepth(float depth) { size.setZ(depth); }

	inline Vector3f& getRelPosition() { return position; }
	inline Vector3f& getRelRotation() { return rotation; }
	inline Vector3f& getRelScale() { return scale; }

	Vector3f getPosition();
	Vector3f getRotation();
	Vector3f getScale();
	Vector3f getSize();

	inline Vector3f& getRelSize() { return size; }

	inline float getWidth() { return size.getX() * getScale().getX(); }
	inline float getRelWidth() { return size.getX(); }
	inline float getHeight() { return size.getY() * getScale().getY(); }
	inline float getRelHeight() { return size.getY(); }
	inline float getDepth() { return size.getZ() * getScale().getZ(); }
	inline float getRelDepth() { return size.getZ(); }

	inline void setParent(GameObject3D* parent) { this->parent = parent; }
	inline bool hasParent() { return parent; }
	inline GameObject3D* getParent() { return parent; }
};

#endif /* CORE_OBJECT_H_ */
