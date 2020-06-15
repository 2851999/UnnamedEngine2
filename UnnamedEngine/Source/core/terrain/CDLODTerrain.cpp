/*****************************************************************************
 *
 *   Copyright 2017 Joel Davies
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

#include "CDLODTerrain.h"

#include "../../utils/GLUtils.h"

#include "../render/Light.h"

/*****************************************************************************
 * The CDLODTerrain class
 *****************************************************************************/

CDLODTerrain::CDLODTerrain(std::string heightMapPath, int lodDepth, float meshSize) : CDLODTerrain(new CDLODHeightMap(heightMapPath), lodDepth, meshSize) {}

CDLODTerrain::CDLODTerrain(CDLODHeightMap* heightMap, int lodDepth, float meshSize) {
	//Create the height map
	this->heightMap = heightMap;
	this->lodDepth = lodDepth;
	this->meshSize = meshSize;


	//Calculate what the leafNodeSize should be for the given lodDepth and size the map should be
	float rootNodeSize = heightMap->getSize();
	leafNodeSize = rootNodeSize / pow(2, lodDepth - 1);

	//Setup the ranges
	ranges.push_back(leafNodeSize * RANGE_MULTIPLIER);
	for (int i = 1; i < lodDepth; i++) {
		ranges.push_back((ranges[i - 1] + pow(2, i) * leafNodeSize) * RANGE_MULTIPLIER); //Multiplier resolves clamping issue
	}

	//Calculate the root node size (OLD)
	//float rootNodeSize = leafNodeSize * pow(2, lodDepth - 1);

	//float rootNodeSize = heightMap->getSize();

//	std::cout << rootNodeSize << std::endl;

	//Create the quad-tree
	root = new CDLODQuadTreeNode(heightMap, meshSize, rootNodeSize, lodDepth - 1, 0.0f, 0.0f);

	//Create and assign the mesh
	Mesh* terrainMesh = new Mesh(createMeshData(meshSize, meshSize));
	setMesh(terrainMesh, Renderer::getRenderShader(Renderer::SHADER_TERRAIN));

//	texture1 = Texture::loadTexture("C:/UnnamedEngine/textures/grass.png");
//	texture2 = Texture::loadTexture("C:/UnnamedEngine/textures/snow.jpg");
//	texture3 = Texture::loadTexture("C:/UnnamedEngine/textures/stone.jpg");
}

CDLODTerrain::~CDLODTerrain() {
	delete heightMap;
	delete root;
}

void CDLODTerrain::update() {
	GameObject3D::update();

	//Get the camera to use
	Camera3D* camera = (Camera3D*)Renderer::getCamera();

	//Select the nodes to render
	root->LODSelect(ranges, lodDepth - 1, camera, selectionList);

	//Go through an update all selected nodes
	for (unsigned int i = 0; i < selectionList.size(); ++i)
		selectionList[i]->update();
}

void CDLODTerrain::render() {

//	utils_gl::enableWireframe();
//	for (CDLODQuadTreeNode* node : selectionList)
//		node->debug();
//	utils_gl::disableWireframe();

//	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, -1.0f, 0.0001f);
//	light0->setUniforms(shader, "[0]");
//	delete light0;

//	long numPolygons = (selectionList.size() * mesh->getData()->getNumIndices()) / 3;
//	std::cout << numPolygons << std::endl;

	//Go through each node to render
	while (! selectionList.empty()) {
		//Get the current node and remove it from the list
		CDLODQuadTreeNode* currentNode = selectionList[selectionList.size() - 1];
		selectionList.pop_back();

		currentNode->getDescriptorSet()->bind();

//		std::cout << currentNode->getSize() << std::endl;
//		std::cout << currentNode->getRange() << std::endl;

		Renderer::render(getMesh(), getModelMatrix(), getRenderShader());
	}
}

MeshData* CDLODTerrain::createMeshData(int width, int height) {
	//The mesh data
	MeshData* meshData = new MeshData(3, MeshData::SEPARATE_POSITIONS);
	meshData->setRenderMode(GL_TRIANGLES);

	//References to the positions and indices vectors
	std::vector<float>&        meshPositions = meshData->getPositions();
	std::vector<unsigned int>& meshIndices   = meshData->getIndices();

	//Assign the positions
	for (int y = 0; y <= height; y++) {
		for (int x = 0; x <= width; x++) {
			meshPositions.push_back(((float) x - (float) width / 2.0f) / (float) width);
			meshPositions.push_back(0.0f);
			meshPositions.push_back(((float) y - (float) height / 2.0f) / (float) height);
		}
	}

	//Assign the indices
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			meshIndices.push_back((width + 1) * (y + 1) + (x + 1));
			meshIndices.push_back((width + 1) * y + (x + 1));
			meshIndices.push_back((width + 1) * (y + 0) + x);

			meshIndices.push_back((width + 1) * y + x);
			meshIndices.push_back((width + 1) * (y + 1) + x);
			meshIndices.push_back((width + 1) * (y + 1) + (x + 1));
		}
	}

	meshData->setNumPositions(meshPositions.size());
	meshData->setNumIndices(meshIndices.size());

	//Return the mesh
	return meshData;
}
