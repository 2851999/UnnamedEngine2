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

#ifndef CORE_TERRAIN_CDLODTERRAIN_H_
#define CORE_TERRAIN_CDLODTERRAIN_H_

#include "CDLODHeightMap.h"
#include "CDLODQuadTree.h"

#include "../render/Renderer.h"

/*****************************************************************************
 * The CDLODTerrain class handles the terrain creation and rendering
 *****************************************************************************/

class CDLODTerrain {
private:
	/* The height map for this terrain */
	CDLODHeightMap* heightMap;

	/* The root node of the quad-tree */
	CDLODQuadTreeNode* root;

	/* The ranges */
	std::vector<int> ranges;

	/* The mesh for this terrain */
	Mesh* mesh;

	/* The mesh size */
	float meshSize = 64.0f;

	/* The parameters for the quad-tree */
	float leafNodeSize = 1.0f;
	int   lodDepth = 8;

	/* The terrain shader */
	RenderShader* terrainShader;

//	Texture* texture1;
//	Texture* texture2;
//	Texture* texture3;
public:
	/* The constructor */
	CDLODTerrain(CDLODHeightMap* heightMap);
	CDLODTerrain(std::string heightMapPath);

	/* The destructor */
	virtual ~CDLODTerrain();

	/* The method used to render this terrain */
	void render();

	/* Static method that returns a mesh of vertices for CDLOD terrain (does not have heights applied) */
	static MeshData* createMeshData(int width, int height);
};

#endif /* CORE_TERRAIN_CDLODTERRAIN_H_ */
