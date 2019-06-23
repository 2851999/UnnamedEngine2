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

#include "../../utils/Logging.h"

/*****************************************************************************
 * The MeshLoader class
 *****************************************************************************/

bool MeshLoader::loadDiffuseTexturesAsSRGB = true;

void MeshLoader::addChildren(const aiNode* node, std::unordered_map<const aiNode*, const aiBone*>& nodes) {
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

Mesh* MeshLoader::loadModel(std::string path, std::string fileName, bool pbr) {
	if (utils_string::strEndsWith(fileName, ".model"))
		return loadEngineModel(path, fileName);
	else
		return loadAssimpModel(path, fileName, pbr);
}

Mesh* MeshLoader::loadAssimpModel(std::string path, std::string fileName, bool pbr, bool genNormals) {
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
			for (unsigned int b = 0; b < currentMesh->mNumFaces; ++b) {
				//Thee current face of the current mesh
				struct aiFace& currentFace = currentMesh->mFaces[b];

				//Goes through each vertex of the face, this assumes the model is triangulated i.e. there are
				//3 vertices per face in the mesh
				for (int c = 0; c < 3; ++c)
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
			std::unordered_map<const aiNode*, const aiBone*> necessaryNodes;
			std::unordered_map<std::string, unsigned int>    boneIndices;
			std::vector<MeshData::VertexBoneData> verticesBonesData;
			//Place to store all of the created bones
			std::vector<Bone*> bones;

			verticesBonesData.resize(numVertices);

			addChildren(scene->mRootNode, necessaryNodes);

			//Go through each mesh
			for (unsigned int a = 0; a < scene->mNumMeshes; ++a) {
				//The current mesh instance
				const aiMesh* currentMesh = scene->mMeshes[a];
				//Go through each bone in the mesh
				for (unsigned int b = 0; b < currentMesh->mNumBones; ++b) {
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

			for (unsigned int a = 0; a < scene->mNumMeshes; ++a) {
				//Pointer to the current mesh being read
				const struct aiMesh* currentMesh = scene->mMeshes[a];
				for (unsigned int b = 0; b < currentMesh->mNumBones; ++b) {
					unsigned int boneIndex = boneIndices[std::string(currentMesh->mBones[b]->mName.C_Str())];

					for (unsigned int c = 0; c < currentMesh->mBones[b]->mNumWeights; ++c) {
						unsigned int vertexID = currentData->getSubData(a).baseVertex + currentMesh->mBones[b]->mWeights[c].mVertexId;
						float weight = currentMesh->mBones[b]->mWeights[c].mWeight;
						verticesBonesData[vertexID].addBoneData(boneIndex, weight);
					}
				}
			}
			for (unsigned int a = 0; a < verticesBonesData.size(); ++a) {
				for (unsigned int b = 0; b < NUM_BONES_PER_VERTEX; ++b)
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

			//Check if number of bones in model exceeds the maximum supported
			if (bones.size() > Skeleton::SKINNING_MAX_BONES)
				Logger::log("Model exceeds maximum number of bones for rendering", "MeshLoader", LogType::Warning);

			//Assign the bones in the skeleton
			skeleton->setBones(bones);
			skeleton->setRootBone(rootBoneIndex);

			//Place to store all of the created animations
			std::vector<Animation*> animations;

			//Make room for all of the animations
			animations.resize(scene->mNumAnimations);

			//Now go through all of the animations
			for (unsigned int b = 0; b < scene->mNumAnimations; ++b) {
				//Create the current animation
				Animation* currentAnimation = new Animation(std::string(scene->mAnimations[b]->mName.C_Str()), scene->mAnimations[b]->mTicksPerSecond, scene->mAnimations[b]->mDuration);
				//The current aiAnimation instance
				const aiAnimation* currentAssimpAnim = scene->mAnimations[b];
				//Place to store the bone data
				std::vector<BoneAnimationData*> boneData;
				//Make room for the bone animation data
				boneData.resize(currentAssimpAnim->mNumChannels);
				//Go through each animation channel
				for (unsigned int c = 0; c < currentAssimpAnim->mNumChannels; ++c) {
					//The current aiNodeAnim instance
					const aiNodeAnim* currentAnimNode = currentAssimpAnim->mChannels[c];
					//Create the bone animation data instance
					BoneAnimationData* currentBoneData = new BoneAnimationData(boneIndices[std::string(currentAnimNode->mNodeName.C_Str())], currentAnimNode->mNumPositionKeys, currentAnimNode->mNumRotationKeys, currentAnimNode->mNumScalingKeys);
					//Go through and assign all of the data
					for (unsigned int d = 0; d < currentAnimNode->mNumPositionKeys; ++d)
						currentBoneData->setKeyframePosition(d, toVector3f(currentAnimNode->mPositionKeys[d].mValue), currentAnimNode->mPositionKeys[d].mTime);
					for (unsigned int d = 0; d < currentAnimNode->mNumRotationKeys; ++d)
						currentBoneData->setKeyframeRotation(d, toQuaternion(currentAnimNode->mRotationKeys[d].mValue), currentAnimNode->mRotationKeys[d].mTime);
					for (unsigned int d = 0; d < currentAnimNode->mNumScalingKeys; ++d)
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

		//Calculate and assign the bounding sphere for the mesh
		mesh->setBoundingSphere(currentData->calculateBoundingSphere());
		mesh->setCullingEnabled(true);

		//Load and add the materials
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
			//Load the current material
			Material* material = loadAssimpMaterial(path, fileName, scene->mMaterials[i], pbr);

			if (i == 0) {
				delete mesh->getMaterial();
				mesh->setMaterial(material);
			} else
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

Material* MeshLoader::loadAssimpMaterial(std::string path, std::string fileName, const aiMaterial* mat, bool pbr) {
	//Create the material instance
	Material* material = new Material(pbr);

	//Load and assign the textures
	material->setAmbient(loadAssimpTexture(path, mat, aiTextureType_AMBIENT));
	material->setDiffuse(loadAssimpTexture(path, mat, aiTextureType_DIFFUSE, loadDiffuseTexturesAsSRGB));
	material->setSpecular(loadAssimpTexture(path, mat, aiTextureType_SPECULAR));

	if (pbr)
		material->setShininess(loadAssimpTexture(path, mat, aiTextureType_SHININESS)); //No standard way of using this for phong shading

	//Check to see whether the material has a normal map
	if (mat->GetTextureCount(aiTextureType_NORMALS) != 0)
		material->setNormalMap(loadAssimpTexture(path, mat, aiTextureType_NORMALS));
	else if (utils_string::strEndsWith(fileName, ".obj") && (mat->GetTextureCount(aiTextureType_HEIGHT) != 0))
		material->setNormalMap(loadAssimpTexture(path, mat, aiTextureType_HEIGHT));

	if (mat->GetTextureCount(aiTextureType_DISPLACEMENT) != 0)
		material->setParallaxMap(loadAssimpTexture(path, mat, aiTextureType_DISPLACEMENT));

	//Load and assign the colours
	material->setAmbient(loadAssimpColour(mat, AI_MATKEY_COLOR_AMBIENT));
	material->setDiffuse(loadAssimpColour(mat, AI_MATKEY_COLOR_DIFFUSE));
	material->setSpecular(loadAssimpColour(mat, AI_MATKEY_COLOR_SPECULAR));

	float value;
	if ((mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) && value != 0.0f) {
		if (pbr)
			value /= 4.0f; //Assimp multiplies this by 4
		material->setShininess(value);
	}

	return material;
}

Texture* MeshLoader::loadAssimpTexture(std::string path, const aiMaterial* material, const aiTextureType type, bool srgb) {
	//Check whether the texture is defined
	if (material->GetTextureCount(type) != 0) {
		//Get the path of the texture
		aiString p;
		material->GetTexture(type, 0, &p);

		//Return the loaded texture
		return Texture::loadTexture(path + utils_string::str(p.C_Str()), TextureParameters().setSRGB(srgb));
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
	BinaryFile file(path + fileName);
	file.open(BinaryFile::Operation::WRITE);

	//Save specific mesh data
	float radius = mesh->getBoundingSphereRadius();
	file.writeVector3f( mesh->getBoundingSphereCentre());
	file.writeFloat( radius);

	//Mesh Data
	MeshData* data = mesh->getData();

	unsigned int numSubData = data->getSubDataCount();

	file.writeUInt(data->getNumDimensions());

	file.writeUInt(data->getNumPositions());
	file.writeUInt(data->getNumColours());
	file.writeUInt(data->getNumTextureCoords());
	file.writeUInt(data->getNumNormals());
	file.writeUInt(data->getNumTangents());
	file.writeUInt(data->getNumBitangents());
	file.writeUInt(data->getNumIndices());
	file.writeUInt(data->getNumBones());

	file.writeVectorDataFloat(data->getPositions());
	file.writeVectorDataFloat(data->getColours());
	file.writeVectorDataFloat(data->getTextureCoords());
	file.writeVectorDataFloat(data->getNormals());
	file.writeVectorDataFloat(data->getTangents());
	file.writeVectorDataFloat(data->getBitangents());
	file.writeVectorDataFloat(data->getOthers());
	file.writeVectorDataUInt(data->getIndices());
	file.writeVectorDataUInt(data->getBoneIDs());
	file.writeVectorDataFloat(data->getBoneWeights());

	//Sub Data
	file.writeUInt(numSubData);

	for (unsigned int i = 0; i < numSubData; i++) {
		MeshData::SubData currentSubData = data->getSubData(i);
		file.writeUInt(currentSubData.baseIndex);
		file.writeUInt(currentSubData.baseVertex);
		file.writeUInt(currentSubData.count);
		file.writeUInt(currentSubData.materialIndex);
	}

	//Materials
	unsigned int numMaterials = mesh->getNumMaterials();

	file.writeUInt(numMaterials);

	for (unsigned int i = 0; i < numMaterials; i++)
		writeMaterial(file, mesh->getMaterial(i), path);

	//Skeleton
	if (mesh->hasSkeleton()) {
		file.writeUInt(1);

		Skeleton* skeleton = mesh->getSkeleton();
		unsigned int numAnimations = skeleton->getNumAnimations();

		file.writeMatrix4f(skeleton->getGlobalInverseTransform());
		file.writeUInt(numAnimations);

		//Animations
		for (unsigned int i = 0; i < numAnimations; i++) {
			Animation* currentAnimation = skeleton->getAnimation(i);

			unsigned int numBoneAnimationData = currentAnimation->getNumBoneAnimationData();

			file.writeString(currentAnimation->getName());
			file.writeFloat(currentAnimation->getTicksPerSecond());
			file.writeFloat(currentAnimation->getDuration());
			file.writeUInt(numBoneAnimationData);

			for (unsigned int j = 0; j < numBoneAnimationData; j++) {
				BoneAnimationData* currentBoneAnimData = currentAnimation->getBoneAnimationDataByIndex(j);

				file.writeUInt(currentBoneAnimData->getBoneIndex());

				unsigned int numKeyframesPosition = currentBoneAnimData->getNumKeyframePositions();
				unsigned int numKeyframesRotation = currentBoneAnimData->getNumKeyframeRotations();
				unsigned int numKeyframesScale = currentBoneAnimData->getNumKeyframeScales();

				file.writeUInt(numKeyframesPosition);

				for (unsigned int k = 0; k < numKeyframesPosition; k++) {
					file.writeVector3f(currentBoneAnimData->getKeyframePosition(k));
					file.writeFloat(currentBoneAnimData->getKeyframePositionTime(k));
				}

				file.writeUInt(numKeyframesRotation);

				for (unsigned int k = 0; k < numKeyframesRotation; k++) {
					file.writeVector4f(currentBoneAnimData->getKeyframeRotation(k));
					file.writeFloat(currentBoneAnimData->getKeyframeRotationTime(k));
				}

				file.writeUInt(numKeyframesScale);

				for (unsigned int k = 0; k < numKeyframesScale; k++) {
					file.writeVector3f(currentBoneAnimData->getKeyframeScale(k));
					file.writeFloat(currentBoneAnimData->getKeyframeScaleTime(k));
				}
			}
		}

		//Bones
		unsigned int numBones = skeleton->getNumBones();
		file.writeUInt(numBones);
		for (unsigned int i = 0; i < numBones; i++) {
			Bone* bone = skeleton->getBone(i);

			file.writeString(bone->getName());
			file.writeMatrix4f(bone->getTransform());
			file.writeMatrix4f(bone->getOffset());

			unsigned int numChildren = bone->getNumChildren();
			file.writeUInt(numChildren);

			for (unsigned int j = 0; j < numChildren; j++)
				file.writeUInt(bone->getChild(j));
		}

		file.writeUInt(skeleton->getRootBoneIndex());
	} else
		file.writeUInt(0);

	file.writeMatrix4f(mesh->getMatrix());

	//Close the file
	file.close();
}

Mesh* MeshLoader::loadEngineModel(std::string path, std::string fileName) {
	//Open the file
	BinaryFile file(path + fileName);
	file.open(BinaryFile::Operation::READ);

	//Save specific mesh data
	Vector3f boundingSphereCentre;
	float boundingSphereRadius;
	file.readVector3f(boundingSphereCentre);
	file.readFloat(boundingSphereRadius);

	std::vector<Material*> materials;

	unsigned int numDimensions;
	file.readUInt(numDimensions);

	//Mesh Data
	MeshData* data = new MeshData(numDimensions);

	unsigned int numPositions, numColours, numTextureCoords, numNormals, numTangents, numBitangents, numIndices, numBones;

	file.readUInt(numPositions);
	file.readUInt(numColours);
	file.readUInt(numTextureCoords);
	file.readUInt(numNormals);
	file.readUInt(numTangents);
	file.readUInt(numBitangents);
	file.readUInt(numIndices);
	file.readUInt(numBones);

	data->setNumPositions(numPositions);
	data->setNumColours(numColours);
	data->setNumTextureCoords(numTextureCoords);
	data->setNumNormals(numNormals);
	data->setNumTangents(numTangents);
	data->setNumBitangents(numBitangents);
	data->setNumIndices(numIndices);
	data->setNumBones(numBones);

	file.readVectorDataFloat(data->getPositions());
	file.readVectorDataFloat(data->getColours());
	file.readVectorDataFloat(data->getTextureCoords());
	file.readVectorDataFloat(data->getNormals());
	file.readVectorDataFloat(data->getTangents());
	file.readVectorDataFloat(data->getBitangents());
	file.readVectorDataFloat(data->getOthers());
	file.readVectorDataUInt(data->getIndices());
	file.readVectorDataUInt(data->getBoneIDs());
	file.readVectorDataFloat(data->getBoneWeights());

	//Sub Data
	unsigned int numSubData;
	file.readUInt(numSubData);

	for (unsigned int i = 0; i < numSubData; i++) {
		MeshData::SubData currentSubData;
		file.readUInt(currentSubData.baseIndex);
		file.readUInt(currentSubData.baseVertex);
		file.readUInt(currentSubData.count);
		file.readUInt(currentSubData.materialIndex);
		data->addSubData(currentSubData);
	}

	//Materials
	unsigned int numMaterials;

	file.readUInt(numMaterials);

	for (unsigned int i = 0; i < numMaterials; i++)
		readMaterial(file, materials, path);

	unsigned int hasSkeleton;
	file.readUInt(hasSkeleton);

	//Skeleton
	Skeleton* skeleton = NULL;

	if (hasSkeleton) {
		skeleton = new Skeleton();

		unsigned int numAnimations;
		Matrix4f globalInverseTransform;

		file.readMatrix4f(globalInverseTransform);
		file.readUInt(numAnimations);
		skeleton->setGlobalInverseTransform(globalInverseTransform);

		std::vector<Animation*> animations;

		//Animations
		for (unsigned int i = 0; i < numAnimations; i++) {
			std::string name;
			float ticksPerSecond;
			float duration;
			unsigned int numBoneAnimationData;

			file.readString(name);
			file.readFloat(ticksPerSecond);
			file.readFloat(duration);
			file.readUInt(numBoneAnimationData);

			Animation* currentAnimation = new Animation(name, ticksPerSecond, duration);
			std::vector<BoneAnimationData*> boneAnimData;

			for (unsigned int j = 0; j < numBoneAnimationData; j++) {
				unsigned int boneIndex;
				file.readUInt(boneIndex);

				BoneAnimationData* currentBoneAnimData = new BoneAnimationData(boneIndex);

				unsigned int numKeyframesPosition, numKeyframesRotation, numKeyramesScale;

				std::vector<Vector3f>   keyframesPositions;
				std::vector<float>      keyframesPositionsTimes;
				std::vector<Quaternion> keyframesRotations;
				std::vector<float>      keyframesRotationsTimes;
				std::vector<Vector3f>   keyframesScales;
				std::vector<float>      keyframesScalesTimes;

				file.readUInt(numKeyframesPosition);

				keyframesPositions.resize(numKeyframesPosition);
				keyframesPositionsTimes.resize(numKeyframesPosition);

				for (unsigned int k = 0; k < numKeyframesPosition; k++) {
					file.readVector3f(keyframesPositions[k]);
					file.readFloat(keyframesPositionsTimes[k]);
				}

				file.readUInt(numKeyframesRotation);

				keyframesRotations.resize(numKeyframesRotation);
				keyframesRotationsTimes.resize(numKeyframesRotation);

				for (unsigned int k = 0; k < numKeyframesRotation; k++) {
					file.readVector4f(keyframesRotations[k]);
					file.readFloat(keyframesRotationsTimes[k]);
				}

				file.readUInt(numKeyramesScale);

				keyframesScales.resize(numKeyramesScale);
				keyframesScalesTimes.resize(numKeyramesScale);

				for (unsigned int k = 0; k < numKeyramesScale; k++) {
					file.readVector3f(keyframesScales[k]);
					file.readFloat(keyframesScalesTimes[k]);
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
		file.readUInt(numBones);

		std::vector<Bone*> bones;

		for (unsigned int i = 0; i < numBones; i++) {
			std::string name;
			Matrix4f transform;
			Matrix4f offset;

			file.readString(name);
			file.readMatrix4f(transform);
			file.readMatrix4f(offset);

			Bone* currentBone = new Bone(name, transform);
			currentBone->setOffset(offset);

			std::vector<unsigned int> children;

			unsigned int numChildren;
			file.readUInt(numChildren);
			children.resize(numChildren);
			for (unsigned int j = 0; j < numChildren; j++)
				file.readUInt(children[j]);

			currentBone->setChildren(children);

			bones.push_back(currentBone);
		}
		skeleton->setBones(bones);

		unsigned int rootBoneIndex;
		file.readUInt(rootBoneIndex);
		skeleton->setRootBone(rootBoneIndex);
	}

	Matrix4f meshTransform;

	file.readMatrix4f(meshTransform);

	//Close the file
	file.close();

	//Create the mesh
	Mesh* mesh = new Mesh(data);
	mesh->setSkeleton(skeleton);
	mesh->setMaterials(materials);
	mesh->setCullingEnabled(true);
	mesh->setBoundingSphere(Sphere(boundingSphereCentre, boundingSphereRadius));
	mesh->setMatrix(meshTransform);

	return mesh;
}

void MeshLoader::convertToEngineModel(std::string path, std::string fileName, bool pbr, bool genNormals) {
	std::string newFileName = fileName.substr(0, fileName.find_last_of(".")) + ".model";
	Mesh* mesh = loadAssimpModel(path, fileName, pbr, genNormals);
	saveEngineModel(path, newFileName, mesh);
	delete mesh;
}

void MeshLoader::writeMaterial(BinaryFile& file, Material* material, std::string path) {
	file.writeVector4f(material->getAmbientColour());
	file.writeVector4f(material->getDiffuseColour());
	file.writeVector4f(material->getSpecularColour());
	writeTexture(file, material->getAmbientTexture(), path);
	writeTexture(file, material->getDiffuseTexture(), path);
	writeTexture(file, material->getSpecularTexture(), path);
	writeTexture(file, material->getShininessTexture(), path);
	writeTexture(file, material->getNormalMap(), path);
	writeTexture(file, material->getParallaxMap(), path);
	file.writeFloat(material->getParallaxScale());
	file.writeFloat(material->getShininess());
}

void MeshLoader::writeTexture(BinaryFile& file, Texture* texture, std::string path) {
	if (texture == NULL)
		file.writeString("NULL");
	else
		file.writeString(utils_string::remove(texture->getPath(), path));
}

void MeshLoader::readMaterial(BinaryFile& file, std::vector<Material*>& materials, std::string path) {
	Material* material = new Material();
	Vector4f readVectorValue;
	file.readVector4f(readVectorValue);
	material->setAmbient(readVectorValue);
	file.readVector4f(readVectorValue);
	material->setDiffuse(readVectorValue);
	file.readVector4f(readVectorValue);
	material->setSpecular(readVectorValue);
	material->setAmbient(readTexture(file, path));
	material->setDiffuse(readTexture(file, path, loadDiffuseTexturesAsSRGB));
	material->setSpecular(readTexture(file, path));
	material->setShininess(readTexture(file, path));
	material->setNormalMap(readTexture(file, path));
	material->setParallaxMap(readTexture(file, path));
	float readFloatValue;
	file.readFloat(readFloatValue);
	material->setParallaxScale(readFloatValue);
	file.readFloat(readFloatValue);
	material->setShininess(readFloatValue);
	materials.push_back(material);
}

Texture* MeshLoader::readTexture(BinaryFile& file, std::string path, bool srgb) {
	std::string value;
	file.readString(value);

	if (value == std::string("NULL"))
		return NULL;
	else
		return Texture::loadTexture(path + value, TextureParameters().setSRGB(srgb));
}
