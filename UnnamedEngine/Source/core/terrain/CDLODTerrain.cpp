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
	root = new CDLODQuadTreeNode(heightMap, rootNodeSize, lodDepth - 1, 0.0f, 0.0f);

	//Create and assign the mesh
	Mesh* terrainMesh = new Mesh(createMeshData(meshSize, meshSize));
	setMesh(terrainMesh, Renderer::getRenderShader(Renderer::SHADER_TERRAIN));

//	texture1 = Texture::loadTexture("C:/UnnamedEngine/textures/grass.png");
//	texture2 = Texture::loadTexture("C:/UnnamedEngine/textures/snow.jpg");
//	texture3 = Texture::loadTexture("C:/UnnamedEngine/textures/stone.jpg");

	//Assign the UBO data
	shaderTerrainUBO =  Renderer::getShaderInterface()->getUBO(ShaderInterface::BLOCK_TERRAIN);
	shaderTerrainData.ue_heightScale = heightMap->getHeightScale();
	shaderTerrainData.ue_size = heightMap->getSize();
}

CDLODTerrain::~CDLODTerrain() {
	delete heightMap;
	delete root;
}

void CDLODTerrain::render() {
	//Get the camera to use
	Camera3D* camera = (Camera3D*) Renderer::getCamera();

	//Select the nodes to render
	std::vector<CDLODQuadTreeNode*> selectionList;
	root->LODSelect(ranges, lodDepth - 1, camera, selectionList);

//	utils_gl::enableWireframe();
//	for (CDLODQuadTreeNode* node : selectionList)
//		node->debug();
//	utils_gl::disableWireframe();

	//Use the terrain shader
	Shader* shader = getShader();
	shader->use();

	shader->setUniformi("HeightMap", Renderer::bindTexture(heightMap->getTexture()));

//	shader->setUniformi("NumLights", 1);
//	shader->setUniformColourRGB("Light_Ambient", Colour(0.1f, 0.1f, 0.1f));
	Renderer::getShaderBlock_Core().ue_cameraPosition = Vector4f(((Camera3D*) Renderer::getCamera())->getPosition(), 0.0f); //Required unless added to RenderScene3D?
//	shader->setUniformi("UseEnvironmentMap", 0);
//	shader->setUniformi("UseShadowMap", 0);
//
//	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, -1.0f, 0.0001f);
//	light0->setUniforms(shader, "[0]");
//	delete light0;

//	shader->setUniformi("GrassTexture", Renderer::bindTexture(texture1));
//	shader->setUniformi("SnowTexture", Renderer::bindTexture(texture2));
//	shader->setUniformi("StoneTexture", Renderer::bindTexture(texture3));

//	long numPolygons = (selectionList.size() * mesh->getData()->getNumIndices()) / 3;
//	std::cout << numPolygons << std::endl;

	//Go through each node to render
	while (! selectionList.empty()) {
		//Get the current node and remove it from the list
		CDLODQuadTreeNode* currentNode = selectionList[selectionList.size() - 1];
		selectionList.pop_back();

		shaderTerrainData.ue_translation = Vector4f(currentNode->getX(), 0.0f, currentNode->getZ(), 0.0f);
		shaderTerrainData.ue_scale = currentNode->getSize();
		shaderTerrainData.ue_range = currentNode->getRange();
		shaderTerrainData.ue_gridSize = Vector2f(meshSize, meshSize);

		shaderTerrainUBO->update(&shaderTerrainData, 0, sizeof(ShaderBlock_Terrain));

//		std::cout << currentNode->getSize() << std::endl;
//		std::cout << currentNode->getRange() << std::endl;

		//mesh->getRenderData()->render();
		Renderer::render(getMesh(), getModelMatrix(), getRenderShader());
	}

//	Renderer::unbindTexture();
//	Renderer::unbindTexture();
//	Renderer::unbindTexture();
	Renderer::unbindTexture();

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
