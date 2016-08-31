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

#ifndef EXPERIMENTAL_BILLBOARD_H_
#define EXPERIMENTAL_BILLBOARD_H_

#include "../core/Object.h"

class Billboard : public GameObject3D {
public:
	Billboard(float width, float height) : GameObject3D(new Mesh(MeshBuilder::createQuad3D(width, height)), Renderer::getRenderShader("Billboard"), width, height) {
		getMaterial()->setDiffuseColour(Colour::RED);
	}

	virtual void update() override {}

	virtual void render(bool overrideShaders) override {
		Shader* shader = getShader();
		shader->use();

		Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

		shader->setUniformVector3("Camera_Right", Vector3f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2)));
		shader->setUniformVector3("Camera_Up", Vector3f(matrix.get(1, 0), matrix.get(1, 1), matrix.get(1, 2)));
		shader->setUniformVector2("Billboard_Size", Vector2f(getSize().getX(), getSize().getY()));
		shader->setUniformVector3("Billboard_Centre", getPosition());

		shader->setUniformMatrix4("ProjectionViewMatrix", (Renderer::getCamera()->getProjectionViewMatrix()));
		//Renderer::saveTextures();
		//getMaterial()->setUniforms(getRenderShader());

		Renderer::render(getMeshes(), getModelMatrix(), getRenderShader());

		//Renderer::releaseNewTextures();
	}
};


#endif /* EXPERIMENTAL_BILLBOARD_H_ */
