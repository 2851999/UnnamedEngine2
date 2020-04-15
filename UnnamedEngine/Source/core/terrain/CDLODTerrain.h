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

#pragma once

#include "CDLODHeightMap.h"
#include "CDLODQuadTree.h"

#include "../render/Renderer.h"

/*****************************************************************************
 * The CDLODTerrain class handles the terrain creation and rendering
 *****************************************************************************/

class CDLODTerrain : public GameObject3D {
private:
	/* The height map for this terrain */
	CDLODHeightMap* heightMap;

	/* The root node of the quad-tree */
	CDLODQuadTreeNode* root;

	/* The ranges */
	std::vector<int> ranges;

	/* The mesh size */
	float meshSize = 32.0f;

	/* The parameters for the quad-tree */
	float leafNodeSize = 4.0f;
	int   lodDepth = 8; //0 is most detail, 8 is least

	/* Range multiplier to help resolve seams between nodes */
	const float RANGE_MULTIPLIER = 1.25f;

	/* The UBO for the terrain data in the shader and an instance of its data structure */
	ShaderBlock_Terrain shaderTerrainData;
	UBO* shaderTerrainUBO;

//	Texture* texture1;
//	Texture* texture2;
//	Texture* texture3;
public:
	/* The constructor */
	CDLODTerrain(CDLODHeightMap* heightMap, int lodDepth, float meshSize);
	CDLODTerrain(std::string heightMapPath, int lodDepth, float meshSize);

	/* The destructor */
	virtual ~CDLODTerrain();

	/* The method used to render this terrain */
	virtual void render() override;

	/* Method used to get the height at a particular location (in world coordinates) */
	inline float getHeight(float x, float y) { return heightMap->getHeight(x, y); }

	/* Static method that returns a mesh of vertices for CDLOD terrain (does not have heights applied) */
	static MeshData* createMeshData(int width, int height);
};

