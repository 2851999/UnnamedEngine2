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

#include "GameRenderer.h"

#include "../../core/render/Renderer.h"

/*****************************************************************************
 * The GameRenderer class
 *****************************************************************************/

GameRenderer::GameRenderer(Mesh* mesh, RenderShader* renderShader, unsigned int numObjects, bool useTextureCoords, bool useLighting, bool useNormalMapping) : mesh(mesh), shader(renderShader->getShader()), numObjects(numObjects), useLighting(useLighting)  {
	//Get the mesh data
	MeshData* meshData = mesh->getData();

	//Setup the render data
	renderData = new RenderData(GL_TRIANGLES, meshData->getNumIndices());

	vboVerticesData = new VBO<GLfloat>(GL_ARRAY_BUFFER, meshData->getOthers().size() * sizeof(meshData->getOthers()[0]), meshData->getOthers(), GL_STATIC_DRAW, true);
	vboVerticesData->addAttribute(shader->getAttributeLocation("Position"), 3, 0);
	if (useTextureCoords)
		vboVerticesData->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2, 0);
	if (useLighting) {
		vboVerticesData->addAttribute(shader->getAttributeLocation("Normal"), 3, 0);
		if (useNormalMapping) {
			vboVerticesData->addAttribute(shader->getAttributeLocation("Tangent"), 3, 0);
			vboVerticesData->addAttribute(shader->getAttributeLocation("Bitangent"), 3, 0);
		}
	}
	renderData->addVBO(vboVerticesData);

	vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, meshData->getNumIndices() * sizeof(meshData->getIndices()[0]), meshData->getIndices(), GL_STATIC_DRAW);
	renderData->setIndicesVBO(vboIndices);


	for (unsigned int i = 0; i < numObjects; i++) {
		matricesData.push_back(Matrix4f().initIdentity());
		normalMatricesData.push_back(Matrix3f().initIdentity());

		for (unsigned int x = 0; x < 4; x++) {
			for (unsigned int y = 0; y < 4; y++) {
				matricesDataRaw.push_back(matricesData[i].get(x, y));
			}
		}

		for (unsigned int x = 0; x < 3; x++) {
			for (unsigned int y = 0; y < 3; y++) {
				normalMatricesDataRaw.push_back(0);
			}
		}

		visibleData.push_back(1);
	}

	vboMatricesData = new VBO<GLfloat>(GL_ARRAY_BUFFER, numObjects * 16 * sizeof(GLfloat), matricesDataRaw, GL_STREAM_DRAW, true);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix"), 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 1, 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 2, 4, 1);
	vboMatricesData->addAttribute(shader->getAttributeLocation("ModelMatrix") + 3, 4, 1);
	renderData->addVBO(vboMatricesData);

	if (useLighting) {
		vboNormalMatricesData = new VBO<GLfloat>(GL_ARRAY_BUFFER, numObjects * 9 * sizeof(GLfloat), normalMatricesDataRaw, GL_STREAM_DRAW, true);
		vboNormalMatricesData->addAttribute(shader->getAttributeLocation("NormalMatrix"), 3, 1);
		vboNormalMatricesData->addAttribute(shader->getAttributeLocation("NormalMatrix") + 1, 3, 1);
		vboNormalMatricesData->addAttribute(shader->getAttributeLocation("NormalMatrix") + 2, 3, 1);
		renderData->addVBO(vboNormalMatricesData);
	}

	vboVisibleData = new VBO<GLfloat>(GL_ARRAY_BUFFER, numObjects * sizeof(GLfloat), visibleData, GL_STREAM_DRAW, true);
	vboVisibleData->addAttribute(shader->getAttributeLocation("Visible"), 1, 1);
	renderData->addVBO(vboVisibleData);

	renderData->add(ShaderInterface::UBO_BINDING_LOCATION_MATERIAL, Renderer::getShaderInterface()->getUBO(ShaderInterface::BLOCK_MATERIAL));

	for (Material* material : mesh->getMaterials())
		renderData->addTextureSet(material->getTextureSet());

	renderData->setup(renderShader);
	renderData->setNumInstances(numObjects);
}

GameRenderer::~GameRenderer() {
	delete renderData;
	delete vboVerticesData;
	delete vboIndices;
	delete vboMatricesData;
	if (useLighting)
		delete vboNormalMatricesData;
	delete mesh;

	for (unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();
}

void GameRenderer::show(unsigned int index) {
	visibleData[index] = 1.0f;
	vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
}

void GameRenderer::hide(unsigned int index) {
	visibleData[index] = 0.0f;
	vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
}

void GameRenderer::update(unsigned int startIndex, unsigned int endIndex) {
	for (unsigned int i = startIndex; i < endIndex; i++) {
		matricesData[i].initIdentity();
		matricesData[i].translate(objects[i]->getPosition());
		matricesData[i].rotate(objects[i]->getRotation());
		matricesData[i].scale(objects[i]->getScale());

		int pos = i * 16;

		for (unsigned int x = 0; x < 4; x++) {
			for (unsigned int y = 0; y < 4; y++) {
				matricesDataRaw[pos] = matricesData[i].get(y, x);
				pos += 1;
			}
		}

		if (useLighting) {
			normalMatricesData[i] = matricesData[i].to3x3().inverse().transpose();
			pos = i * 9;
			for (unsigned int x = 0; x < 3; x++) {
				for (unsigned int y = 0; y < 3; y++) {
					normalMatricesDataRaw[pos] = normalMatricesData[i].get(y, x);
					pos += 1;
				}
			}
		}
	}
	vboMatricesData->updateStream(numObjects * 16 * sizeof(GLfloat));
	if (useLighting)
		vboNormalMatricesData->updateStream(numObjects * 9 * sizeof(GLfloat));
}

void GameRenderer::render() {
	//Use the shader
	shader->use();
	Renderer::saveTextures();
	shader->setUniformMatrix4("ViewProjectionMatrix", Renderer::getCamera()->getProjectionViewMatrix());
	Renderer::useMaterial(renderData, mesh->getData()->getSubData(0).materialIndex, mesh->getMaterial(mesh->getData()->getSubData(0).materialIndex), renderData->getUBO(ShaderInterface::BLOCK_MATERIAL));
	if (useLighting) {
		shader->setUniformVector3("Light_Direction", Vector3f(0.0f, -1.0f, -1.0f));
		shader->setUniformColourRGB("Light_DiffuseColour", Colour(1.0f, 1.0f, 1.0f));
		shader->setUniformColourRGB("Light_SpecularColour", Colour(1.0f, 1.0f, 1.0f));
		shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());
	}
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	renderData->render();
	glDisable(GL_CULL_FACE);
	Renderer::stopUsingMaterial(mesh->getMaterial(mesh->getData()->getSubData(0).materialIndex));
	Renderer::releaseNewTextures();
	shader->stopUsing();
}

void GameRenderer::showAll() {
	//Go through all of the visible data
	for (unsigned int i = 0; i < visibleData.size(); i++)
		visibleData[i] = 1.0f;
	vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
}

void GameRenderer::hideAll() {
	//Go through all of the visible data
	for (unsigned int i = 0; i < visibleData.size(); i++)
		visibleData[i] = 0.0f;
	vboVisibleData->updateStream(numObjects * sizeof(GLfloat));
}


