/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#include "Surface.h"

Surface::Surface(unsigned int meshSize) {
	this->width = meshSize;
	this->height = meshSize;
	//Create and assign the mesh
	Mesh* terrainMesh = new Mesh(createMeshData(width, height));
	setMesh(terrainMesh, Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
}

float Surface::f(float x, float y, float t) {
	return sin(x) - x*x + cos(y*t) + sqrt(x*x + y*y + 1);
}

void Surface::updateHeights(float time) {
	MeshData* meshData = getMesh()->getData();

	std::vector<float>& meshPositions = meshData->getPositions();

	for (unsigned int y = 0; y <= height; ++y) {
		for (unsigned int x = 0; x <= width; ++x) {
			unsigned int index = ((y * width) + x) * 3;
			meshPositions[index + 1] = f(meshPositions[index], meshPositions[index + 2], time);
		}
	}
	getMesh()->getRenderData()->updatePositions(meshData);
}

MeshData* Surface::createMeshData(unsigned int width, unsigned int height) {
	//The mesh data
	MeshData* meshData = new MeshData(3, MeshData::SEPARATE_POSITIONS);
	meshData->setRenderMode(GL_TRIANGLES);

	//References to the positions and indices vectors
	std::vector<float>&        meshPositions = meshData->getPositions();
	std::vector<unsigned int>& meshIndices   = meshData->getIndices();

	//Assign the positions
	for (unsigned int y = 0; y <= height; y++) {
		for (unsigned int x = 0; x <= width; x++) {
			meshPositions.push_back(((float) x - (float) width / 2.0f) / (float) (width / 10.0f));
			meshPositions.push_back(0.0f);
			meshPositions.push_back(((float) y - (float) height / 2.0f) / (float) (height / 10.0f));
		}
	}

	//Assign the indices
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
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
