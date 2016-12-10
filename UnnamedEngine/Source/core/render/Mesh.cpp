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

#include "Mesh.h"

#include "Renderer.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The MeshData class
 *****************************************************************************/

void MeshData::addPosition(Vector2f position) {
	//Check to see whether it should be separated
	if (separatePositions()) {
		positions.push_back(position.getX());
		positions.push_back(position.getY());
	} else {
		others.push_back(position.getX());
		others.push_back(position.getY());
	}

	numPositions++;
}

void MeshData::addPosition(Vector3f position) {
	//Check to see whether it should be separated
	if (separatePositions()) {
		positions.push_back(position.getX());
		positions.push_back(position.getY());
		positions.push_back(position.getZ());
	} else {
		others.push_back(position.getX());
		others.push_back(position.getY());
		others.push_back(position.getZ());
	}

	numPositions++;
}

void MeshData::addColour(Colour colour) {
	//Check to see whether it should be separated
	if (separateColours()) {
		colours.push_back(colour.getR());
		colours.push_back(colour.getG());
		colours.push_back(colour.getB());
		colours.push_back(colour.getA());
	} else {
		others.push_back(colour.getR());
		others.push_back(colour.getG());
		others.push_back(colour.getB());
		others.push_back(colour.getA());
	}

	numColours++;
}

void MeshData::addTextureCoord(Vector2f textureCoord) {
	//Check to see whether it should be separated
	if (separateTextureCoords()) {
		textureCoords.push_back(textureCoord.getX());
		textureCoords.push_back(textureCoord.getY());
	} else {
		others.push_back(textureCoord.getX());
		others.push_back(textureCoord.getY());
	}

	numTextureCoords++;
}

void MeshData::addNormal(Vector3f normal) {
	//Check to see whether it should be separated
	if (separateNormals()) {
		normals.push_back(normal.getX());
		normals.push_back(normal.getY());
		normals.push_back(normal.getZ());
	} else {
		others.push_back(normal.getX());
		others.push_back(normal.getY());
		others.push_back(normal.getZ());
	}

	numNormals++;
}

void MeshData::addTangent(Vector3f tangent) {
	//Check to see whether it should be separated
	if (separateTangents()) {
		tangents.push_back(tangent.getX());
		tangents.push_back(tangent.getY());
		tangents.push_back(tangent.getZ());
	} else {
		others.push_back(tangent.getX());
		others.push_back(tangent.getY());
		others.push_back(tangent.getZ());
	}

	numTangents++;
}

void MeshData::addBitangent(Vector3f bitangent) {
	//Check to see whether it should be separated
	if (separateBitangents()) {
		bitangents.push_back(bitangent.getX());
		bitangents.push_back(bitangent.getY());
		bitangents.push_back(bitangent.getZ());
	} else {
		others.push_back(bitangent.getX());
		others.push_back(bitangent.getY());
		others.push_back(bitangent.getZ());
	}

	numBitangents++;
}

void MeshData::addBoneData(unsigned int boneID, float boneWeight) {
	boneIDs.push_back(boneID);
	boneWeights.push_back(boneWeight);
	numBones++;
}

/*****************************************************************************
 * The MeshRenderData class
 *****************************************************************************/

void MeshRenderData::setup(MeshData* data, RenderShader* renderShader) {
	//The shader used for rendering
	Shader* shader = renderShader->getShader();
	//Determine the number of vertices
	if (data->hasIndices()) {
		//As the data has indices, this will determine the number of vertices
		numVertices = data->getNumIndices();
		hasIndices = true;
	} else {
		//As there are no indices, the number of positions will be the number of vertices
		numVertices = data->getNumPositions();
		hasIndices = false;
	}

	//Create the RenderData instance
	renderData = new RenderData(GL_TRIANGLES, numVertices);

	//Setup positions
	if (data->hasPositions() && data->separatePositions()) {
		vboPositions = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getPositions().size() * sizeof(data->getPositions()[0]), data->getPositions(), usagePositions);
		vboPositions->addAttribute(shader->getAttributeLocation("Position"), data->getNumDimensions());
		renderData->addVBO(vboPositions);
	}

	//Setup colours
	if (data->hasColours() && data->separateColours()) {
		vboColours = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getColours().size() * sizeof(data->getColours()[0]), data->getColours(), usageColours);
		vboColours->addAttribute(shader->getAttributeLocation("Colour"), 4);
		renderData->addVBO(vboColours);
	}

	//Setup texture coordinates
	if (data->hasTextureCoords() && data->separateTextureCoords()) {
		vboTextureCoords = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getTextureCoords().size() * sizeof(data->getTextureCoords()[0]), data->getTextureCoords(), usageTextureCoords);
		vboTextureCoords->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);
		renderData->addVBO(vboTextureCoords);
	}

	//Setup normals
	if (data->hasNormals() && data->separateNormals()) {
		vboNormals = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getNormals().size() * sizeof(data->getNormals()[0]), data->getNormals(), usageNormals);
		vboNormals->addAttribute(shader->getAttributeLocation("Normal"), 3);
		renderData->addVBO(vboNormals);
	}

	//Setup tangents
	if (data->hasTangents() && data->separateTangents()) {
		vboTangents = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getTangents().size() * sizeof(data->getTangents()[0]), data->getTangents(), usageTangents);
		vboTangents->addAttribute(shader->getAttributeLocation("Tangent"), 3);
		renderData->addVBO(vboTangents);
	}

	//Setup bitangents
	if (data->hasBitangents() && data->separateBitangents()) {
		vboBitangents = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getBitangents().size() * sizeof(data->getBitangents()[0]), data->getBitangents(), usageBitangents);
		vboBitangents->addAttribute(shader->getAttributeLocation("Bitangent"), 3);
		renderData->addVBO(vboBitangents);
	}

	//Setup bones
	if (data->hasBones()) {
		vboBoneIDs = new VBO<unsigned int>(GL_ARRAY_BUFFER, data->getBoneIDs().size() * sizeof(data->getBoneIDs()[0]), data->getBoneIDs(), GL_STATIC_DRAW);
		vboBoneIDs->addAttribute(shader->getAttributeLocation("BoneIDs"), 4);
		renderData->addVBO(vboBoneIDs);

		vboBoneWeights = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getBoneWeights().size() * sizeof(data->getBoneWeights()[0]), data->getBoneWeights(), GL_STATIC_DRAW);
		vboBoneWeights->addAttribute(shader->getAttributeLocation("BoneWeights"), 4);
		renderData->addVBO(vboBoneWeights);
	}

	//Check to see whether the 'other' VBO is required
	if (data->hasOthers()) {

		vboOthers = new VBO<GLfloat>(GL_ARRAY_BUFFER, data->getOthers().size() * sizeof(data->getOthers()[0]), data->getOthers(), usageOthers);

		if (data->hasPositions() && ! data->separatePositions())
			vboOthers->addAttribute(shader->getAttributeLocation("Position"), data->getNumDimensions());

		if (data->hasColours() && ! data->separateColours())
			vboOthers->addAttribute(shader->getAttributeLocation("Colour"), 4);

		if (data->hasTextureCoords() && ! data->separateTextureCoords())
			vboOthers->addAttribute(shader->getAttributeLocation("TextureCoordinate"), 2);

		if (data->hasNormals() && ! data->separateNormals())
			vboOthers->addAttribute(shader->getAttributeLocation("Normal"), 3);

		if (data->hasTangents() && ! data->separateTangents())
			vboOthers->addAttribute(shader->getAttributeLocation("Tangent"), 3);

		if (data->hasBitangents() && ! data->separateBitangents())
			vboOthers->addAttribute(shader->getAttributeLocation("Bitangent"), 3);

		renderData->addVBO(vboOthers);
	}

	//Check to see whether indices are needed
	if (data->hasIndices()) {
		vboIndices = new VBO<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, data->getNumIndices() * sizeof(data->getIndices()[0]), data->getIndices(), usageIndices);
		renderData->setIndicesVBO(vboIndices);
	}

	//Setup the render data
	renderData->setup();
}

void MeshRenderData::render() {
	renderData->render();
}

void MeshRenderData::updatePositions(MeshData* data) {
	//Determine the number of vertices
	if (data->hasIndices()) {
		//As the data has indices, this will determine the number of vertices
		numVertices = data->getNumIndices();
		hasIndices = true;
	} else {
		//As there are no indices, the number of positions will be the number of vertices
		numVertices = data->getNumPositions();
		hasIndices = false;
	}

	glBindVertexArray(renderData->getVAO());

	vboPositions->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateColours() {
	glBindVertexArray(renderData->getVAO());

	vboColours->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateTextureCoords() {
	glBindVertexArray(renderData->getVAO());

	vboTextureCoords->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateNormals() {
	glBindVertexArray(renderData->getVAO());

	vboNormals->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateTangents() {
	glBindVertexArray(renderData->getVAO());

	vboTangents->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateBitangents() {
	glBindVertexArray(renderData->getVAO());

	vboBitangents->update();

	glBindVertexArray(0);
}

void MeshRenderData::updateIndices(MeshData* data) {
	//As the data has indices, this will determine the number of vertices
	numVertices = data->getNumIndices();
	hasIndices = true;

	renderData->setCount(numVertices);

	glBindVertexArray(renderData->getVAO());

	vboIndices->update();

	glBindVertexArray(0);
}

void MeshRenderData::destroy() {
	delete renderData;
	delete vboPositions;
	delete vboColours;
	delete vboTextureCoords;
	delete vboNormals;
	delete vboTangents;
	delete vboBitangents;
	delete vboIndices;
}

/*****************************************************************************
 * The Mesh class
 *****************************************************************************/

Mesh::Mesh(MeshData* data) {
	this->data = data;
	//Add the default material
	this->addMaterial(new Material());

	transform.setIdentity();
}

Mesh::~Mesh() {
	//Delete the created resources
	if (hasMaterial()) {
		for (Material* material : materials)
			delete material;
		materials.clear();
	}
	delete renderData;
	delete data;
}

void Mesh::addChildren(const aiNode* node, std::map<const aiNode*, const aiBone*>& nodes) {
	if (nodes.count(node) == 0)
		nodes.insert(std::pair<const aiNode*, const aiBone*>(node, NULL));
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		addChildren(node->mChildren[i], nodes);
}

const aiNode* Mesh::findMeshNode(const aiNode* parent) {
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

const aiMatrix4x4 Mesh::calculateMatrix(const aiNode* current, aiMatrix4x4 currentMatrix) {
	currentMatrix = currentMatrix * current->mTransformation;
	if (current->mParent)
		currentMatrix = calculateMatrix(current->mParent, currentMatrix);
	return currentMatrix;
}

Mesh* Mesh::loadModel(std::string path, std::string fileName) {
	//Load the file using Assimp
	const struct aiScene* scene = aiImportFile((path + fileName).c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices); //aiProcess_JoinIdenticalVertices aiProcessPreset_TargetRealtime_MaxQuality
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

		//Load and add the materials
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			//Load the current material
			Material* material = loadMaterial(path, fileName, scene->mMaterials[i]);

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

Material* Mesh::loadMaterial(std::string path, std::string fileName, const aiMaterial* mat) {
	//Create the material instance
	Material* material = new Material();

	//Load and assign the textures
	material->ambientTexture  = loadTexture(path, mat, aiTextureType_AMBIENT);
	material->diffuseTexture  = loadTexture(path, mat, aiTextureType_DIFFUSE);
	material->specularTexture = loadTexture(path, mat, aiTextureType_SPECULAR);

	//Check to see whether the material has a normal map
	if (mat->GetTextureCount(aiTextureType_NORMALS) != 0)
		material->normalMap = loadTexture(path, mat, aiTextureType_NORMALS);
	else if (StrUtils::strEndsWith(fileName, ".obj") && (mat->GetTextureCount(aiTextureType_HEIGHT) != 0))
		material->normalMap = loadTexture(path, mat, aiTextureType_HEIGHT);

	if (mat->GetTextureCount(aiTextureType_DISPLACEMENT) != 0)
		material->parallaxMap = loadTexture(path, mat, aiTextureType_DISPLACEMENT);

	//Load and assign the colours
	material->ambientColour  = loadColour(mat, AI_MATKEY_COLOR_AMBIENT);
	material->diffuseColour  = loadColour(mat, AI_MATKEY_COLOR_DIFFUSE);
	material->specularColour = loadColour(mat, AI_MATKEY_COLOR_SPECULAR);

	return material;
}

Texture* Mesh::loadTexture(std::string path, const aiMaterial* material, const aiTextureType type) {
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

Colour Mesh::loadColour(const aiMaterial* material, const char* key, unsigned int type, unsigned int idx) {
	//The colour
	aiColor4D colour;
	//Attempt to load the colour
	if (material->Get(key, type, idx, colour) == AI_SUCCESS)
		return Colour(colour.r, colour.g, colour.b, colour.a);
	else
		return Colour::WHITE;
}

Vector3f Mesh::toVector3f(aiVector3D vec) {
	return Vector3f(vec.x, vec.y, vec.z);
}

Quaternion Mesh::toQuaternion(aiQuaternion quat) {
	return Quaternion(quat.x, quat.y, quat.z, quat.w);
}

Matrix4f Mesh::toMatrix4f(aiMatrix4x4 mat) {
	Matrix4f m;

	m.set(0, 0, mat.a1); m.set(0, 1, mat.a2); m.set(0, 2, mat.a3); m.set(0, 3, mat.a4);
	m.set(1, 0, mat.b1); m.set(1, 1, mat.b2); m.set(1, 2, mat.b3); m.set(1, 3, mat.b4);
	m.set(2, 0, mat.c1); m.set(2, 1, mat.c2); m.set(2, 2, mat.c3); m.set(2, 3, mat.c4);
	m.set(3, 0, mat.d1); m.set(3, 1, mat.d2); m.set(3, 2, mat.d3); m.set(3, 3, mat.d4);

	return m;
}

Matrix4f Mesh::toMatrix4f(aiMatrix3x3 mat) {
	Matrix4f m;

	m.set(0, 0, mat.a1); m.set(0, 1, mat.a2); m.set(0, 2, mat.a3); m.set(0, 3, 0);
	m.set(1, 0, mat.b1); m.set(1, 1, mat.b2); m.set(1, 2, mat.b3); m.set(1, 3, 0);
	m.set(2, 0, mat.c1); m.set(2, 1, mat.c2); m.set(2, 2, mat.c3); m.set(2, 3, 0);
	m.set(3, 0, 0);      m.set(3, 1, 0);      m.set(3, 2, 0);      m.set(3, 3, 1);

	return m;
}

/*****************************************************************************
 * The MeshBuilder class
 *****************************************************************************/

/* 2D Stuff */

MeshData* MeshBuilder::createTriangle(Vector2f v1, Vector2f v2, Vector2f v3, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);

	data->addPosition(v1);
	data->addPosition(v2);
	data->addPosition(v3);

	return data;
}

MeshData* MeshBuilder::createQuad(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, v1, v2, v3, v4);
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad(float width, float height, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, Vector2f(0, 0), Vector2f(width, 0), Vector2f(width, height), Vector2f(0, height));
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad(float width, float height, Texture* texture, MeshData::Flag flags) {
	MeshData* data = new MeshData(2, flags);
	addQuadData(data, Vector2f(0, 0), Vector2f(width, 0), Vector2f(width, height), Vector2f(0, height), texture);
	addQuadI(data);
	return data;
}

void MeshBuilder::addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	data->addPosition(v1);
	data->addPosition(v2);
	data->addPosition(v3);
	data->addPosition(v4);
}

void MeshBuilder::addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture) {
	data->addPosition(v1);
	data->addTextureCoord(Vector2f(0, 0));
	data->addPosition(v2);
	data->addTextureCoord(Vector2f(1, 0));
	data->addPosition(v3);
	data->addTextureCoord(Vector2f(1, 1));
	data->addPosition(v4);
	data->addTextureCoord(Vector2f(0, 1));
}

void MeshBuilder::addQuadI(MeshData* data) {
	data->addIndex(0);
	data->addIndex(1);
	data->addIndex(2);
	data->addIndex(3);
	data->addIndex(0);
	data->addIndex(2);
}

void MeshBuilder::addQuadT(MeshData* data, float top, float left, float bottom, float right) {
	data->addTextureCoord(Vector2f(left, top));
	data->addTextureCoord(Vector2f(right, top));
	data->addTextureCoord(Vector2f(right, bottom));
	data->addTextureCoord(Vector2f(left, bottom));
}

/* 3D Stuff */

MeshData* MeshBuilder::createQuad3D(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, v1, v2, v3, v4);
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad3D(float width, float height, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, Vector2f(-width / 2, -height / 2), Vector2f(width / 2, -height / 2), Vector2f(width / 2, height / 2), Vector2f(-width / 2, height / 2));
	addQuadI(data);
	return data;
}

MeshData* MeshBuilder::createQuad3D(float width, float height, Texture* texture, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addQuadData3D(data, Vector2f(-width / 2, -height / 2), Vector2f(width / 2, -height / 2), Vector2f(width / 2, height / 2), Vector2f(-width / 2, height / 2), texture);
	addQuadI(data);
	return data;
}

void MeshBuilder::addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4) {
	data->addPosition(Vector3f(v1));
	data->addPosition(Vector3f(v2));
	data->addPosition(Vector3f(v3));
	data->addPosition(Vector3f(v4));
}

void MeshBuilder::addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture) {
	data->addPosition(Vector3f(v1));
	data->addTextureCoord(Vector2f(0, 0));
	data->addPosition(Vector3f(v2));
	data->addTextureCoord(Vector2f(1, 0));
	data->addPosition(Vector3f(v3));
	data->addTextureCoord(Vector2f(1, 1));
	data->addPosition(Vector3f(v4));
	data->addTextureCoord(Vector2f(0, 1));
}

MeshData* MeshBuilder::createCube(float width, float height, float depth, MeshData::Flag flags) {
	MeshData* data = new MeshData(3, flags);
	addCubeData(data, width, height, depth);
	addCubeI(data);
	return data;
}

void MeshBuilder::addCubeData(MeshData* data, float width, float height, float depth) {
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	//Front face
	data->addPosition(Vector3f(-w, h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(-w, -h, d));

	//Left face
	data->addPosition(Vector3f(-w, -h, d));
	data->addPosition(Vector3f(-w, -h, -d));
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(-w, h, d));

	//Back face
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(w, h, -d));
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(-w, -h, -d));

	//Bottom face
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(-w, -h, d));
	data->addPosition(Vector3f(-w, -h, -d));

	//Right face
	data->addPosition(Vector3f(w, -h, -d));
	data->addPosition(Vector3f(w, -h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, h, -d));

	//Top face
	data->addPosition(Vector3f(-w, h, -d));
	data->addPosition(Vector3f(-w, h, d));
	data->addPosition(Vector3f(w, h, d));
	data->addPosition(Vector3f(w, h, -d));
}

void MeshBuilder::addCubeI(MeshData* data) {
	//Front face
	//B-L triangle
	data->addIndex(0); data->addIndex(1); data->addIndex(2);
	//T-R
	data->addIndex(2); data->addIndex(3); data->addIndex(0);

	//Left face
	//B-L triangle
	data->addIndex(4); data->addIndex(5); data->addIndex(6);
	//T-R
	data->addIndex(6); data->addIndex(7); data->addIndex(4);

	//Back face
	//B-L triangle
	data->addIndex(8); data->addIndex(9); data->addIndex(10);
	//T-R
	data->addIndex(10); data->addIndex(11); data->addIndex(8);

	//Bottom face
	//B-L triangle
	data->addIndex(12); data->addIndex(13); data->addIndex(14);
	//T-R
	data->addIndex(14); data->addIndex(15); data->addIndex(12);

	//Right face
	//B-L triangle
	data->addIndex(16); data->addIndex(17); data->addIndex(18);
	//T-R
	data->addIndex(18); data->addIndex(19); data->addIndex(16);

	//Top face
	//B-L triangle
	data->addIndex(20); data->addIndex(21); data->addIndex(22);
	//T-R
	data->addIndex(22); data->addIndex(23); data->addIndex(20);
}
