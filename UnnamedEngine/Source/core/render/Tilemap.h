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

#pragma once

#include "RenderData.h"
#include "ShaderInterface.h"
#include "TextureAtlas.h"

/*****************************************************************************
 * The TilemapLayer class
 *****************************************************************************/

class TilemapLayer {
private:
	/* The name of this layer (If it has one) */
	std::string name;

	/* The tileset used in rendering this map */
	TextureAtlas* tileset;

	/* The number of rows and columns of tiles */
	unsigned int layerColumns;
	unsigned int layerRows;

	/* The tile width and height */
	unsigned int tileWidth;
	unsigned int tileHeight;

	/* The data in this tilemap */
	std::vector<unsigned int> data;

	/* States whether this layer is visible */
	bool visible;

	/* The data passed to OpenGL about the tilemap */
	std::vector<unsigned int> mapIndices;   //This stores the indices for the vertices of the tiles
	std::vector<GLfloat> mapVertices; 		//This stores the position's of the vertices of the tiles
	std::vector<GLfloat> mapTextureCoords;  //This stores the data for the texture coordinates of the tiles
	std::vector<GLfloat> mapVisibility;     //This stores the data for the visibility of the tiles

	/* The render data used for rendering */
	RenderData* renderData = NULL;

	/* The VBO's for OpenGL */
	VBO<unsigned int>* vboIndices = NULL;
	VBO<GLfloat>* vboVertices = NULL;
	VBO<GLfloat>* vboTextureCoords = NULL;
	VBO<GLfloat>* vboVisibility = NULL;

	/* The shader used for rendering */
	Shader* shader = NULL;

	/* UBO and data structure for updating core shader values */
	ShaderBlock_Core shaderCoreData;
	UBO* shaderCoreUBO;

	/* States whether this layer is editable */
	bool editable;
public:
	/* The constructor */
	TilemapLayer(std::string name, TextureAtlas* tileset, unsigned int columns, unsigned int rows, unsigned int tileWidth, unsigned int tileHeight, std::vector<unsigned int> &data, bool visible, bool editable, GLenum usage);

	/* The destructor */
	virtual ~TilemapLayer();

	/* Renders this tilemap */
	void render();

	/* Method to set a tile's id given its location in world coordinates */
	void setTileID(float x, float y, unsigned int id);

	/* Returns the tile at a specific location in the map given in world coordinates - returns
	 * 0 if out of bounds */
	unsigned int getTileID(float x, float y);

	/* Returns a boolean stating if a specific location given in world coordinates is within
	 * the bounds of this layer */
	bool isWithinBounds(float x, float y);

	/* Getters and setters */
	inline void setName(std::string name) { this->name = name; }
	inline std::string getName() { return name; }

	inline std::vector<unsigned int>& getData() { return data; }

	/* Returns the width and height of this layer's grid */
	inline unsigned int getGridWidth() { return layerColumns; }
	inline unsigned int getGridHeight() { return layerRows; }

	/* Returns the width and height of this layer in pixels */
	inline unsigned int getWidth() { return layerColumns * tileWidth; }
	inline unsigned int getHeight() { return layerRows * tileHeight; }

	/* Returns the width and height of the tiles in this layer */
	inline unsigned int getTileWidth() { return tileWidth; }
	inline unsigned int getTileHeight() { return tileHeight; }

	inline bool isVisible() { return visible; }
};

/*****************************************************************************
 * The Tilemap class
 *****************************************************************************/

class Tilemap {
private:
	/* The layers of this tilemap */
	std::vector<TilemapLayer*> layers;

	/* The grid width and height of this tilemap */
	unsigned int gridWidth;
	unsigned int gridHeight;

	/* The width and height of the tiles */
	unsigned int tileWidth;
	unsigned int tileHeight;
public:
	/* The constructor */
	Tilemap(unsigned int gridWidth, unsigned int gridHeight, unsigned int tileWidth, unsigned int tileHeight) : gridWidth(gridWidth), gridHeight(gridHeight), tileWidth(tileWidth), tileHeight(tileHeight) {}

	/* The destructor */
	virtual ~Tilemap();

	/* Renders this tilemap */
	inline void render() { renderLayers(0, layers.size() - 1); }

	/* Renders a specific range of layers (inclusive) */
	void renderLayers(unsigned int startIndex, unsigned int endIndex);

	/* Returns the index of the layer with a given name or -1 if it is not found */
	int findLayer(std::string name);

	/* Adds a tilemap layer to this tilemap */
	inline void addLayer(TilemapLayer* layer) { layers.push_back(layer); }

	/* Getters and setters */
	inline std::vector<TilemapLayer*>& getLayers() { return layers; }

	inline unsigned int getGridWidth() { return gridWidth; }
	inline unsigned int getGridHeight() { return gridHeight; }

	inline unsigned int getWidth() { return gridWidth * tileWidth; }
	inline unsigned int getHeight() { return gridHeight * tileHeight; }

	inline unsigned int getTileWidth() { return tileWidth; }
	inline unsigned int getTileHeight() { return tileHeight; }

	/* Method used to load a tileset from a file */
	static TextureAtlas* loadTileset(std::string path, std::string name);
	/* Method used to load a tile map from a file */
	static Tilemap* loadTilemap(std::string path, std::string name, bool editable = false, GLenum usage = GL_STATIC_DRAW);
};

