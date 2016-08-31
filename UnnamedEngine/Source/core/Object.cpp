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

#include "render/Renderer.h"
#include "Object.h"

/*****************************************************************************
 * The GameObject class
 *****************************************************************************/

void GameObject::render(bool overrideShader) {
	if (hasMesh()) {
		if (! overrideShader)
			renderShader->getShader()->use();
		Renderer::render(meshes, getModelMatrix(), renderShader);
		if (! overrideShader)
			renderShader->getShader()->stopUsing();
	}
}

/*****************************************************************************
 * The GameObject2D class
 *****************************************************************************/

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
	}
}

/*****************************************************************************
 * The GameObject3D class
 *****************************************************************************/

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
