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

#include "../../core/render/Renderer.h"
#include "LasersRenderer.h"

/*****************************************************************************
 * The LasersRenderer class
 *****************************************************************************/

LasersRenderer::LasersRenderer(const ResourceLoader& loader, unsigned int numObjects) : numObjects(numObjects)  {
	//Load the model mesh
	mesh = loader.loadModel("laser.obj")[0];
	//mesh->getMaterial()->setDiffuseColour(Colour(mesh->getMaterial()->getDiffuseColour(), 0.5f));
	MeshData* meshData = mesh->getData();
	//Setup the shader
	shader = loader.loadShader("LaserShader");
	//Setup the render data
	renderData = new RenderData(GL_TRIANGLES, meshData->getNumIndices());

	vboVerticesData = new VBO<GLfloat>(GL_ARRAY_BUFFER, meshData->getOthers().size() * sizeof(meshData->getOthers()[0]), meshData->getOthers(), GL_STATIC_DRAW, true);
	vboVerticesData->addAttribute(shader->getAttributeLocation("Position"), 3, 0);
	renderData->addVBO(vboVerticesData);

	vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, meshData->getNumIndices() * sizeof(meshData->getIndices()[0]), meshData->getIndices(), GL_STATIC_DRAW);
	renderData->setIndicesVBO(vboIndices);


	for (unsigned int i = 0; i < numObjects; i++) {
		matricesData.push_back(Matrix4f().initIdentity());

		for (unsigned int x = 0; x < 4; x++) {
			for (unsigned int y = 0; y < 4; y++) {
				matricesDataRaw.push_back(matricesData[i].get(x, y));
			}
		}

		visibleData.push_back(0);
	}

	vboMatricesData = new VBO<GLfloat>(GL_ARRAY_BUFFER, numObjects * 16 * sizeof(GLfloat), matricesDataRaw, GL_STREAM_DRAW, true);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix"), 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 1, 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 2, 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 3, 4, 1);
	renderData->addVBO(vboMatricesData);

	vboVisibleData = new VBO<GLfloat>(GL_ARRAY_BUFFER, numObjects * sizeof(GLfloat), visibleData, GL_STREAM_DRAW, true);
	vboVisibleData->addAttribute(shader->getAttributeLocation("Visible"), 1, 1);
	renderData->addVBO(vboVisibleData);

	renderData->setup();
	renderData->setNumInstances(numObjects);
}

LasersRenderer::~LasersRenderer() {
	delete renderData;
	delete vboVerticesData;
	delete vboIndices;
	delete vboMatricesData;
	delete mesh;

	for (unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();
}

void LasersRenderer::showLaser(unsigned int index) {
	//Ensure the laser is currently showing
	if (visibleData[index] < 0.5f) {
		visibleData[index] = 1.0f;
		vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
	}
}

void LasersRenderer::hideLaser(unsigned int index) {
	//Ensure the laser is currently showing
	if (visibleData[index] > 0.5f) {
		visibleData[index] = 0.0f;
		vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
	}
}

void LasersRenderer::update() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		matricesData[i].initIdentity();
		matricesData[i].translate(objects[i]->getPosition());
		matricesData[i].rotate(objects[i]->getRotation());

		int pos = i * 16;

		for (unsigned int x = 0; x < 4; x++) {
			for (unsigned int y = 0; y < 4; y++) {
				matricesDataRaw[pos] = matricesData[i].get(y, x);
				pos += 1;
			}
		}
	}
	vboMatricesData->updateStream(numObjects * 16 * sizeof(GLfloat));
}

void LasersRenderer::render() {
	//Use the shader
	shader->use();
	Renderer::saveTextures();
	mesh->getMaterial()->setUniforms(shader, "Material");
	shader->setUniformMatrix4("ViewProjectionMatrix", Renderer::getCamera()->getProjectionViewMatrix());

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	renderData->render();
	glDisable(GL_CULL_FACE);

	Renderer::releaseNewTextures();
	shader->stopUsing();
}
