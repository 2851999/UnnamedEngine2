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

GameObject::GameObject(Mesh* mesh, RenderShader* shader) : mesh(mesh), renderShader(shader) {
	if (this->mesh)
		this->mesh->setup(shader);
}

GameObject::~GameObject() {
	if (mesh) {
		delete mesh;
	}
}

void GameObject::render() {
	if (hasMesh()) {
		renderShader->getShader()->use();
		Renderer::render(mesh, getModelMatrix(), renderShader);
	}
}

/*****************************************************************************
 * The GameObject2D class
 *****************************************************************************/

GameObject2D::GameObject2D(float width, float height) : GameObject(NULL, NULL) {
	size = Vector2f(width, height);
}

GameObject2D::GameObject2D(Mesh* mesh, std::string shaderId, float width , float height) :
		GameObject2D(mesh, Renderer::getRenderShader(shaderId), width, height) {}

void GameObject2D::update() {
	//Check to make sure this object has a mesh
	if (hasMesh() && transform->hasChanged())
		transform->calculateMatrix(Vector2f(getWidth() / 2, getHeight() / 2));
}

Vector2f GameObject2D::getSize() {
	return size * Vector2f(getScale().getX(), getScale().getY());
}

/*****************************************************************************
 * The GameObject3D class
 *****************************************************************************/

GameObject3D::GameObject3D(float width, float height, float depth) : GameObject(NULL, NULL) {
	size = Vector3f(width, height, depth);
}

GameObject3D::GameObject3D(Mesh* mesh, std::string shaderId, float width , float height, float depth) :
		GameObject3D(mesh, Renderer::getRenderShader(shaderId), width, height, depth) {}

void GameObject3D::update() {
	//Check to make sure this object has a mesh
	if (hasMesh() && transform->hasChanged())
		transform->calculateMatrix();
}

Vector3f GameObject3D::getSize() {
	return size * getScale();
}
