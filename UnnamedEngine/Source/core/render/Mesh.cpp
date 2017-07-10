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

#include "Mesh.h"

#include "Renderer.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The MeshData class
 *****************************************************************************/

Sphere MeshData::calculateBoundingSphere() {
	//The bounding sphere
	Sphere sphere;
	//Calculate the find the lengths between the mesh, and also find the largest one
	float lengthX = maxX - minX;
	float lengthY = maxY - minY;
	float lengthZ = maxZ - minZ;
	float largestLength = Vector3f(lengthX, lengthY, lengthZ).length();

	//Calculate the centre and radius of the bound sphere
	sphere.centre = Vector3f((maxX + minX) / 2.0f, (maxY + minY) / 2.0f, (maxZ + minZ) / 2.0f);
	sphere.radius = largestLength / 2.0f;

	//Return the sphere
	return sphere;
}

void MeshData::addPosition(Vector2f position) {
	//Check to see whether it should be separated
	if (separatePositions()) {
		positions.push_back(position.getX());
		positions.push_back(position.getY());
	} else {
		others.push_back(position.getX());
		others.push_back(position.getY());
	}

	numPositions++;
}

void MeshData::addPosition(Vector3f position) {
	//Check to see whether it should be separated
	if (separatePositions()) {
		positions.push_back(position.getX());
		positions.push_back(position.getY());
		positions.push_back(position.getZ());
	} else {
		others.push_back(position.getX());
		others.push_back(position.getY());
		others.push_back(position.getZ());
	}

	//Check whether data for a bounding sphere should be calculated
	if (numDimensions == 3) {
		minX = utils_maths::min(minX, position.getX());
		maxX = utils_maths::max(maxX, position.getX());
		minY = utils_maths::min(minY, position.getY());
		maxY = utils_maths::max(maxY, position.getY());
		minZ = utils_maths::min(minZ, position.getZ());
		maxZ = utils_maths::max(maxZ, position.getZ());
	}

	numPositions++;
}

void MeshData::addColour(Colour colour) {
	//Check to see whether it should be separated
	if (separateColours()) {
		colours.push_back(colour.getR());
		colours.push_back(colour.getG());
		colours.push_back(colour.getB());
		colours.push_back(colour.getA());
	} else {
		others.push_back(colour.getR());
		others.push_back(colour.getG());
		others.push_back(colour.getB());
		others.push_back(colour.getA());
	}

	numColours++;
}

void MeshData::addTextureCoord(Vector2f textureCoord) {
	//Check to see whether it should be separated
	if (separateTextureCoords()) {
		textureCoords.push_back(textureCoord.getX());
		textureCoords.push_back(textureCoord.getY());
	} else {
		others.push_back(textureCoord.getX());
		others.push_back(textureCoord.getY());
	}

	numTextureCoords++;
}

void MeshData::addNormal(Vector3f normal) {
	//Check to see whether it should be separated
	if (separateNormals()) {
		normals.push_back(normal.getX());
		normals.push_back(normal.getY());
		normals.push_back(normal.getZ());
	} else {
		others.push_back(normal.getX());
		others.push_back(normal.getY());
		others.push_back(normal.getZ());
	}

	numNormals++;
}

void MeshData::addTangent(Vector3f tangent) {
	//Check to see whether it should be separated
	if (separateTangents()) {
		tangents.push_back(tangent.getX());
		tangents.push_back(tangent.getY());
		tangents.push_back(tangent.getZ());
	} else {
		others.push_back(tangent.getX());
		others.push_back(tangent.getY());
		others.push_back(tangent.getZ());
	}

	numTangents++;
}

void MeshData::addBitangent(Vector3f bitangent) {
	//Check to see whether it should be separated
	if (separateBitangents()) {
		bitangents.push_back(bitangent.getX());
		bitangents.push_back(bitangent.getY());
		bitangents.push_back(bitangent.getZ());
	} else {
		others.push_back(bitangent.getX());
		others.push_back(bitangent.getY());
		others.push_back(bitangent.getZ());
	}

	numBitangents++;
}

void MeshData::addBoneData(unsigned int boneID, float boneWeight) {
	boneIDs.push_back(boneID);
	boneWeights.push_back(boneWeight);
	numBones++;
}

/*****************************************************************************
 * The MeshRenderData class
 *****************************************************************************/

void MeshRenderData::setup(MeshData* data, RenderShader* renderShader) {
	//Assign the shader used during the setup
	this->setupShader = renderShader;
	//The shader used for the setup
	Shader* shader = renderShader->getShader();
	//Determine the number of vertices
	if (data->hasIndices()) {
		//As the data has indices, this will determine the number of vertices
		numVertices = data->getNumIndices();
		hasIndices = true;
	} else {
		//As there are no indices, the number of positions will be the number of vertices
		numVertices = data->getNumPositions();
		hasIndices = false;
	}

	//Create the RenderData instance
	renderData = new RenderData(data->getRenderMode(), numVertices);

	//Setup positions
	if (data->hasPositions() && data->separatePositions()) {
		vboPositions = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getPositions().size() * sizeof(data->getPositions()[0]), data->getPositions(), usagePositions);
		vboPositions->addAttribute(shader->getAttributeLocation("Position"), data->getNumDimensions());
		renderData->addVBO(vboPositions);
	}

	//Setup colours
	if (data->hasColours() && data->separateColours()) {
		vboColours = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getColours().size() * sizeof(data->getColours()[0]), data->getColours(), usageColours);
		vboColours->addAttribute(shader->getAttributeLocation("Colour"), 4);
		renderData->addVBO(vboColours);
	}

	//Setup texture coordinates
	if (data->hasTextureCoords() && data->separateTextureCoords()) {
		vboTextureCoords = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getTextureCoords().size() * sizeof(data->getTextureCoords()[0]), data->getTextureCoords(), usageTextureCoords);
		vboTextureCoords->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);
		renderData->addVBO(vboTextureCoords);
	}

	//Setup normals
	if (data->hasNormals() && data->separateNormals()) {
		vboNormals = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getNormals().size() * sizeof(data->getNormals()[0]), data->getNormals(), usageNormals);
		vboNormals->addAttribute(shader->getAttributeLocation("Normal"), 3);
		renderData->addVBO(vboNormals);
	}

	//Setup tangents
	if (data->hasTangents() && data->separateTangents()) {
		vboTangents = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getTangents().size() * sizeof(data->getTangents()[0]), data->getTangents(), usageTangents);
		vboTangents->addAttribute(shader->getAttributeLocation("Tangent"), 3);
		renderData->addVBO(vboTangents);
	}

	//Setup bitangents
	if (data->hasBitangents() && data->separateBitangents()) {
		vboBitangents = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getBitangents().size() * sizeof(data->getBitangents()[0]), data->getBitangents(), usageBitangents);
		vboBitangents->addAttribute(shader->getAttributeLocation("Bitangent"), 3);
		renderData->addVBO(vboBitangents);
	}

	//Setup bones
	if (data->hasBones()) {
		vboBoneIDs = new VBO<unsigned int>(GL_ARRAY_BUFFER, data->getBoneIDs().size() * sizeof(data->getBoneIDs()[0]), data->getBoneIDs(), GL_STATIC_DRAW);
		vboBoneIDs->addAttribute(shader->getAttributeLocation("BoneIDs"), 4);
		renderData->addVBO(vboBoneIDs);

		vboBoneWeights = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getBoneWeights().size() * sizeof(data->getBoneWeights()[0]), data->getBoneWeights(), GL_STATIC_DRAW);
		vboBoneWeights->addAttribute(shader->getAttributeLocation("BoneWeights"), 4);
		renderData->addVBO(vboBoneWeights);
	}

	//Check to see whether the 'other' VBO is required
	if (data->hasOthers()) {

		vboOthers = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getOthers().size() * sizeof(data->getOthers()[0]), data->getOthers(), usageOthers);

		if (data->hasPositions() && ! data->separatePositions())
			vboOthers->addAttribute(shader->getAttributeLocation("Position"), data->getNumDimensions());

		if (data->hasColours() && ! data->separateColours())
			vboOthers->addAttribute(shader->getAttributeLocation("Colour"), 4);

		if (data->hasTextureCoords() && ! data->separateTextureCoords())
			vboOthers->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);

		if (data->hasNormals() && ! data->separateNormals())
			vboOthers->addAttribute(shader->getAttributeLocation("Normal"), 3);

		if (data->hasTangents() && ! data->separateTangents())
			vboOthers->addAttribute(shader->getAttributeLocation("Tangent"), 3);

		if (data->hasBitangents() && ! data->separateBitangents())
			vboOthers->addAttribute(shader->getAttributeLocation("Bitangent"), 3);

		renderData->addVBO(vboOthers);
	}

	//Check to see whether indices are needed
	if (data->hasIndices()) {
		vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, data->getNumIndices() * sizeof(data->getIndices()[0]), data->getIndices(), usageIndices);
		renderData->setIndicesVBO(vboIndices);
	}

	//Setup the render data
	renderData->setup();
}

void MeshRenderData::render() {
	renderData->render();
}

void MeshRenderData::updatePositions(MeshData* data) {
	//Determine the number of vertices
	if (data->hasIndices()) {
		//As the data has indices, this will determine the number of vertices
		numVertices = data->getNumIndices();
		hasIndices = true;
	} else {
		//As there are no indices, the number of positions will be the number of vertices
		numVertices = data->getNumPositions();
		hasIndices = false;
	}

	glBindVertexArray(renderData->getVAO());

	vboPositions->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateColours() {
	glBindVertexArray(renderData->getVAO());

	vboColours->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateTextureCoords() {
	glBindVertexArray(renderData->getVAO());

	vboTextureCoords->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateNormals() {
	glBindVertexArray(renderData->getVAO());

	vboNormals->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateTangents() {
	glBindVertexArray(renderData->getVAO());

	vboTangents->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateBitangents() {
	glBindVertexArray(renderData->getVAO());

	vboBitangents->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateIndices(MeshData* data) {
	//As the data has indices, this will determine the number of vertices
	numVertices = data->getNumIndices();
	hasIndices = true;

	renderData->setCount(numVertices);

	glBindVertexArray(renderData->getVAO());

	vboIndices->update();

	glBindVertexArray(0);
}

void MeshRenderData::destroy() {
	delete renderData;
	delete vboPositions;
	delete vboColours;
	delete vboTextureCoords;
	delete vboNormals;
	delete vboTangents;
	delete vboBitangents;
	delete vboIndices;
}

/*****************************************************************************
 * The Mesh class
 *****************************************************************************/

Mesh::Mesh(MeshData* data) {
	this->data = data;
	//Add the default material
	this->addMaterial(new Material());

	transform.setIdentity();
}

Mesh::~Mesh() {
	//Delete the created resources
	if (hasMaterial()) {
		for (Material* material : materials)
			delete material;
		materials.clear();
	}
	delete renderData;
	delete data;
}

void Mesh::updateAnimation(float deltaSeconds) {
	//Ensure there is a skeleton with an animation
	if (skeleton)
		//Update the skeleton
		skeleton->update(deltaSeconds);
	else
		Logger::log("No skeleton instance to update", "Mesh", LogType::Error);
}

/*****************************************************************************
 * The MeshBuilder class
 *****************************************************************************/

/* 2D Stuff */

MeshData* MeshBuilder::createTriangle(Vector2f v1, Vector2f v2, Vector2f v3, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);

	data->addPosition(v1);
	data->addPosition(v2);
	data->addPosition(v3);

	return data;
}

MeshData* MeshBuilder::createQuad(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, v1, v2, v3, v4);
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad(float width, float height, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, Vector2f(0, 0), Vector2f(width, 0), Vector2f(width, height), Vector2f(0, height));
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad(float width, float height, Texture* texture, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, Vector2f(0, 0), Vector2f(width, 0), Vector2f(width, height), Vector2f(0, height), texture);
	addQuadI(data);
	return data;
}

void MeshBuilder::addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	data->addPosition(v1);
	data->addPosition(v2);
	data->addPosition(v3);
	data->addPosition(v4);
}

void MeshBuilder::addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture) {
	data->addPosition(v1);
	data->addTextureCoord(Vector2f(0, 0));
	data->addPosition(v2);
	data->addTextureCoord(Vector2f(1, 0));
	data->addPosition(v3);
	data->addTextureCoord(Vector2f(1, 1));
	data->addPosition(v4);
	data->addTextureCoord(Vector2f(0, 1));
}

void MeshBuilder::addQuadI(MeshData* data) {
	data->addIndex(0);
	data->addIndex(1);
	data->addIndex(2);
	data->addIndex(3);
	data->addIndex(0);
	data->addIndex(2);
}

void MeshBuilder::addQuadT(MeshData* data, float top, float left, float bottom, float right) {
	data->addTextureCoord(Vector2f(left, top));
	data->addTextureCoord(Vector2f(right, top));
	data->addTextureCoord(Vector2f(right, bottom));
	data->addTextureCoord(Vector2f(left, bottom));
}

/* 3D Stuff */

MeshData* MeshBuilder::createQuad3D(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, v1, v2, v3, v4);
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad3D(float width, float height, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, Vector2f(-width / 2, -height / 2), Vector2f(width / 2, -height / 2), Vector2f(width / 2, height / 2), Vector2f(-width / 2, height / 2));
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad3D(float width, float height, Texture* texture, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, Vector2f(-width / 2, -height / 2), Vector2f(width / 2, -height / 2), Vector2f(width / 2, height / 2), Vector2f(-width / 2, height / 2), texture);
	addQuadI(data);
	return data;
}

void MeshBuilder::addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	data->addPosition(Vector3f(v1));
	data->addPosition(Vector3f(v2));
	data->addPosition(Vector3f(v3));
	data->addPosition(Vector3f(v4));
}

void MeshBuilder::addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture) {
	data->addPosition(Vector3f(v1));
	data->addTextureCoord(Vector2f(0, 0));
	data->addPosition(Vector3f(v2));
	data->addTextureCoord(Vector2f(1, 0));
	data->addPosition(Vector3f(v3));
	data->addTextureCoord(Vector2f(1, 1));
	data->addPosition(Vector3f(v4));
	data->addTextureCoord(Vector2f(0, 1));
}

MeshData* MeshBuilder::createCube(float width, float height, float depth, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addCubeData(data, width, height, depth);
	addCubeI(data);
	return data;
}

void MeshBuilder::addCubeData(MeshData* data, float width, float height, float depth) {
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	//Front face
	data->addPosition(Vector3f(-w, h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(-w, -h, d));

	//Left face
	data->addPosition(Vector3f(-w, -h, d));
	data->addPosition(Vector3f(-w, -h, -d));
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(-w, h, d));

	//Back face
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(w, h, -d));
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(-w, -h, -d));

	//Bottom face
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(-w, -h, d));
	data->addPosition(Vector3f(-w, -h, -d));

	//Right face
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, h, -d));

	//Top face
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(-w, h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, h, -d));
}

void MeshBuilder::addCubeI(MeshData* data) {
	//Front face
	//B-L triangle
	data->addIndex(0); data->addIndex(1); data->addIndex(2);
	//T-R
	data->addIndex(2); data->addIndex(3); data->addIndex(0);

	//Left face
	//B-L triangle
	data->addIndex(4); data->addIndex(5); data->addIndex(6);
	//T-R
	data->addIndex(6); data->addIndex(7); data->addIndex(4);

	//Back face
	//B-L triangle
	data->addIndex(8); data->addIndex(9); data->addIndex(10);
	//T-R
	data->addIndex(10); data->addIndex(11); data->addIndex(8);

	//Bottom face
	//B-L triangle
	data->addIndex(12); data->addIndex(13); data->addIndex(14);
	//T-R
	data->addIndex(14); data->addIndex(15); data->addIndex(12);

	//Right face
	//B-L triangle
	data->addIndex(16); data->addIndex(17); data->addIndex(18);
	//T-R
	data->addIndex(18); data->addIndex(19); data->addIndex(16);

	//Top face
	//B-L triangle
	data->addIndex(20); data->addIndex(21); data->addIndex(22);
	//T-R
	data->addIndex(22); data->addIndex(23); data->addIndex(20);
}
