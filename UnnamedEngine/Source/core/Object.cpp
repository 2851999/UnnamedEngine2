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

#include "Object.h"
#include "render/Renderer.h"

/*****************************************************************************
 * The GameObject class
 *****************************************************************************/

GameObject::GameObject(Mesh* mesh, RenderShader* shader) : renderShader(shader) {
	if (mesh) {
		mesh->setup(shader);
		meshes.push_back(mesh);
	}
}

GameObject::GameObject(std::vector<Mesh*> meshes, RenderShader* shader) : renderShader(shader) {
	this->meshes = meshes;

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->setup(shader);
}

GameObject::~GameObject() {
	if (hasMesh()) {
		for (unsigned int i = 0; i < meshes.size(); i++)
			delete meshes[i];
		meshes.clear();
	}
}

void GameObject::render() {
	if (hasMesh()) {
		renderShader->getShader()->use();
		Renderer::render(meshes, getModelMatrix(), renderShader);
	}
}

void GameObject::addMesh(Mesh* mesh) {
	mesh->setup(renderShader);
	meshes.push_back(mesh);
}

/*****************************************************************************
 * The GameObject2D class
 *****************************************************************************/

GameObject2D::GameObject2D(float width, float height) : GameObject(NULL, NULL) {
	rotation = 0;
	scale = Vector2f(1.0f, 1.0f);
	size = Vector2f(width, height);
}

GameObject2D::GameObject2D(std::vector<Mesh*> meshes, RenderShader* shader, float width, float height) : GameObject(meshes, shader) {
	rotation = 0;
	scale = Vector2f(1.0f, 1.0f);
	size = Vector2f(width, height);
}

GameObject2D::GameObject2D(Mesh* mesh, std::string shaderId, float width , float height) :
		GameObject2D(mesh, Renderer::getRenderShader(shaderId), width, height) {}

GameObject2D::GameObject2D(std::vector<Mesh*> meshes, std::string shaderId, float width, float height) :
		GameObject2D(meshes, Renderer::getRenderShader(shaderId), width, height) {}

//#include "../experimental/Quaternion.h"
void GameObject2D::update() {
	//Check to make sure this object has a mesh
	if (hasMesh()) {
		Matrix4f& modelMatrix = getModelMatrix();

		modelMatrix.setIdentity();

		Vector2f offset = Vector2f(getWidth() / 2, getHeight() / 2);

		modelMatrix.translate(offset + getPosition());
		modelMatrix.rotate(getRotation());
		modelMatrix.translate(offset * -1);
		modelMatrix.scale(getScale());
//		modelMatrix.translate(offset + getPosition());
//		modelMatrix *= Quaternion(Vector3f(0.0f, 0.0f, 1.0f), 3.14159f/2.0f).toRotationMatrix();
//		modelMatrix.translate(offset * -1);
//		modelMatrix.scale(getScale());
	}
}

Vector2f GameObject2D::getPosition() {
	if (parent)
		return position + parent->getPosition();
	else
		return position;
}

float GameObject2D::getRotation() {
	if (parent)
		return rotation + parent->getRotation();
	else
		return rotation;
}

Vector2f GameObject2D::getScale() {
	if (parent)
		return scale * parent->getScale();
	else
		return scale;
}

Vector2f GameObject2D::getSize() {
	return size * getScale();
}

/*****************************************************************************
 * The GameObject3D class
 *****************************************************************************/

GameObject3D::GameObject3D(float width, float height, float depth) : GameObject(NULL, NULL) {
	rotation = 0;
	scale = Vector3f(1.0f, 1.0f, 1.0f);
	size = Vector3f(width, height, depth);
}

GameObject3D::GameObject3D(std::vector<Mesh*> meshes, RenderShader* shader, float width, float height, float depth) : GameObject(meshes, shader) {
	rotation = 0;
	scale = Vector3f(1.0f, 1.0f, 1.0f);
	size = Vector3f(width, height, depth);
}

GameObject3D::GameObject3D(Mesh* mesh, std::string shaderId, float width , float height, float depth) :
		GameObject3D(mesh, Renderer::getRenderShader(shaderId), width, height, depth) {}

GameObject3D::GameObject3D(std::vector<Mesh*> meshes, std::string shaderId, float width, float height, float depth) :
		GameObject3D(meshes, Renderer::getRenderShader(shaderId), width, height, depth) {}

void GameObject3D::update() {
	//Check to make sure this object has a mesh
	if (hasMesh()) {
		Matrix4f& modelMatrix = getModelMatrix();

		modelMatrix.setIdentity();

		Vector3f offset = Vector3f(getWidth() / 2, getHeight() / 2, getDepth() / 2);

		modelMatrix.translate(offset + getPosition());
		modelMatrix.rotate(getRotation());
		modelMatrix.translate(offset * -1);
		modelMatrix.scale(getScale());
	}
}

Vector3f GameObject3D::getPosition() {
	if (parent)
		return position + parent->getPosition();
	else
		return position;
}

Vector3f GameObject3D::getRotation() {
	if (parent)
		return rotation + parent->getRotation();
	else
		return rotation;
}

Vector3f GameObject3D::getScale() {
	if (parent)
		return scale * parent->getScale();
	else
		return scale;
}

Vector3f GameObject3D::getSize() {
	return size * getScale();
}
