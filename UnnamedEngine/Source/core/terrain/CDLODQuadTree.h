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

#ifndef CORE_TERRAIN_CDLODQUADTREE_H_
#define CORE_TERRAIN_CDLODQUADTREE_H_

#include "CDLODHeightMap.h"

#include "../render/Camera.h"

/*****************************************************************************
 * The CDLODQuadTreeNode class represents a node of a quad-tree for CDLOD
 *****************************************************************************/

class CDLODQuadTreeNode {
private:
	/* The maximum and minimum height values */
	float minHeight;
	float maxHeight;

	/* The position of this node */
	float x;
	float z;

	/* The size of this node */
	float size;

	/* The current range this node is visible */
	float currentRange = 0.0f;

	/* States whether this is a leaf node */
	bool isLeafNode;

	/* Pointers to the 4 child nodes of this node (If assigned) */
	CDLODQuadTreeNode* children[4];

	//GameObject3D* debugMesh;
public:
	/* The constructor */
	CDLODQuadTreeNode(CDLODHeightMap* heightMap, float nodeSize, int lod, float x, float z);

	/* The destructor */
	virtual ~CDLODQuadTreeNode();

	/* Method to select the nodes to render */
	bool LODSelect(std::vector<int> ranges, int lodLevel, Camera3D* camera, std::vector<CDLODQuadTreeNode*>& selectionList);

	/* Returns whether the bounds of this node (an AABB) intersects a sphere */
	bool intersectsSphere(Vector3f position, float radius);

	//inline void debug() { debugMesh->render(); }

	/* Getters */
	inline float getMinHeight() { return minHeight; }
	inline float getMaxHeight() { return maxHeight; }
	inline float getX() { return x; }
	inline float getZ() { return z; }
	inline float getSize() { return size; } //* 1.1f; }
	inline float getRange() { return currentRange; }
};

#endif /* CORE_TERRAIN_CDLODQUADTREE_H_ */
