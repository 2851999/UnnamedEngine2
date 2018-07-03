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

#include "Tilemap.h"

#include "Renderer.h"

/*****************************************************************************
 * The Tilemap class
 *****************************************************************************/

Tilemap::Tilemap(TextureAtlas* tileset, unsigned int rows, unsigned int columns, std::vector<unsigned int> &data) : tileset(tileset), mapRows(rows), mapColumns(columns), data(data) {
	//Calculate the number of vertices required
	unsigned int numTiles = mapRows * mapColumns;
	unsigned int numVertices = numTiles * 4;
	unsigned int numIndices = numTiles * 6;
	//Assign the data

	mapIndices.resize(numIndices);
	mapVertices.resize(numVertices * 2);
	mapTextureCoords.resize(numVertices * 2);

	//The current x and y position of the tile in the map
	float mapX = 0;
	float mapY = 0;

	//The tile width and height
	float tileWidth = tileset->getSubTextureWidth();
	float tileHeight = tileset->getSubTextureHeight();

	//Go through each column in the map
	for (unsigned int y = 0; y < mapColumns; y++) {
		//Start on the left hand side
		mapX = 0;

		//Go through each row in the map
		for (unsigned int x = 0; x < mapRows; x++) {
			//Calculate the index for the tile
			unsigned int tileIndex = ((y * mapColumns) + x);

			//The texture coordinate values
			float top, left, bottom, right;

			tileset->getSides(data[tileIndex] - 1, top, left, bottom, right); //Take 1 to be the first tile

			//Calculate the position in the data to assign (4 vertices per tile, each with 2 values)
			unsigned int index = tileIndex * 4;
			unsigned int pos = index * 2;
			//Calculate the position in the indices data to assign
			unsigned int posIndices = tileIndex * 6;

			//Assign the vertices

			//Top-left
			mapVertices[pos] = mapX;
			mapVertices[pos + 1] = mapY;
			mapTextureCoords[pos] = left;
			mapTextureCoords[pos + 1] = top;

			//Top-right
			mapVertices[pos + 2] = mapX + tileWidth;
			mapVertices[pos + 3] = mapY;
			mapTextureCoords[pos + 2] = right;
			mapTextureCoords[pos + 3] = top;

			//Bottom-left
			mapVertices[pos + 4] = mapX;
			mapVertices[pos + 5] = mapY + tileHeight;
			mapTextureCoords[pos + 4] = left;
			mapTextureCoords[pos + 5] = bottom;

			//Bottom-right
			mapVertices[pos + 6] = mapX + tileWidth;
			mapVertices[pos + 7] = mapY + tileHeight;
			mapTextureCoords[pos + 6] = right;
			mapTextureCoords[pos + 7] = bottom;

			//Assign the indices
			mapIndices[posIndices]     = index;     //Top-left
			mapIndices[posIndices + 1] = index + 1; //Top-right
			mapIndices[posIndices + 2] = index + 2; //Bottom-left

			mapIndices[posIndices + 3] = index + 1; //Top-right
			mapIndices[posIndices + 4] = index + 3; //Bottom-right
			mapIndices[posIndices + 5] = index + 2; //Bottom-left

			mapX += tileWidth;
		}
		mapY += tileHeight;
	}

	//Setup for rendering
	renderData = new RenderData(GL_TRIANGLES, numIndices);
	shader = Renderer::getRenderShader("Material")->getShader();

	vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, mapIndices.size() * sizeof(mapIndices[0]), mapIndices, GL_STATIC_DRAW);
	renderData->setIndicesVBO(vboIndices);

	vboVertices = new VBO<GLfloat>(GL_ARRAY_BUFFER, mapVertices.size() * sizeof(mapVertices[0]), mapVertices, GL_STATIC_DRAW, false);
	vboVertices->addAttribute(shader->getAttributeLocation("Position"), 2);
	renderData->addVBO(vboVertices);

	vboTextureCoords = new VBO<GLfloat>(GL_ARRAY_BUFFER, mapTextureCoords.size() * sizeof(mapTextureCoords[0]), mapTextureCoords, GL_STATIC_DRAW, false);
	vboTextureCoords->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);
	renderData->addVBO(vboTextureCoords);

	renderData->setup();
}

void Tilemap::render() {
	//Render the map
	shader->use();

	shader->setUniformMatrix4("MVPMatrix", (Renderer::getCamera()->getProjectionViewMatrix() * Matrix4f().initIdentity()));

	shader->setUniformi("Material_DiffuseTexture", Renderer::bindTexture(tileset->getTexture()));
	shader->setUniformi("Material_HasDiffuseTexture", 1);

	renderData->render();

	Renderer::unbindTexture();

	shader->stopUsing();
}
