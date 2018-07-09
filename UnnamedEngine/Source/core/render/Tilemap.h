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

#ifndef CORE_RENDER_TILEMAP_H_
#define CORE_RENDER_TILEMAP_H_

#include "RenderData.h"
#include "TextureAtlas.h"

/*****************************************************************************
 * The TilemapLayer class
 *****************************************************************************/

class TilemapLayer {
private:
	/* The tileset used in rendering this map */
	TextureAtlas* tileset;

	/* The number of rows and columns of tiles */
	unsigned int mapRows;
	unsigned int mapColumns;

	/* The data in this tilemap */
	std::vector<unsigned int> data;

	/* The data passed to OpenGL about the tilemap */
	std::vector<unsigned int> mapIndices;   //This stores the indices for the vertices of the tiles
	std::vector<GLfloat> mapVertices; 		//This stores the position's of the vertices of the tiles
	std::vector<GLfloat> mapTextureCoords;  //This stores the data for the texture coordinates of the tiles
	std::vector<GLfloat> mapVisibility;  //This stores the data for the visibility of the tiles

	/* The render data used for rendering */
	RenderData* renderData = NULL;

	/* The VBO's for OpenGL */
	VBO<unsigned int>* vboIndices = NULL;
	VBO<GLfloat>* vboVertices = NULL;
	VBO<GLfloat>* vboTextureCoords = NULL;
	VBO<GLfloat>* vboVisibility = NULL;

	/* The shader used for rendering */
	Shader* shader = NULL;

	/* The width and height of this layer */
	unsigned int width;
	unsigned int height;
public:
	/* The constructor */
	TilemapLayer(TextureAtlas* tileset, unsigned int rows, unsigned int columns, std::vector<unsigned int> &data, GLenum usage);

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

	/* Returns the width and height of this layer in pixels */
	inline unsigned int getWidth() { return width; }
	inline unsigned int getHeight() { return height; }
};

/*****************************************************************************
 * The Tilemap class
 *****************************************************************************/

class Tilemap {
private:
	/* The layers of this tilemap */
	std::vector<TilemapLayer*> layers;
public:
	/* The constructor */
	Tilemap() {}

	/* The destructor */
	virtual ~Tilemap();

	/* Renders this tilemap */
	void render();

	/* Adds a tilemap layer to this tilemap */
	inline void addLayer(TilemapLayer* layer) { layers.push_back(layer); }

	/* Getters and setters */
	inline std::vector<TilemapLayer*>& getLayers() { return layers; }

	/* Method used to load a tileset from a file */
	static TextureAtlas* loadTileset(std::string path, std::string name);
	/* Method used to load a tile map from a file */
	static Tilemap* loadTilemap(std::string path, std::string name, GLenum usage = GL_STATIC_DRAW);
};

#endif /* CORE_RENDER_TILEMAP_H_ */
