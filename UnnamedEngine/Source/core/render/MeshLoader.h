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

#ifndef CORE_RENDER_MESHLOADER_H_
#define CORE_RENDER_MESHLOADER_H_

#include <string>
#include <map>
#include <fstream>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

#include "Mesh.h"

struct aiScene;

/*****************************************************************************
 * The MeshLoader class contains methods to load a mesh from a file
 *****************************************************************************/

class MeshLoader {
private:
	static void addChildren(const aiNode* node, std::map<const aiNode*, const aiBone*>& nodes);
	static const aiNode* findMeshNode(const aiNode* parent);
	static const aiMatrix4x4 calculateMatrix(const aiNode* current, aiMatrix4x4 currentMatrix);

	/* Static method called to load a material */
	static Material* loadAssimpMaterial(std::string path, std::string fileName, const aiMaterial* material);

	/* Static method called to load a texture */
	static Texture* loadAssimpTexture(std::string path, const aiMaterial* material, const aiTextureType type);

	/* Static method called to load a colour (If not assigned, returns WHITE instead) */
	static Colour loadAssimpColour(const aiMaterial* material, const char* key, unsigned int type, unsigned int idx);

	static Vector3f toVector3f(aiVector3D vec);
	static Quaternion toQuaternion(aiQuaternion quat);
	static Matrix4f toMatrix4f(aiMatrix4x4 mat);
	static Matrix4f toMatrix4f(aiMatrix3x3 mat);

	/* Various read/write methods */
	static void writeVectorDataFloat(std::ofstream& output, std::vector<float>& data);
	static void writeVectorDataUInt(std::ofstream& output, std::vector<unsigned int>& data);
	static void writeMaterial(std::ofstream& output, Material* material, std::string path);
	static void writeTexture(std::ofstream& output, Texture* texture, std::string path);

	static void writeUInt(std::ofstream& output, unsigned int value);
	static void writeFloat(std::ofstream& output, float value);
	static void writeVector3f(std::ofstream& output, Vector3f vector);
	static void writeVector4f(std::ofstream& output, Vector4f vector);
	static void writeMatrix4f(std::ofstream& output, Matrix4f matrix);
	static void writeString(std::ofstream& output, std::string string);

	static void readVectorDataFloat(std::ifstream& input, std::vector<float>& data);
	static void readVectorDataUInt(std::ifstream& input, std::vector<unsigned int>& data);
	static void readMaterial(std::ifstream& input, std::vector<Material*>& materials, std::string path);
	static Texture* readTexture(std::ifstream& input, std::string path);

	static void readUInt(std::ifstream& output, unsigned int& value);
	static void readFloat(std::ifstream& output, float& value);
	static void readVector3f(std::ifstream& input, Vector3f& vector);
	static void readVector4f(std::ifstream& input, Vector4f& vector);
	static void readMatrix4f(std::ifstream& input, Matrix4f& matrix);
	static void readString(std::ifstream& input, std::string& string);
public:
	/* Static method called to read a file and load a model's meshes */
	static Mesh* loadModel(std::string path, std::string fileName);

	/* Static method called to read a file and load a model's meshes using Assimp */
	static Mesh* loadAssimpModel(std::string path, std::string fileName);

	/* Static method called to write a model to a file in a custom engine format */
	static void saveEngineModel(std::string path, std::string fileName, Mesh* mesh);

	/* Static method called to read a model from a file in a custom engine format */
	static Mesh* loadEngineModel(std::string path, std::string fileName);

	/* Static method called to convert a model to the engine format and save it in the same location */
	static void convertToEngineModel(std::string path, std::string fileName);
};

#endif /* CORE_RENDER_MESHLOADER_H_ */
