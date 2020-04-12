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

#include "../ml/ML.h"

/*****************************************************************************
 * The TilemapLayer class
 *****************************************************************************/

TilemapLayer::TilemapLayer(std::string name, TextureAtlas* tileset, unsigned int columns, unsigned int rows, unsigned int tileWidth, unsigned int tileHeight, std::vector<unsigned int> &data, bool visible, bool editable, GLenum usage) : name(name), tileset(tileset), layerColumns(columns), layerRows(rows), tileWidth(tileWidth), tileHeight(tileHeight), data(data), visible(visible), editable(editable) {
	if (visible) {
		//Assign the data

		bool reduceTriangles = ! editable;

		//Number of indices
		unsigned int numIndices = 0;

		if (! reduceTriangles) {
			//Calculate the number of vertices required
			unsigned int numTiles = layerColumns * layerRows;
			unsigned int numVertices = numTiles * 4;
			numIndices = numTiles * 6;

			//Resize the arrays
			mapIndices.resize(numIndices);
			mapVertices.resize(numVertices * 2);
			mapTextureCoords.resize(numVertices * 2);
			mapVisibility.resize(numVertices);
		}

		//The current x and y position of the tile in the map
		float mapX = 0;
		float mapY = 0;

//		//The tile width and height
//		float tileWidth = tileset->getSubTextureWidth();
//		float tileHeight = tileset->getSubTextureHeight();

		//Go through each row in the map
		for (unsigned int y = 0; y < layerRows; y++) {
			//Start on the left hand side
			mapX = 0;

			//Go through each column in the map
			for (unsigned int x = 0; x < layerColumns; x++) {
				//Calculate the index for the tile
				unsigned int tileIndex = ((y * layerColumns) + x);

				//The id for the tile
				unsigned int tileID = data[tileIndex];

				//The texture coordinate values
				float top = 0, left = 0, bottom = 0, right = 0;

				//Visibility of the current tile
				float tileVisibility = 1.0f;

				if (tileID == 0)
					//Empty tile
					tileVisibility = 0.0f;
				else
					//Get the texture data
					tileset->getSides(tileID - 1, top, left, bottom, right); //Take 1 to be the first tile

				//Pad the texture, so a tiny part of the edge is not shown reducing bleeding artifacts
				top += 0.5f / (float) tileset->getTexture()->getHeight();
				bottom -= 0.5f / (float) tileset->getTexture()->getHeight();
				left += 0.5f / (float) tileset->getTexture()->getWidth();
				right -= 0.5f / (float) tileset->getTexture()->getWidth();

				if (! reduceTriangles) {
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

					//Assign the visibility
					mapVisibility[index]     = tileVisibility;
					mapVisibility[index + 1] = tileVisibility;
					mapVisibility[index + 2] = tileVisibility;
					mapVisibility[index + 3] = tileVisibility;
				} else {
					//Ensure the tile is visible
					if (tileID != 0) {
						unsigned int index = (mapVertices.size() / 2);

						//Top-left
						mapVertices.push_back(mapX);
						mapVertices.push_back(mapY);
						mapTextureCoords.push_back(left);
						mapTextureCoords.push_back(top);

						//Top-right
						mapVertices.push_back(mapX + tileWidth);
						mapVertices.push_back(mapY);
						mapTextureCoords.push_back(right);
						mapTextureCoords.push_back(top);

						//Bottom-left
						mapVertices.push_back(mapX);
						mapVertices.push_back(mapY + tileHeight);
						mapTextureCoords.push_back(left);
						mapTextureCoords.push_back(bottom);

						//Bottom-right
						mapVertices.push_back(mapX + tileWidth);
						mapVertices.push_back(mapY + tileHeight);
						mapTextureCoords.push_back(right);
						mapTextureCoords.push_back(bottom);

						//Assign the indices
						mapIndices.push_back(index);     //Top-left
						mapIndices.push_back(index + 1); //Top-right
						mapIndices.push_back(index + 2); //Bottom-left

						mapIndices.push_back(index + 1); //Top-right
						mapIndices.push_back(index + 3); //Bottom-right
						mapIndices.push_back(index + 2); //Bottom-left

						//Assign the visibility
						mapVisibility.push_back(tileVisibility);
						mapVisibility.push_back(tileVisibility);
						mapVisibility.push_back(tileVisibility);
						mapVisibility.push_back(tileVisibility);
					}
				}

				mapX += tileWidth;
			}
			mapY += tileHeight;
		}

		if (reduceTriangles)
			numIndices = mapIndices.size();

		//Setup for rendering
		renderData = new RenderData(GL_TRIANGLES, numIndices);
		RenderShader* renderShader = Renderer::getRenderShader(Renderer::SHADER_TILEMAP);
		shader = renderShader->getShader();

		vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, mapIndices.size() * sizeof(mapIndices[0]), mapIndices, usage);
		renderData->setIndicesVBO(vboIndices);

		vboVertices = new VBO<GLfloat>(GL_ARRAY_BUFFER, mapVertices.size() * sizeof(mapVertices[0]), mapVertices, usage, false);
		vboVertices->addAttribute(shader->getAttributeLocation("Position"), 2);
		renderData->addVBO(vboVertices);

		vboTextureCoords = new VBO<GLfloat>(GL_ARRAY_BUFFER, mapTextureCoords.size() * sizeof(mapTextureCoords[0]), mapTextureCoords, usage, false);
		vboTextureCoords->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);
		renderData->addVBO(vboTextureCoords);

		vboVisibility = new VBO<GLfloat>(GL_ARRAY_BUFFER, mapVisibility.size() * sizeof(mapVisibility[0]), mapVisibility, usage, false);
		vboVisibility->addAttribute(shader->getAttributeLocation("Visibility"), 1);
		renderData->addVBO(vboVisibility);

		renderData->setup(renderShader);

		//Obtain the required UBO
		shaderCoreUBO = Renderer::getShaderInterface()->getUBO(ShaderInterface::BLOCK_CORE);
	}
}

TilemapLayer::~TilemapLayer() {
	if (visible) {
		delete renderData;
		delete vboIndices;
		delete vboVertices;
		delete vboTextureCoords;
		delete vboVisibility;
	}
}

void TilemapLayer::render() {
	if (visible) {
		//Render the map
		shader->use();

		renderData->getShaderBlock_Model().ue_mvpMatrix = Renderer::getCamera()->getProjectionViewMatrix() * Matrix4f().initIdentity();
		shaderCoreUBO->update(&shaderCoreData, 0, sizeof(ShaderBlock_Core));

		shader->setUniformi("Texture", Renderer::bindTexture(tileset->getTexture()));

		renderData->render();

		Renderer::unbindTexture();

		shader->stopUsing();
	}
}

void TilemapLayer::setTileID(float x, float y, unsigned int id) {
	//Ensure this layer is editable
	if (editable) {
		//Ensure in bounds
		if (isWithinBounds(x, y)) {
			//Get the row and column
			unsigned int col = floor(x / tileset->getSubTextureWidth());
			unsigned int row = floor(y / tileset->getSubTextureHeight());
			//Calculate the tile index
			unsigned int dataIndex = (row * layerColumns) + col;

			//States if the visibility has changed
			bool visibilityChanged = false;

			//Calculate the position in the vertices data to change
			unsigned int index = dataIndex * 4;

			//Check whether the tile will now be hidden
			if (id == 0) {
				//Need to change visibility
				mapVisibility[index]     = 0.0f;
				mapVisibility[index + 1] = 0.0f;
				mapVisibility[index + 2] = 0.0f;
				mapVisibility[index + 3] = 0.0f;

				visibilityChanged = true;
			} else {
				//Ensure tile is visible
				if (data[dataIndex] == 0) {
					//This tile needs its visibility changing
					mapVisibility[index]     = 1.0f;
					mapVisibility[index + 1] = 1.0f;
					mapVisibility[index + 2] = 1.0f;
					mapVisibility[index + 3] = 1.0f;

					visibilityChanged = true;
				}
				//Assign the texture coordinates#
				unsigned int pos = index * 2;

				//The texture coordinate values
				float top, left, bottom, right;

				//Get the texture data
				tileset->getSides(id - 1, top, left, bottom, right); //Take 1 to be the first tile

				//Top-left
				mapTextureCoords[pos] = left;
				mapTextureCoords[pos + 1] = top;

				//Top-right
				mapTextureCoords[pos + 2] = right;
				mapTextureCoords[pos + 3] = top;

				//Bottom-left
				mapTextureCoords[pos + 4] = left;
				mapTextureCoords[pos + 5] = bottom;

				//Bottom-right
				mapTextureCoords[pos + 6] = right;
				mapTextureCoords[pos + 7] = bottom;

				//Update the texture coordinates
				vboTextureCoords->updateStream(mapTextureCoords.size() * sizeof(mapTextureCoords[0]));
			}

			//Update the visibility if necessary
			if (visibilityChanged)
				vboVisibility->updateStream(mapVisibility.size() * sizeof(mapVisibility[0]));

			//Assign the id
			data[dataIndex] = id;
		}
	}
}

unsigned int TilemapLayer::getTileID(float x, float y) {
	//Ensure in bounds
	if (isWithinBounds(x, y)) {
		//Get the row and column
		unsigned int col = floor(x / tileWidth);
		unsigned int row = floor(y / tileHeight);
		//Calculate the tile index
		unsigned int index = (row * layerColumns) + col;

		//Return the id of the tile at this index
		return data[index];
	} else {
		//Return nothing
		return 0;
	}
}

bool TilemapLayer::isWithinBounds(float x, float y) {
	return x > 0 && y > 0 && x < getWidth() && y < getHeight();
}

/*****************************************************************************
 * The Tilemap class
 *****************************************************************************/

Tilemap::~Tilemap() {
	//Go through and delete all of the tilemap layers
	for (unsigned int i = 0; i < layers.size(); i++)
		delete layers[i];
	layers.clear();
}

void Tilemap::renderLayers(unsigned int startIndex, unsigned int endIndex) {
	//Go through each layer and render it
	for (unsigned int i = startIndex; i <= endIndex; i++)
		layers[i]->render();
}

int Tilemap::findLayer(std::string name) {
	//Go through each layer and check if it has name given, returning the index
	//if it does
	for (unsigned int i = 0; i < layers.size(); i++) {
		if (layers[i]->getName() == name)
			return i;
	}

	//No layer has that name
	return -1;
}

TextureAtlas* Tilemap::loadTileset(std::string path, std::string name) {
	//Load the file
	MLDocument document;
	document.load(path + name);

	//Expecting only one element for the tileset
	MLElement tilesetElement = document.getRoot();

	//Get the attributes
	std::vector<MLAttribute> tilesetAttributes = tilesetElement.getAttributes();

	//Info needed
	unsigned int tileWidth = 0, tileHeight = 0, tileCount = 0;

	//Go through all of the attributes
	for (unsigned int i = 0; i < tilesetAttributes.size(); i++) {
		//The current attribute
		MLAttribute current = tilesetAttributes[i];
		//Assign the required info
		if (current.getName() == "tilewidth")
			tileWidth = current.getDataAsUInt();
		else if (current.getName() == "tileheight")
			tileHeight = current.getDataAsUInt();
		else if (current.getName() == "tilecount")
			tileCount = current.getDataAsUInt();
	}

	//Get the image element
	MLElement imageElement = tilesetElement.getChild(tilesetElement.findChild("image"));

	//Get the path to the texture
	std::string texturePath = path + imageElement.getAttribute(imageElement.findAttribute("source")).getData();

	//Load the texture
	Texture* texture = Texture::loadTexture(texturePath);

	//Calculate the number of rows/columns in the tileset
	unsigned int columns = texture->getWidth() / tileWidth;
	unsigned int rows = texture->getHeight() / tileHeight;

	//Return the result
	return new TextureAtlas(texture, columns, rows, tileCount);
}

Tilemap* Tilemap::loadTilemap(std::string path, std::string name, bool editable, GLenum usage) {
	//Load the file
	MLDocument document;
	document.load(path + name);

	//Expecting only one element for the tilemap
	MLElement tilemapElement = document.getRoot();

	//Get the attributes
	std::vector<MLAttribute> tilesetAttributes = tilemapElement.getAttributes();

	//Info needed
	unsigned int rows = 0, columns = 0, tileWidth = 0, tileHeight = 0;

	//Go through all of the attributes
	for (unsigned int i = 0; i < tilesetAttributes.size(); i++) {
		//The current attribute
		MLAttribute current = tilesetAttributes[i];
		//Assign the required info
		if (current.getName() == "width")
			columns = current.getDataAsUInt();
		else if (current.getName() == "height")
			rows = current.getDataAsUInt();
		else if (current.getName() == "tilewidth")
			tileWidth = current.getDataAsUInt();
		else if (current.getName() == "tileheight")
			tileHeight = current.getDataAsUInt();
	}

	//The tilesets
	std::vector<TextureAtlas*> tilesets;
	std::vector<unsigned int> firstgids;

	//Get the children of the tilemap element and go through them
	std::vector<MLElement> children = tilemapElement.getChildren();

	//The tilemap
	Tilemap* tilemap = new Tilemap(columns, rows, tileWidth, tileHeight);

	for (unsigned int i = 0; i < children.size(); i++) {
		//Check what the current child element is
		MLElement current = children[i];

		if (current.getName() == "tileset") {
			std::vector<std::string> pathName = utils_string::strSplitLast(path + current.getAttribute(current.findAttribute("source")).getData(), "/");
			//Load the tileset and add it
			tilesets.push_back(loadTileset(pathName[0] + "/", pathName[1]));
			//Get the first grid id and add it
			firstgids.push_back(current.getAttribute(current.findAttribute("firstgid")).getDataAsUInt());
		} else if (current.getName() == "layer") {
			//Get the name
			std::string name = current.getAttribute(current.findAttribute("name")).getData();

			//Determine whether the layer is visible (default is true)
			bool visible = true;

			int pos = current.findAttribute("visible");
			if (pos != -1)
				visible = current.getAttribute(pos).getDataAsBool();

			//Get the data element
			MLElement dataElement = current.getChild(current.findChild("data"));
			//Get the data
			std::string dataString = dataElement.getContents();
			//The data for the map
			std::vector<unsigned int> data;
			//Split up the data
			std::vector<std::string> split = utils_string::strSplit(dataString, ",");
			//Assign the tileset to the first available
			TextureAtlas* tileset = NULL;
			//Go through each tile and convert the value to an integer
			data.resize(split.size());

			//The first grid id for the current tileset
			unsigned int currentFirstGID = 0;

			for (unsigned int i = 0; i < split.size(); i++) {
				data[i] = utils_string::strToUInt(split[i]);
				//Check which tileset this uses if not assigned
				if (! tileset && data[i] != 0) {
					//Go through and find which tileset should be used
					unsigned int loc = 0;
					while (loc < firstgids.size() - 1 && firstgids[loc + 1] <= data[i])
						loc++;
					//Assign the tileset
					tileset = tilesets[loc];
					currentFirstGID = firstgids[loc];
				}
				if (data[i] != 0)
					//Subtract the first grid id
					data[i] -= (currentFirstGID - 1);
			}
			//Create and add this layer
			tilemap->addLayer(new TilemapLayer(name, tileset, columns, rows, tileWidth, tileHeight, data, visible, editable, usage));
		}
	}

	//Return the tilemap
	return tilemap;
}
