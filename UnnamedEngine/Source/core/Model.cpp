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

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>

#include "Model.h"
#include "render/Renderer.h"
#include "../utils/Logging.h"

/*****************************************************************************
 * The Model class
 *****************************************************************************/

std::vector<Mesh*> Model::loadModel(std::string path, std::string fileName) {
	//Load the file using Assimp
	const struct aiScene* scene = aiImportFile((path + fileName).c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes); //aiProcessPreset_TargetRealtime_MaxQuality
	//This map is used to keep track of materials that have already been loaded so they aren't loaded again
	std::map<std::string, Material*> loadedMaterials;
	//Create the std::vector of meshes in the model
	std::vector<Mesh*> meshes;

	//Ensure the data was loaded successfully
	if (scene != NULL) {
		//Go through each loaded mesh
		for (unsigned int a = 0; a < scene->mNumMeshes; a++) {
			//The MeshData instance used to store the data for the current mesh
			MeshData* currentData = new MeshData(3);
			//Pointer to the current mesh being read
			const struct aiMesh* currentMesh = scene->mMeshes[a];

			//Go though all of the vertices
			for (unsigned int i = 0; i < currentMesh->mNumVertices; i++) {
				//Add all of the position data
				aiVector3D& position = currentMesh->mVertices[i];
				currentData->addPosition(Vector3f(position.x, position.y, position.z));
				//Add the texture coordinates data if it exists
				if (currentMesh->mTextureCoords[0] != NULL) {
					aiVector3D& textureCoord = currentMesh->mTextureCoords[0][i];
					currentData->addTextureCoord(Vector2f(textureCoord.x, textureCoord.y));
				}
				//Add the normals data if it exists
				if (currentMesh->mNormals != NULL) {
					aiVector3D& normal = currentMesh->mNormals[i];
					currentData->addNormal(Vector3f(normal.x, normal.y, normal.z));

					//Add the tangent data if it exists
					if (currentMesh->mTangents != NULL) {
						aiVector3D& tangent = currentMesh->mTangents[i];
						currentData->addTangent(Vector3f(tangent.x, tangent.y, tangent.z));
					}

					//Add the bitangent data if it exists
					if (currentMesh->mBitangents != NULL) {
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
			//Create the mesh
			Mesh* mesh = new Mesh(currentData);

			//Check for any materials that also need to be loaded
			if (scene->mNumMaterials > 0) {
				//Pointer to the material for the current mesh
				aiMaterial* currentMaterial = scene->mMaterials[currentMesh->mMaterialIndex];
				//Define the material and material name
				Material* material;
				aiString currentMaterialName;
				//Get the material name
				currentMaterial->Get(AI_MATKEY_NAME, currentMaterialName);

				//Check to see whether the material has already been loaded
				if (loadedMaterials.find(currentMaterialName.C_Str()) != loadedMaterials.end())
					//Assign the material to the one loaded earlier
					material = loadedMaterials.at(currentMaterialName.C_Str());
				else {
					//Create the material instance as a new material needs to be loaded
					material = new Material();
					//Add the material to the loaded materials
					loadedMaterials.insert(std::pair<std::string, Material*>(currentMaterialName.C_Str(), material));

					//Check to see whether the material has a diffuse texture
					if (currentMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
						//Load the texture and assign it in the material
						aiString p;
						currentMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &p);
						material->setDiffuseTexture(Texture::loadTexture(path + StrUtils::str(p.C_Str())));
					}

					//Check to see whether the material has a specular texture
					if (currentMaterial->GetTextureCount(aiTextureType_SPECULAR) != 0) {
						//Load the texture and assign it in the material
						aiString p;
						currentMaterial->GetTexture(aiTextureType_SPECULAR, 0, &p);
						material->setSpecularTexture(Texture::loadTexture(path + StrUtils::str(p.C_Str())));
					}

					//Check to see whether the material has a normal map
					if (currentMaterial->GetTextureCount(aiTextureType_HEIGHT) != 0) {
						//Load the texture and assign it in the material
						aiString p;
						currentMaterial->GetTexture(aiTextureType_HEIGHT, 0, &p);
						material->setNormalMap(Texture::loadTexture(path + StrUtils::str(p.C_Str())));
					}

					//Get the ambient, diffuse and specular colours and set them in the material
					aiColor3D ambientColour = aiColor3D(1.0f, 1.0f, 1.0f);
					currentMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColour);
					material->setAmbientColour(Colour(ambientColour.r, ambientColour.g, ambientColour.b, 1.0f));

					aiColor3D diffuseColour = aiColor3D(1.0f, 1.0f, 1.0f);
					currentMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColour);
					material->setDiffuseColour(Colour(diffuseColour.r, diffuseColour.g, diffuseColour.b, 1.0f));

					aiColor3D specularColour = aiColor3D(1.0f, 1.0f, 1.0f);
					currentMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColour);
					material->setSpecularColour(Colour(specularColour.r, specularColour.g, specularColour.b, 1.0f));
				}

				//Assign the material to the current mesh
				mesh->setMaterial(material);
			}
			//Add the current mesh to the list
			meshes.push_back(mesh);
		}
		//Release all of the resources Assimp loaded
		aiReleaseImport(scene);
		//Return the meshes
		return meshes;
	} else {
		//Log an error as Assimp didn't manage to load the model correctly
		Logger::log("The model '" + path + fileName + "' could not be loaded", "Model", Logger::Error);
		return meshes;
	}
}
