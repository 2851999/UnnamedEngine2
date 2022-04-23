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

#pragma once

#include <unordered_map>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

#include "Mesh.h"

#include "../../utils/BinaryFile.h"

struct aiScene;

/*****************************************************************************
 * The MeshLoader class contains methods to load a mesh from a file
 *****************************************************************************/

class MeshLoader {
private:
	static void addChildren(const aiNode* node, std::unordered_map<const aiNode*, const aiBone*>& nodes);
	static const aiNode* findMeshNode(const aiNode* parent);
	static const aiMatrix4x4 calculateMatrix(const aiNode* current, aiMatrix4x4 currentMatrix);

	/* Static method called to load a material */
	static Material* loadAssimpMaterial(std::string path, std::string fileName, const aiMaterial* material, bool pbr);

	/* Static method called to load a texture */
	static Texture* loadAssimpTexture(std::string path, const aiMaterial* material, const aiTextureType type, bool srgb = false);

	/* Static method called to load a colour (If not assigned, returns WHITE instead) */
	static Colour loadAssimpColour(const aiMaterial* material, const char* key, unsigned int type, unsigned int idx);

	static Vector3f toVector3f(aiVector3D vec);
	static Quaternion toQuaternion(aiQuaternion quat);
	static Matrix4f toMatrix4f(aiMatrix4x4 mat);
	static Matrix4f toMatrix4f(aiMatrix3x3 mat);

	/* Various read/write methods */
	static void writeMaterial(BinaryFile& file, Material* material, std::string path);
	static void writeTexture(BinaryFile& file, Texture* texture, std::string path);

	static void readMaterial(BinaryFile& file, std::vector<Material*>& materials, std::string path);
	static Texture* readTexture(BinaryFile& file, std::string path, bool srgb = false);
public:
	static bool loadDiffuseTexturesAsSRGB;

	/* Static method called to read a file and load a model's meshes
	 * NOTE: PBR only needs to be assigned if loading a model using assimp (i.e. not
	 * using the engine's model format) */
	static Mesh* loadModel(std::string path, std::string fileName, bool pbr = false);

	/* Static method called to read a file and load a model's meshes using Assimp */
	static Mesh* loadAssimpModel(std::string path, std::string fileName, bool pbr, bool genNormals = true);

	/* Static method called to read a file and load a model's meshes using Assimp keeping Mesh's separate */
	static std::vector<Mesh*> loadAssimpModelSeparate(std::string path, std::string fileName, bool pbr, bool genNormals = true);

	/* Static method called to write a model to a file in a custom engine format */
	static void saveEngineModel(std::string path, std::string fileName, Mesh* mesh);

	/* Static method called to read a model from a file in a custom engine format */
	static Mesh* loadEngineModel(std::string path, std::string fileName);

	/* Static method called to convert a model to the engine format and save it in the same location */
	static void convertToEngineModel(std::string path, std::string fileName, bool pbr, bool genNormals = true);
};

