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

/*****************************************************************************
 * The CDLODTerrain class
 *****************************************************************************/

CDLODTerrain::CDLODTerrain(std::string heightMapPath) {
	//Create the height map
	heightMap = new CDLODHeightMap(heightMapPath);

	//Setup the ranges
	for (int i = 0; i < lodDepth; i++) {
		ranges.push_back(pow(2, i));
	}

	//Calculate the root node size
	//float rootNodeSize = leafNodeSize * pow(2, lodDepth - 1);
	float rootNodeSize = heightMap->getSize();

	//Create the quad-tree
	root = new CDLODQuadTreeNode(heightMap, rootNodeSize, lodDepth - 1, 0.0f, 0.0f);

	//Assign the shader
	terrainShader = Renderer::getRenderShader(Renderer::SHADER_CDLOD_TERRAIN);

	//Create the mesh
	mesh = new Mesh(createMeshData(heightMap->getSize(), heightMap->getSize()));
	mesh->setup(terrainShader);
}

CDLODTerrain::~CDLODTerrain() {
	delete heightMap;
	delete root;
	delete mesh;
}

void CDLODTerrain::render() {
	//Get the camera to use
	Camera3D* camera = (Camera3D*) Renderer::getCamera();

	//Select the nodes to render
	std::vector<CDLODQuadTreeNode*> selectionList;
	root->LODSelect(ranges, lodDepth - 1, camera, selectionList);

	//Use the terrain shader
	Shader* shader = terrainShader->getShader();
	shader->use();

	shader->setUniformMatrix4("ProjectionMatrix", camera->getProjectionMatrix());
	shader->setUniformMatrix4("ViewMatrix", camera->getViewMatrix());
	shader->setUniformVector3("CameraPosition", camera->getPosition());

	shader->setUniformi("HeightMap", Renderer::bindTexture(heightMap->getTexture()));

	//Go through each node to render
	while (! selectionList.empty()) {
		//Get the current node and remove it from the list
		CDLODQuadTreeNode* currentNode = selectionList[selectionList.size() - 1];
		selectionList.pop_back();

		shader->setUniformVector3("Translation", Vector3f(currentNode->getX(), 0.0f, currentNode->getZ()));
		shader->setUniformf("Scale", currentNode->getSize());
		shader->setUniformf("Range", currentNode->getRange());
		shader->setUniformVector2("GridSize", Vector2f(256, 256));

		//std::cout << currentNode->getSize() << std::endl;

		mesh->getRenderData()->render();
	}

	//Stop using the shader
	shader->stopUsing();
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
			meshIndices.push_back((width + 1) * (y + 0) + x);
			meshIndices.push_back((width + 1) * y + (x + 1));
			meshIndices.push_back((width + 1) * (y + 1) + (x + 1));

			meshIndices.push_back((width + 1) * (y + 1) + (x + 1));
			meshIndices.push_back((width + 1) * (y + 1) + x);
			meshIndices.push_back((width + 1) * y + x);
		}
	}

	meshData->setNumPositions(meshPositions.size());
	meshData->setNumIndices(meshIndices.size());

	//Return the mesh
	return meshData;
}
