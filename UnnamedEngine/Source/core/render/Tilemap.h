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
 * The Tilemap class
 *****************************************************************************/

class Tilemap {
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
public:
	/* The constructor */
	Tilemap(TextureAtlas* tileset, unsigned int rows, unsigned int columns, std::vector<unsigned int> &data);

	/* The destructor */
	virtual ~Tilemap() {}

	/* Renders this tilemap */
	void render();

	/* Method used to load a tileset from a file */
	static TextureAtlas* loadTileset(std::string path, std::string name);
	static Tilemap* loadTilemap(std::string path, std::string name);
};


#endif /* CORE_RENDER_TILEMAP_H_ */
