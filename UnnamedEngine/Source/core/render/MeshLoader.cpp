/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
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

#include "MeshLoader.h"

#include <fstream>
#include "../../utils/Logging.h"

using std::__basic_file;

void MeshLoader::addChildren(const aiNode* node, std::map<const aiNode*, const aiBone*>& nodes) {
	if (nodes.count(node) == 0)
		nodes.insert(std::pair<const aiNode*, const aiBone*>(node, NULL));
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		addChildren(node->mChildren[i], nodes);
}

const aiNode* MeshLoader::findMeshNode(const aiNode* parent) {
	//The current node
	const aiNode* node = NULL;
	//Check whether the current node is the correct one
	if (parent->mNumMeshes > 0)
		//The parent given is the one being searched for so return it
		return parent;
	//Go through each child node of the parent
	for (unsigned int i = 0; i < parent->mNumChildren; i++) {
		//Check the current child
		node = findMeshNode(parent->mChildren[i]);
		//Return the node if it has been found
		if (node)
			return node;
	}
	//Return NULL as the node was not found
	return NULL;
}

const aiMatrix4x4 MeshLoader::calculateMatrix(const aiNode* current, aiMatrix4x4 currentMatrix) {
	currentMatrix = currentMatrix * current->mTransformation;
	if (current->mParent)
		currentMatrix = calculateMatrix(current->mParent, currentMatrix);
	return currentMatrix;
}

Mesh* MeshLoader::loadModel(std::string path, std::string fileName) {
	if (StrUtils::strEndsWith(fileName, ".model"))
		return loadEngineModel(path, fileName);
	else
		return loadAssimpModel(path, fileName);
}

Mesh* MeshLoader::loadAssimpModel(std::string path, std::string fileName, bool genNormals) {
	//Load the file using Assimp
	unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices;
	if (genNormals)
		flags = flags | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
	const struct aiScene* scene = aiImportFile((path + fileName).c_str(), flags); //aiProcess_JoinIdenticalVertices aiProcessPreset_TargetRealtime_MaxQuality
	//The MeshData instance used to store the data for the current mesh
	MeshData* currentData = new MeshData(MeshData::DIMENSIONS_3D);
	//The current and last number of indices added
	unsigned int numIndices = 0;
	unsigned int numVertices = 0;
	bool hasBones = false;

	//Ensure the data was loaded successfully
	if (scene != NULL) {
		//Go through each loaded mesh
		for (unsigned int a = 0; a < scene->mNumMeshes; a++) {
			//Pointer to the current mesh being read
			const struct aiMesh* currentMesh = scene->mMeshes[a];

			hasBones = hasBones || (currentMesh->mNumBones > 0);

			//Go though all of the vertices
			for (unsigned int i = 0; i < currentMesh->mNumVertices; i++) {
				//Add all of the position data
				aiVector3D& position = currentMesh->mVertices[i];
				currentData->addPosition(Vector3f(position.x, position.y, position.z));

				//Add the texture coordinates data if it exists
				if (currentMesh->HasTextureCoords(0)) {
					aiVector3D& textureCoord = currentMesh->mTextureCoords[0][i];
					currentData->addTextureCoord(Vector2f(textureCoord.x, textureCoord.y));
				}
				//Add the normals data if it exists
				if (currentMesh->HasNormals()) {
					aiVector3D& normal = currentMesh->mNormals[i];
					currentData->addNormal(Vector3f(normal.x, normal.y, normal.z));

					//Add the tangent data if it exists
					if (currentMesh->HasTangentsAndBitangents()) {
						aiVector3D& tangent = currentMesh->mTangents[i];
						currentData->addTangent(Vector3f(tangent.x, tangent.y, tangent.z));

						//Add the bitangent data
						aiVector3D& bitangent = currentMesh->mBitangents[i];
						currentData->addBitangent(Vector3f(bitangent.x, bitangent.y, bitangent.z));
					}
				}
			}
			//Go through each face in the current mesh
			for (unsigned int b = 0; b < currentMesh->mNumFaces; b++) {
				//Thee current face of the current mesh
				struct aiFace& currentFace = currentMesh->mFaces[b];

				//Goes through each vertex of the face, this assumes the model is triangulated i.e. there are
				//3 vertices per face in the mesh
				for (int c = 0; c < 3; c++)
					//Add the indices for the current face
					currentData->addIndex(currentFace.mIndices[c]);
			}

			//Add a sub data instance
			currentData->addSubData(numIndices, numVertices, currentMesh->mNumFaces * 3, currentMesh->mMaterialIndex);
			numIndices += currentMesh->mNumFaces * 3;
			numVertices += currentMesh->mNumVertices;
		}

		//The skeleton instance
		Skeleton* skeleton = NULL;

		//Check whether there are bones
		if (hasBones) {
			//Create the skeleton instance
			skeleton = new Skeleton();
			//Assign the global inverse transform
			aiMatrix4x4 matrix = scene->mRootNode->mTransformation;
			skeleton->setGlobalInverseTransform(toMatrix4f(matrix.Inverse()));

			//Necessary nodes
			std::map<const aiNode*, const aiBone*> necessaryNodes;
			std::map<std::string, unsigned int>    boneIndices;
			std::vector<MeshData::VertexBoneData> verticesBonesData;
			//Place to store all of the created bones
			std::vector<Bone*> bones;

			verticesBonesData.resize(numVertices);

			addChildren(scene->mRootNode, necessaryNodes);

			//Go through each mesh
			for (unsigned int a = 0; a < scene->mNumMeshes; a++) {
				//The current mesh instance
				const aiMesh* currentMesh = scene->mMeshes[a];
				//Go through each bone in the mesh
				for (unsigned int b = 0; b < currentMesh->mNumBones; b++) {
					//Find the corresponding node in the scene's hierarchy
					const aiNode* correspondingNode = scene->mRootNode->FindNode(currentMesh->mBones[b]->mName);
					//Add the node to the necessary nodes if it was found
					if (correspondingNode)
						necessaryNodes[correspondingNode] = currentMesh->mBones[b];
				}
			}

			//Add the bone indices
			unsigned int currentIndex = 0;
			for (const auto& current : necessaryNodes) {
				boneIndices.insert(std::pair<std::string, unsigned int>(std::string(current.first->mName.C_Str()), currentIndex));
				currentIndex++;
			}

			for (unsigned int a = 0; a < scene->mNumMeshes; a++) {
				//Pointer to the current mesh being read
				const struct aiMesh* currentMesh = scene->mMeshes[a];
				for (unsigned int b = 0; b < currentMesh->mNumBones; b++) {
					unsigned int boneIndex = boneIndices[std::string(currentMesh->mBones[b]->mName.C_Str())];

					for (unsigned int c = 0; c < currentMesh->mBones[b]->mNumWeights; c++) {
						unsigned int vertexID = currentData->getSubData(a).baseVertex + currentMesh->mBones[b]->mWeights[c].mVertexId;
						float weight = currentMesh->mBones[b]->mWeights[c].mWeight;
						verticesBonesData[vertexID].addBoneData(boneIndex, weight);
					}
				}
			}
			for (unsigned int a = 0; a < verticesBonesData.size(); a++) {
				for (unsigned int b = 0; b < NUM_BONES_PER_VERTEX; b++)
					currentData->addBoneData(verticesBonesData[a].ids[b], verticesBonesData[a].weights[b]);
			}

			//Make room for all of the bones
			bones.resize(currentIndex);

			//Reset the current index, to keep track of the bone being processed
			currentIndex = 0;
			//The root bone index
			unsigned int rootBoneIndex = 0;
			//At this point, all necessary nodes should have been added, so now go through and add the bones
			for (const auto& current : necessaryNodes) {
				//Create the current bone
				Bone* currentBone = new Bone(std::string(current.first->mName.C_Str()), toMatrix4f(current.first->mTransformation));
				//Check whether the current bone is the root one and assign it's index if necessary
				if (current.first == scene->mRootNode)
					rootBoneIndex = currentIndex;
				//Assign the bone offset matrix if the bone exists
				if (current.second)
					currentBone->setOffset(toMatrix4f(current.second->mOffsetMatrix));
				//Add the child bone indices
				for (unsigned int b = 0; b < current.first->mNumChildren; b++) {
					//Check whether the child is necessary
					if (necessaryNodes.find(current.first->mChildren[b]) != necessaryNodes.end())
						//Add the child index
						currentBone->addChild(boneIndices[std::string(current.first->mChildren[b]->mName.C_Str())]);
				}
				//Add the current bone
				bones[currentIndex] = currentBone;
				//Increment the current index
				currentIndex++;
			}

			//Assign the bones in the skeleton
			skeleton->setBones(bones);
			skeleton->setRootBone(rootBoneIndex);

			//Place to store all of the created animations
			std::vector<Animation*> animations;

			//Make room for all of the animations
			animations.resize(scene->mNumAnimations);

			//Now go through all of the animations
			for (unsigned int b = 0; b < scene->mNumAnimations; b++) {
				//Create the current animation
				Animation* currentAnimation = new Animation(std::string(scene->mAnimations[b]->mName.C_Str()), scene->mAnimations[b]->mTicksPerSecond, scene->mAnimations[b]->mDuration);
				//The current aiAnimation instance
				const aiAnimation* currentAssimpAnim = scene->mAnimations[b];
				//Place to store the bone data
				std::vector<BoneAnimationData*> boneData;
				//Make room for the bone animation data
				boneData.resize(currentAssimpAnim->mNumChannels);
				//Go through each animation channel
				for (unsigned int c = 0; c < currentAssimpAnim->mNumChannels; c++) {
					//The current aiNodeAnim instance
					const aiNodeAnim* currentAnimNode = currentAssimpAnim->mChannels[c];
					//Create the bone animation data instance
					BoneAnimationData* currentBoneData = new BoneAnimationData(boneIndices[std::string(currentAnimNode->mNodeName.C_Str())], currentAnimNode->mNumPositionKeys, currentAnimNode->mNumRotationKeys, currentAnimNode->mNumScalingKeys);
					//Go through and assign all of the data
					for (unsigned int d = 0; d < currentAnimNode->mNumPositionKeys; d++)
						currentBoneData->setKeyframePosition(d, toVector3f(currentAnimNode->mPositionKeys[d].mValue), currentAnimNode->mPositionKeys[d].mTime);
					for (unsigned int d = 0; d < currentAnimNode->mNumRotationKeys; d++)
						currentBoneData->setKeyframeRotation(d, toQuaternion(currentAnimNode->mRotationKeys[d].mValue), currentAnimNode->mRotationKeys[d].mTime);
					for (unsigned int d = 0; d < currentAnimNode->mNumScalingKeys; d++)
						currentBoneData->setKeyframeScale(d, toVector3f(currentAnimNode->mScalingKeys[d].mValue), currentAnimNode->mScalingKeys[d].mTime);
					//Add the bone data
					boneData[c] = currentBoneData;
				}
				//Assign the bone data in the current animation
				currentAnimation->setBoneData(boneData);
				//Assign the animation
				animations[b] = currentAnimation;
			}
			//Assign the animations in the skeleton instance
			skeleton->setAnimations(animations);
		}

		//Create the mesh
		Mesh* mesh = new Mesh(currentData);
		const aiNode* meshNode = findMeshNode(scene->mRootNode);
		aiMatrix4x4 matrix = calculateMatrix(meshNode, aiMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
																   0.0f, 1.0f, 0.0f, 0.0f,
																   0.0f, 0.0f, 1.0f, 0.0f,
																   0.0f, 0.0f, 0.0f, 1.0f));
		mesh->setMatrix(toMatrix4f(matrix));
		if (skeleton)
			skeleton->setGlobalInverseTransform(toMatrix4f(matrix.Inverse()));
		//Assign the mesh's skeleton
		mesh->setSkeleton(skeleton);

		//Calculate the bounding sphere for the mesh
		MeshData::BoundingSphere sphere = currentData->calculateBoundingSphere();
		//Assign the bounding sphere properties
		mesh->setBoundingSphereCentre(sphere.centre);
		mesh->setBoundingSphereRadius(sphere.radius);

		//Load and add the materials
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			//Load the current material
			Material* material = loadAssimpMaterial(path, fileName, scene->mMaterials[i]);

			if (i == 0)
				mesh->setMaterial(material);
			else
				mesh->addMaterial(material);
		}

		//Release all of the resources Assimp loaded
		aiReleaseImport(scene);
		//Return the meshes
		return mesh;
	} else {
		//Log an error as Assimp didn't manage to load the model correctly
		Logger::log("The model '" + path + fileName + "' could not be loaded", "Mesh", LogType::Error);
		return NULL;
	}
}

Material* MeshLoader::loadAssimpMaterial(std::string path, std::string fileName, const aiMaterial* mat) {
	//Create the material instance
	Material* material = new Material();

	//Load and assign the textures
	material->ambientTexture  = loadAssimpTexture(path, mat, aiTextureType_AMBIENT);
	material->diffuseTexture  = loadAssimpTexture(path, mat, aiTextureType_DIFFUSE);
	material->specularTexture = loadAssimpTexture(path, mat, aiTextureType_SPECULAR);

	//Check to see whether the material has a normal map
	if (mat->GetTextureCount(aiTextureType_NORMALS) != 0)
		material->normalMap = loadAssimpTexture(path, mat, aiTextureType_NORMALS);
	else if (StrUtils::strEndsWith(fileName, ".obj") && (mat->GetTextureCount(aiTextureType_HEIGHT) != 0))
		material->normalMap = loadAssimpTexture(path, mat, aiTextureType_HEIGHT);

	if (mat->GetTextureCount(aiTextureType_DISPLACEMENT) != 0)
		material->parallaxMap = loadAssimpTexture(path, mat, aiTextureType_DISPLACEMENT);

	//Load and assign the colours
	material->ambientColour  = loadAssimpColour(mat, AI_MATKEY_COLOR_AMBIENT);
	material->diffuseColour  = loadAssimpColour(mat, AI_MATKEY_COLOR_DIFFUSE);
	material->specularColour = loadAssimpColour(mat, AI_MATKEY_COLOR_SPECULAR);

	return material;
}

Texture* MeshLoader::loadAssimpTexture(std::string path, const aiMaterial* material, const aiTextureType type) {
	//Check whether the texture is defined
	if (material->GetTextureCount(type) != 0) {
		//Get the path of the texture
		aiString p;
		material->GetTexture(type, 0, &p);
		//Return the loaded texture
		return Texture::loadTexture(path + StrUtils::str(p.C_Str()));
	} else
		return NULL;
}

Colour MeshLoader::loadAssimpColour(const aiMaterial* material, const char* key, unsigned int type, unsigned int idx) {
	//The colour
	aiColor4D colour;
	//Attempt to load the colour
	if (material->Get(key, type, idx, colour) == AI_SUCCESS)
		return Colour(colour.r, colour.g, colour.b, colour.a);
	else
		return Colour::WHITE;
}

Vector3f MeshLoader::toVector3f(aiVector3D vec) {
	return Vector3f(vec.x, vec.y, vec.z);
}

Quaternion MeshLoader::toQuaternion(aiQuaternion quat) {
	return Quaternion(quat.x, quat.y, quat.z, quat.w);
}

Matrix4f MeshLoader::toMatrix4f(aiMatrix4x4 mat) {
	Matrix4f m;

	m.set(0, 0, mat.a1); m.set(0, 1, mat.a2); m.set(0, 2, mat.a3); m.set(0, 3, mat.a4);
	m.set(1, 0, mat.b1); m.set(1, 1, mat.b2); m.set(1, 2, mat.b3); m.set(1, 3, mat.b4);
	m.set(2, 0, mat.c1); m.set(2, 1, mat.c2); m.set(2, 2, mat.c3); m.set(2, 3, mat.c4);
	m.set(3, 0, mat.d1); m.set(3, 1, mat.d2); m.set(3, 2, mat.d3); m.set(3, 3, mat.d4);

	return m;
}

Matrix4f MeshLoader::toMatrix4f(aiMatrix3x3 mat) {
	Matrix4f m;

	m.set(0, 0, mat.a1); m.set(0, 1, mat.a2); m.set(0, 2, mat.a3); m.set(0, 3, 0);
	m.set(1, 0, mat.b1); m.set(1, 1, mat.b2); m.set(1, 2, mat.b3); m.set(1, 3, 0);
	m.set(2, 0, mat.c1); m.set(2, 1, mat.c2); m.set(2, 2, mat.c3); m.set(2, 3, 0);
	m.set(3, 0, 0);      m.set(3, 1, 0);      m.set(3, 2, 0);      m.set(3, 3, 1);

	return m;
}

void MeshLoader::saveEngineModel(std::string path, std::string fileName, Mesh* mesh) {
	//Open the file
	std::ofstream output;
	output.open(path + fileName, std::ifstream::binary);

	//Save specific mesh data
	float radius = mesh->getBoundingSphereRadius();
	writeVector3f(output, mesh->getBoundingSphereCentre());
	writeFloat(output, radius);

	//Mesh Data
	MeshData* data = mesh->getData();

	unsigned int numSubData = data->getSubDataCount();

	writeUInt(output, data->getNumDimensions());

	writeUInt(output, data->getNumPositions());
	writeUInt(output, data->getNumColours());
	writeUInt(output, data->getNumTextureCoords());
	writeUInt(output, data->getNumNormals());
	writeUInt(output, data->getNumTangents());
	writeUInt(output, data->getNumBitangents());
	writeUInt(output, data->getNumIndices());
	writeUInt(output, data->getNumBones());

	writeVectorDataFloat(output, data->getPositions());
	writeVectorDataFloat(output, data->getColours());
	writeVectorDataFloat(output, data->getTextureCoords());
	writeVectorDataFloat(output, data->getNormals());
	writeVectorDataFloat(output, data->getTangents());
	writeVectorDataFloat(output, data->getBitangents());
	writeVectorDataFloat(output, data->getOthers());
	writeVectorDataUInt(output, data->getIndices());
	writeVectorDataUInt(output, data->getBoneIDs());
	writeVectorDataFloat(output, data->getBoneWeights());

	//Sub Data
	writeUInt(output, numSubData);

	for (unsigned int i = 0; i < numSubData; i++) {
		MeshData::SubData currentSubData = data->getSubData(i);
		writeUInt(output, currentSubData.baseIndex);
		writeUInt(output, currentSubData.baseVertex);
		writeUInt(output, currentSubData.count);
		writeUInt(output, currentSubData.materialIndex);
	}

	//Materials
	unsigned int numMaterials = mesh->getNumMaterials();

	writeUInt(output, numMaterials);

	for (unsigned int i = 0; i < numMaterials; i++)
		writeMaterial(output, mesh->getMaterial(i), path);

	//Skeleton
	if (mesh->hasSkeleton()) {
		writeUInt(output, 1);

		Skeleton* skeleton = mesh->getSkeleton();
		unsigned int numAnimations = skeleton->getNumAnimations();

		writeMatrix4f(output, skeleton->getGlobalInverseTransform());
		writeUInt(output, numAnimations);

		//Animations
		for (unsigned int i = 0; i < numAnimations; i++) {
			Animation* currentAnimation = skeleton->getAnimation(i);

			unsigned int numBoneAnimationData = currentAnimation->getNumBoneAnimationData();

			writeString(output, currentAnimation->getName());
			writeFloat(output, currentAnimation->getTicksPerSecond());
			writeFloat(output, currentAnimation->getDuration());
			writeUInt(output, numBoneAnimationData);

			for (unsigned int j = 0; j < numBoneAnimationData; j++) {
				BoneAnimationData* currentBoneAnimData = currentAnimation->getBoneAnimationDataByIndex(j);

				writeUInt(output, currentBoneAnimData->getBoneIndex());

				unsigned int numKeyframesPosition = currentBoneAnimData->getNumKeyframePositions();
				unsigned int numKeyframesRotation = currentBoneAnimData->getNumKeyframeRotations();
				unsigned int numKeyframesScale = currentBoneAnimData->getNumKeyframeScales();

				writeUInt(output, numKeyframesPosition);

				for (unsigned int k = 0; k < numKeyframesPosition; k++) {
					writeVector3f(output, currentBoneAnimData->getKeyframePosition(k));
					writeFloat(output, currentBoneAnimData->getKeyframePositionTime(k));
				}

				writeUInt(output, numKeyframesRotation);

				for (unsigned int k = 0; k < numKeyframesRotation; k++) {
					writeVector4f(output, currentBoneAnimData->getKeyframeRotation(k));
					writeFloat(output, currentBoneAnimData->getKeyframeRotationTime(k));
				}

				writeUInt(output, numKeyframesScale);

				for (unsigned int k = 0; k < numKeyframesScale; k++) {
					writeVector3f(output, currentBoneAnimData->getKeyframeScale(k));
					writeFloat(output, currentBoneAnimData->getKeyframeScaleTime(k));
				}
			}
		}

		//Bones
		unsigned int numBones = skeleton->getNumBones();
		writeUInt(output, numBones);
		for (unsigned int i = 0; i < numBones; i++) {
			Bone* bone = skeleton->getBone(i);

			writeString(output, bone->getName());
			writeMatrix4f(output, bone->getTransform());
			writeMatrix4f(output, bone->getOffset());

			unsigned int numChildren = bone->getNumChildren();
			writeUInt(output, numChildren);

			for (unsigned int j = 0; j < numChildren; j++)
				writeUInt(output, bone->getChild(j));
		}

		writeUInt(output, skeleton->getRootBoneIndex());
	} else
		writeUInt(output, 0);

	writeMatrix4f(output, mesh->getMatrix());

	//Close the file
	output.close();
}

Mesh* MeshLoader::loadEngineModel(std::string path, std::string fileName) {
	//Open the file
	std::ifstream input;
	input.open(path + fileName, std::ifstream::binary);

	//Save specific mesh data
	Vector3f boundingSphereCentre;
	float boundingSphereRadius;
	readVector3f(input, boundingSphereCentre);
	readFloat(input, boundingSphereRadius);

	std::vector<Material*> materials;

	unsigned int numDimensions;
	readUInt(input, numDimensions);

	//Mesh Data
	MeshData* data = new MeshData(numDimensions);

	unsigned int numPositions, numColours, numTextureCoords, numNormals, numTangents, numBitangents, numIndices, numBones;

	readUInt(input, numPositions);
	readUInt(input, numColours);
	readUInt(input, numTextureCoords);
	readUInt(input, numNormals);
	readUInt(input, numTangents);
	readUInt(input, numBitangents);
	readUInt(input, numIndices);
	readUInt(input, numBones);

	data->setNumPositions(numPositions);
	data->setNumColours(numColours);
	data->setNumTextureCoords(numTextureCoords);
	data->setNumNormals(numNormals);
	data->setNumTangents(numTangents);
	data->setNumBitangents(numBitangents);
	data->setNumIndices(numIndices);
	data->setNumBones(numBones);

	readVectorDataFloat(input, data->getPositions());
	readVectorDataFloat(input, data->getColours());
	readVectorDataFloat(input, data->getTextureCoords());
	readVectorDataFloat(input, data->getNormals());
	readVectorDataFloat(input, data->getTangents());
	readVectorDataFloat(input, data->getBitangents());
	readVectorDataFloat(input, data->getOthers());
	readVectorDataUInt(input, data->getIndices());
	readVectorDataUInt(input, data->getBoneIDs());
	readVectorDataFloat(input, data->getBoneWeights());

	//Sub Data
	unsigned int numSubData;
	readUInt(input, numSubData);

	for (unsigned int i = 0; i < numSubData; i++) {
		MeshData::SubData currentSubData;
		readUInt(input, currentSubData.baseIndex);
		readUInt(input, currentSubData.baseVertex);
		readUInt(input, currentSubData.count);
		readUInt(input, currentSubData.materialIndex);
		data->addSubData(currentSubData);
	}

	//Materials
	unsigned int numMaterials;

	readUInt(input, numMaterials);

	for (unsigned int i = 0; i < numMaterials; i++)
		readMaterial(input, materials, path);

	unsigned int hasSkeleton;
	readUInt(input, hasSkeleton);

	//Skeleton
	Skeleton* skeleton = NULL;

	if (hasSkeleton) {
		skeleton = new Skeleton();

		unsigned int numAnimations;
		Matrix4f globalInverseTransform;

		readMatrix4f(input, globalInverseTransform);
		readUInt(input, numAnimations);
		skeleton->setGlobalInverseTransform(globalInverseTransform);

		std::vector<Animation*> animations;

		//Animations
		for (unsigned int i = 0; i < numAnimations; i++) {
			std::string name;
			float ticksPerSecond;
			float duration;
			unsigned int numBoneAnimationData;

			readString(input, name);
			readFloat(input, ticksPerSecond);
			readFloat(input, duration);
			readUInt(input, numBoneAnimationData);

			Animation* currentAnimation = new Animation(name, ticksPerSecond, duration);
			std::vector<BoneAnimationData*> boneAnimData;

			for (unsigned int j = 0; j < numBoneAnimationData; j++) {
				unsigned int boneIndex;
				readUInt(input, boneIndex);

				BoneAnimationData* currentBoneAnimData = new BoneAnimationData(boneIndex);

				unsigned int numKeyframesPosition, numKeyframesRotation, numKeyramesScale;

				std::vector<Vector3f>   keyframesPositions;
				std::vector<float>      keyframesPositionsTimes;
				std::vector<Quaternion> keyframesRotations;
				std::vector<float>      keyframesRotationsTimes;
				std::vector<Vector3f>   keyframesScales;
				std::vector<float>      keyframesScalesTimes;

				readUInt(input, numKeyframesPosition);

				keyframesPositions.resize(numKeyframesPosition);
				keyframesPositionsTimes.resize(numKeyframesPosition);

				for (unsigned int k = 0; k < numKeyframesPosition; k++) {
					readVector3f(input, keyframesPositions[k]);
					readFloat(input, keyframesPositionsTimes[k]);
				}

				readUInt(input, numKeyframesRotation);

				keyframesRotations.resize(numKeyframesRotation);
				keyframesRotationsTimes.resize(numKeyframesRotation);

				for (unsigned int k = 0; k < numKeyframesRotation; k++) {
					readVector4f(input, keyframesRotations[k]);
					readFloat(input, keyframesRotationsTimes[k]);
				}

				readUInt(input, numKeyramesScale);

				keyframesScales.resize(numKeyramesScale);
				keyframesScalesTimes.resize(numKeyramesScale);

				for (unsigned int k = 0; k < numKeyramesScale; k++) {
					readVector3f(input, keyframesScales[k]);
					readFloat(input, keyframesScalesTimes[k]);
				}

				currentBoneAnimData->setKeyframePositions(keyframesPositions);
				currentBoneAnimData->setKeyframePositionsTimes(keyframesPositionsTimes);
				currentBoneAnimData->setKeyframeRotations(keyframesRotations);
				currentBoneAnimData->setKeyframeRotationsTimes(keyframesRotationsTimes);
				currentBoneAnimData->setKeyframeScales(keyframesScales);
				currentBoneAnimData->setKeyframeScalesTimes(keyframesScalesTimes);

				boneAnimData.push_back(currentBoneAnimData);
			}
			currentAnimation->setBoneData(boneAnimData);
			animations.push_back(currentAnimation);
		}
		skeleton->setAnimations(animations);

		//Bones
		unsigned int numBones;
		readUInt(input, numBones);

		std::vector<Bone*> bones;

		for (unsigned int i = 0; i < numBones; i++) {
			std::string name;
			Matrix4f transform;
			Matrix4f offset;

			readString(input, name);
			readMatrix4f(input, transform);
			readMatrix4f(input, offset);

			Bone* currentBone = new Bone(name, transform);
			currentBone->setOffset(offset);

			std::vector<unsigned int> children;

			unsigned int numChildren;
			readUInt(input, numChildren);
			children.resize(numChildren);
			for (unsigned int j = 0; j < numChildren; j++)
				readUInt(input, children[j]);

			currentBone->setChildren(children);

			bones.push_back(currentBone);
		}
		skeleton->setBones(bones);

		unsigned int rootBoneIndex;
		readUInt(input, rootBoneIndex);
		skeleton->setRootBone(rootBoneIndex);
	}

	Matrix4f meshTransform;

	readMatrix4f(input, meshTransform);

	//Close the file
	input.close();

	//Create the mesh
	Mesh* mesh = new Mesh(data);
	mesh->setSkeleton(skeleton);
	mesh->setMaterials(materials);
	mesh->setBoundingSphereCentre(boundingSphereCentre);
	mesh->setBoundingSphereRadius(boundingSphereRadius);
	mesh->setMatrix(meshTransform);

	return mesh;
}

void MeshLoader::convertToEngineModel(std::string path, std::string fileName, bool genNormals) {
	std::string newFileName = fileName.substr(0, fileName.find_last_of(".")) + ".model";
	Mesh* mesh = loadAssimpModel(path, fileName, genNormals);
	saveEngineModel(path, newFileName, mesh);
	delete mesh;
}

void MeshLoader::writeUInt(std::ofstream& output, unsigned int value) {
	output.write(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

void MeshLoader::writeFloat(std::ofstream& output, float value) {
	output.write(reinterpret_cast<char*>(&value), sizeof(float));
}

void MeshLoader::writeVectorDataFloat(std::ofstream& output, std::vector<float>& data) {
	unsigned int numItems = data.size();
	writeUInt(output, numItems);
	if (numItems > 0)
		output.write(reinterpret_cast<char*>(data.data()), numItems * sizeof(float));
}

void MeshLoader::writeVectorDataUInt(std::ofstream& output, std::vector<unsigned int>& data) {
	unsigned int numItems = data.size();
	output.write(reinterpret_cast<char*>(&numItems), sizeof(unsigned int));
	if (numItems > 0)
		output.write(reinterpret_cast<char*>(data.data()), numItems * sizeof(unsigned int));
}

void MeshLoader::writeMaterial(std::ofstream& output, Material* material, std::string path) {
	writeVector4f(output, material->ambientColour);
	writeVector4f(output, material->diffuseColour);
	writeVector4f(output, material->specularColour);
	writeTexture(output, material->ambientTexture, path);
	writeTexture(output, material->diffuseTexture, path);
	writeTexture(output, material->specularTexture, path);
	writeTexture(output, material->normalMap, path);
	writeTexture(output, material->parallaxMap, path);
	writeFloat(output, material->parallaxScale);
	writeFloat(output, material->shininess);
}

void MeshLoader::writeTexture(std::ofstream& output, Texture* texture, std::string path) {
	if (texture == NULL)
		writeString(output, "NULL");
	else
		writeString(output, StrUtils::remove(texture->getPath(), path));
}

void MeshLoader::writeVector3f(std::ofstream& output, Vector3f vector) {
	output.write(reinterpret_cast<char*>(vector.data()), 3 * sizeof(float));
}

void MeshLoader::writeVector4f(std::ofstream& output, Vector4f vector) {
	output.write(reinterpret_cast<char*>(vector.data()), 4 * sizeof(float));
}

void MeshLoader::writeMatrix4f(std::ofstream& output, Matrix4f matrix) {
	output.write(reinterpret_cast<char*>(matrix.data()), 16 * sizeof(float));
}

void MeshLoader::writeString(std::ofstream& output, std::string string) {
	size_t length = string.size();
	output.write(reinterpret_cast<char*>(&length), sizeof(size_t));
	output.write(string.c_str(), length);
}

void MeshLoader::readVectorDataFloat(std::ifstream& input, std::vector<float>& data) {
	unsigned int numItems = data.size();
	readUInt(input, numItems);
	if (numItems > 0) {
		data.resize(numItems);
		input.read(reinterpret_cast<char*>(data.data()), numItems * sizeof(float));
	}
}

void MeshLoader::readVectorDataUInt(std::ifstream& input, std::vector<unsigned int>& data) {
	unsigned int numItems = data.size();
	readUInt(input, numItems);
	if (numItems > 0) {
		data.resize(numItems);
		input.read(reinterpret_cast<char*>(data.data()), numItems * sizeof(unsigned int));
	}
}

void MeshLoader::readMaterial(std::ifstream& input, std::vector<Material*>& materials, std::string path) {
	Material* material = new Material();
	readVector4f(input, material->ambientColour);
	readVector4f(input, material->diffuseColour);
	readVector4f(input, material->specularColour);
	material->ambientTexture = readTexture(input, path);
	material->diffuseTexture = readTexture(input, path);
	material->specularTexture = readTexture(input, path);
	material->normalMap = readTexture(input, path);
	material->parallaxMap = readTexture(input, path);
	readFloat(input, material->parallaxScale);
	readFloat(input, material->shininess);
	materials.push_back(material);
}

Texture* MeshLoader::readTexture(std::ifstream& input, std::string path) {
	std::string value;
	readString(input, value);
	if (value == std::string("NULL"))
		return NULL;
	else
		return Texture::loadTexture(path + value);
}

void MeshLoader::readUInt(std::ifstream& input, unsigned int& value) {
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

void MeshLoader::readFloat(std::ifstream& input, float& value) {
	input.read(reinterpret_cast<char*>(&value), sizeof(float));
}


void MeshLoader::readVector3f(std::ifstream& input, Vector3f& vector) {
	input.read(reinterpret_cast<char*>(vector.data()), 3 * sizeof(float));
}

void MeshLoader::readVector4f(std::ifstream& input, Vector4f& vector) {
	input.read(reinterpret_cast<char*>(vector.data()), 4 * sizeof(float));
}

void MeshLoader::readMatrix4f(std::ifstream& input, Matrix4f& matrix) {
	input.read(reinterpret_cast<char*>(matrix.data()), 16 * sizeof(float));
}

void MeshLoader::readString(std::ifstream& input, std::string& string) {
	size_t length;
	input.read(reinterpret_cast<char*>(&length), sizeof(size_t));
	char* temp = new char[length + 1];
	input.read(temp, length);
	temp[length] = '\0';
	string = temp;
	delete [] temp;
}
