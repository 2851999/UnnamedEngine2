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

#include "CDLODQuadTree.h"

#include "../../utils/Utils.h"

#include "../render/Renderer.h"

/*****************************************************************************
 * The CDLODQuadTree class
 *****************************************************************************/

CDLODQuadTreeNode::CDLODQuadTreeNode(CDLODHeightMap* heightMap, float nodeSize, int lod, float x, float z) {
	//Assign the position and size
	this->x = x;
	this->z = z;
	this->size = nodeSize;
	//Check whether this is a leaf node
	if (lod == 0) {
		isLeafNode = true;

		//Assign the nodes
		children[0] = nullptr;
		children[1] = nullptr;
		children[2] = nullptr;
		children[3] = nullptr;

		//Assign the minimum and maximum height values
		minHeight = heightMap->getMinHeight(x, z, nodeSize);
		maxHeight = heightMap->getMaxHeight(x, z, nodeSize);

//		std::cout << "NODE" << std::endl;
//		std::cout << minHeight << std::endl;
//		std::cout << maxHeight << std::endl;
	} else {
		isLeafNode = false;

		float halfSize = nodeSize / 2.0f;
		float qSize = halfSize / 2.0f;

		//Assign the nodes
		children[0] = new CDLODQuadTreeNode(heightMap, halfSize, lod - 1, x - qSize, z + qSize); //Top left
		children[1] = new CDLODQuadTreeNode(heightMap, halfSize, lod - 1, x + qSize, z + qSize); //Top right
		children[2] = new CDLODQuadTreeNode(heightMap, halfSize, lod - 1, x - qSize, z - qSize); //Bottom left
		children[3] = new CDLODQuadTreeNode(heightMap, halfSize, lod - 1, x + qSize, z - qSize); //Bottom right

		//Assign the maximum and minimum height values
		minHeight = utils_maths::min(utils_maths::min(children[0]->getMinHeight(), children[1]->getMinHeight()), utils_maths::min(children[2]->getMinHeight(), children[3]->getMinHeight()));
		maxHeight = utils_maths::max(utils_maths::max(children[0]->getMaxHeight(), children[1]->getMaxHeight()), utils_maths::max(children[2]->getMaxHeight(), children[3]->getMaxHeight()));
	}

//	debugMesh = new GameObject3D(new Mesh(MeshBuilder::createCube(nodeSize, maxHeight - minHeight, nodeSize)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
//	debugMesh->setPosition(Vector3f(x, (maxHeight + minHeight) / 2.0f, z));
//	debugMesh->update();
}

CDLODQuadTreeNode::~CDLODQuadTreeNode() {
	if (! isLeafNode) {
		delete children[0];
		delete children[1];
		delete children[2];
		delete children[3];
	}
}

bool CDLODQuadTreeNode::LODSelect(std::vector<int> ranges, int lodLevel, Camera3D* camera, std::vector<CDLODQuadTreeNode*>& selectionList) {
	currentRange = ranges[lodLevel];

	if (! intersectsSphere(camera->getPosition(), ranges[lodLevel])) {
		return false;
	}

	float halfSize = size / 2.0f;
	//Calculate the maximum and minimum points of the AABB
	Vector3f min = Vector3f(x - halfSize, minHeight, z - halfSize);
	Vector3f max = Vector3f(x + halfSize, maxHeight, z + halfSize);

	if (! camera->getFrustum().AABBInFrustum(min, max)) {
		return true;
	}

	if (lodLevel == 0) {
		selectionList.push_back(this);
		return true;
	} else {
		if (! intersectsSphere(camera->getPosition(), ranges[lodLevel - 1])) {
			selectionList.push_back(this);
		} else {
			for (unsigned int i = 0; i < 4; i++) {
				//The current child node
				CDLODQuadTreeNode* childNode = children[i];

				if (! childNode->LODSelect(ranges, lodLevel - 1, camera, selectionList)) {
					selectionList.push_back(childNode);
				}
			}
		}
		return true;
	}
}

bool CDLODQuadTreeNode::intersectsSphere(Vector3f position, float radius) {
	float halfSize = size / 2.0f;
	//Calculate the maximum and minimum points of the AABB
	Vector3f c1 = Vector3f(x - halfSize, minHeight, z - halfSize);
	Vector3f c2 = Vector3f(x + halfSize, maxHeight, z + halfSize);

	float squaredDistance = radius * radius;

	if (position.getX() < c1.getX()) squaredDistance -= pow(position.getX() - c1.getX(), 2.0f);
	else if (position.getX() > c2.getX()) squaredDistance -= pow(position.getX() - c2.getX(), 2.0f);
	if (position.getY() < c1.getY()) squaredDistance -= pow(position.getY() - c1.getY(), 2.0f);
	else if (position.getY() > c2.getY()) squaredDistance -= pow(position.getY() - c2.getY(), 2.0f);
	if (position.getZ() < c1.getZ()) squaredDistance -= pow(position.getZ() - c1.getZ(), 2.0f);
	else if (position.getZ() > c2.getZ()) squaredDistance -= pow(position.getZ() - c2.getZ(), 2.0f);

	return squaredDistance > 0.0f;
}
