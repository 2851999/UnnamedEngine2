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

#include "HeightMapTerrain.h"

#include "../../core/render/Mesh.h"
#include "../../core/render/Renderer.h"


//https://www.3dgep.com/multi-textured-terrain-in-opengl/

inline float getHeightValue(const unsigned char* data, unsigned int numBytes) {
	switch (numBytes) {
	case 1:
		return (unsigned char) (data[0]) / (float) 0xff;
		break;
	case 2:
		return (unsigned short) (data[1] << 8 | data[0]) / (float) 0xffff;
		break;
	case 4:
		return (unsigned int) (data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float) 0xffffffff;
		break;
	}
	return 0.0f;
}

void HeightMapTerrain::setup(std::string filePath, unsigned int bitsPerPixel) {
	MeshData* meshData = new MeshData(3, MeshData::SEPARATE_POSITIONS); //| MeshData::SEPARATE_NORMALS);
	meshData->setRenderMode(GL_TRIANGLES);

	//The data needed for the texture
	int numComponents, width, height;
	//Obtain the texture data
	unsigned char* image = Texture::loadTexture(filePath, numComponents, width, height, false);

	//Various terrain values
	const unsigned int bytesPerPixel = (bitsPerPixel / 8);
	float blockScale = 2.0f;
	float heightScale = 100.0f;
	float terrainWidth = (width - 1) * blockScale;
	float terrainHeight = (height - 1) * blockScale;
	float halfTerrainWidth = terrainWidth * 0.5f;
	float halfTerrainHeight = terrainHeight * 0.5f;
	unsigned int numVertices = width * height;
	unsigned int numTriangles = (width - 1) * (height - 1) * 2;
	unsigned int numIndices = numTriangles * 3;

	//References to the positions and indices vectors
	std::vector<float>&        meshPositions = meshData->getPositions();
	//std::vector<float>&        meshNormals   = meshData->getNormals();
	std::vector<unsigned int>& meshIndices   = meshData->getIndices();

	meshPositions.resize(numVertices * 3);
	meshData->setNumPositions(numVertices);

	//Texture coordinates, and vertex coordinates
	float s, t, x, y, z;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			unsigned int index = (j * width) + i;
			float heightValue = getHeightValue(&image[index * bytesPerPixel * numComponents], bytesPerPixel);

			s = (i / (float)(width - 1));
			t = (j / (float)(height - 1));

			x = (s * terrainWidth) - halfTerrainWidth;
			y = heightValue * heightScale;
			z = (t * terrainHeight) - halfTerrainHeight;

			float pos = index * 3;

			meshPositions[pos] = x;
			meshPositions[pos + 1] = y;
			meshPositions[pos + 2] = z;
		}
	}

	meshIndices.resize(numIndices);
	meshData->setNumIndices(numIndices);

	unsigned int index = 0;
	for (int j = 0; j < (height - 1); j++) {
		for (int i = 0; i < (width - 1); i++) {
			int vertexIndex = (j * width) + i;
			meshIndices[index++] = vertexIndex;
			meshIndices[index++] = vertexIndex + width + 1;
			meshIndices[index++] = vertexIndex + 1;

			meshIndices[index++] = vertexIndex;
			meshIndices[index++] = vertexIndex + width;
			meshIndices[index++] = vertexIndex + width + 1;
		}
	}

//	meshNormals.resize(numVertices* 3);
//	meshData->setNumNormals(numVertices);
//
//	for (unsigned int i = 0; i < meshIndices.size(); i += 3) {
//		Vector3f v0, v1, v2;
//		v0 = Vector3f(meshPositions[meshIndices[i + 0] * 3], meshPositions[meshIndices[i + 0] * 3 + 1], meshPositions[meshIndices[i + 0] * 3 + 2]);
//		v1 = Vector3f(meshPositions[meshIndices[i + 1] * 3], meshPositions[meshIndices[i + 1] * 3 + 1], meshPositions[meshIndices[i + 1] * 3 + 2]);
//		v2 = Vector3f(meshPositions[meshIndices[i + 2] * 3], meshPositions[meshIndices[i + 2] * 3 + 1], meshPositions[meshIndices[i + 2] * 3 + 2]);
//
//		Vector3f c1 = v1 - v0;
//		Vector3f c2 = v2 - v0;
//
//		Vector3f normal = c1.cross(c2).normalised();
//
//		meshNormals[meshIndices[i + 0] * 3] += normal.getX();
//		meshNormals[meshIndices[i + 0] * 3 + 1] += normal.getY();
//		meshNormals[meshIndices[i + 0] * 3 + 2] += normal.getZ();
//
//		meshNormals[meshIndices[i + 1] * 3] += normal.getX();
//		meshNormals[meshIndices[i + 1] * 3 + 1] += normal.getY();
//		meshNormals[meshIndices[i + 1] * 3 + 2] += normal.getZ();
//
//		meshNormals[meshIndices[i + 2] * 3] += normal.getX();
//		meshNormals[meshIndices[i + 2] * 3 + 1] += normal.getY();
//		meshNormals[meshIndices[i + 2] * 3 + 2] += normal.getZ();
//    }

	//Release the texture
	Texture::freeTexture(image);

	//Assign the new mesh
	setMesh(new Mesh(meshData), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
}
