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

#ifndef CORE_RENDER_MESH_H_
#define CORE_RENDER_MESH_H_

#include "Colour.h"
#include "Material.h"
#include "RenderData.h"
#include "Shader.h"
#include "Skinning.h"
#include "VBO.h"
#include "../Sphere.h"

/*****************************************************************************
 * The MeshData class stores information about a mesh
 *****************************************************************************/

#define NUM_BONES_PER_VERTEX 4

class MeshData {
public:
	/* Data for a sub mesh that may be rendered */
	struct SubData {
		unsigned int baseIndex;  	//The base index to start rendering from
		unsigned int baseVertex; 	//The base vertex to start rendering from
		unsigned int count;      	//Number of indices/vertices to render
		unsigned int materialIndex; //The material index of this part of the mesh
	};

	struct VertexBoneData {
		unsigned int ids[NUM_BONES_PER_VERTEX];
		float weights[NUM_BONES_PER_VERTEX];

		void addBoneData(unsigned int boneID, float weight) {
			for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++) {
				if (weights[i] == 0.0f) {
					ids[i] = boneID;
					weights[i] = weight;
					return;
				}
			}
		}
	};

	/* Data for a bounding sphere (for frustum culling) */
	struct BoundingSphere {
		Vector3f centre;
		float radius;
	};
private:
	/* The raw data stored for this mesh */
	std::vector<float> positions;
	std::vector<float> colours;
	std::vector<float> textureCoords;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<float> bitangents;
	std::vector<float> others;
	std::vector<unsigned int> indices;

	std::vector<unsigned int>   boneIDs;
	std::vector<float> 		    boneWeights;

	/* Various data about how the data is stored */
	unsigned int numDimensions = 0;

	/* Keeps track of the number of each value stored */
	unsigned int numPositions  = 0;
	unsigned int numColours    = 0;
	unsigned int numTextureCoords = 0;
	unsigned int numNormals    = 0;
	unsigned int numTangents   = 0;
	unsigned int numBitangents = 0;
	unsigned int numIndices    = 0;

	unsigned int numBones      = 0;

	/* The sub data instances */
	std::vector<SubData> subData;

	/* We need to calculate the max/min extents of each x, y and z coordinate of the vertices in the mesh for bounding spheres, so store that info here */
	float minX =  1000000000;
	float maxX = -1000000000;
	float minY =  1000000000;
	float maxY = -1000000000;
	float minZ =  1000000000;
	float maxZ = -1000000000;
public:
	static const unsigned int DIMENSIONS_2D = 2;
	static const unsigned int DIMENSIONS_3D = 3;

	/* Flags */
	enum Flag {
		NONE = 0,
		SEPARATE_POSITIONS  = 1,
		SEPARATE_COLOURS    = 2,
		SEPARATE_TEXTURE_COORDS = 4,
		SEPARATE_NORMALS    = 8,
		SEPARATE_TANGENTS   = 16,
		SEPARATE_BITANGENTS = 32
	};

	MeshData(unsigned int numDimensions, Flag flags = Flag::NONE) {
		this->numDimensions = numDimensions;
		this->flags = flags;
	}

	/* Method to calculate and return a bounding sphere for this mesh */
	Sphere calculateBoundingSphere();

	/* Method called to setup the bones structures */
	void setupBones(unsigned int numVertices);

	/* Methods to add data */
	void addPosition(Vector2f position);
	void addPosition(Vector3f position);
	void addColour(Colour colour);
	void addTextureCoord(Vector2f textureCoord);
	void addNormal(Vector3f normal);
	void addTangent(Vector3f tangnet);
	void addBitangent(Vector3f bitangent);

	inline void addIndex(unsigned int index) { indices.push_back(index); numIndices++; }

	void addBoneData(unsigned int boneID, float boneWeight);

	inline void setNumPositions(unsigned int numPositions) { this->numPositions = numPositions; }
	inline void setNumColours(unsigned int numColours) { this->numColours = numColours; }
	inline void setNumTextureCoords(unsigned int numTextureCoords) { this->numTextureCoords = numTextureCoords; }
	inline void setNumNormals(unsigned int numNormals) { this->numNormals = numNormals; }
	inline void setNumTangents(unsigned int numTangents) { this->numTangents = numTangents; }
	inline void setNumBitangents(unsigned int numBitangents) { this->numBitangents = numBitangents; }
	inline void setNumIndices(unsigned int numIndices) { this->numIndices = numIndices; }
	inline void setNumBones(unsigned int numBones) { this->numBones = numBones; }


	/* Methods used to add a sub data structure */
	inline void addSubData(SubData& data) { subData.push_back(data); }
	inline void addSubData(unsigned int baseIndex, unsigned int baseVertex, unsigned int count, unsigned int materialIndex = 0) {
		SubData data;
		data.baseIndex  = baseIndex;
		data.baseVertex = baseVertex;
		data.count      = count;
		data.materialIndex = materialIndex;
		addSubData(data);
	}

	/* Methods to remove data */
	inline void clearPositions()     { positions.clear();     numPositions     = 0; }
	inline void clearColours()       { colours.clear();       numColours       = 0; }
	inline void clearTextureCoords() { textureCoords.clear(); numTextureCoords = 0; }
	inline void clearNormals()       { normals.clear();       numNormals       = 0; }
	inline void clearTangents()      { tangents.clear();      numTangents      = 0; }
	inline void clearBitangents()    { bitangents.clear();    numBitangents    = 0; }
	inline void clearIndices()       { indices.clear();       numIndices       = 0; }

	/* Methods to check whether certain data should be separated */
	inline bool separatePositions()     { return flags & Flag::SEPARATE_POSITIONS;      }
	inline bool separateColours()       { return flags & Flag::SEPARATE_COLOURS;        }
	inline bool separateTextureCoords() { return flags & Flag::SEPARATE_TEXTURE_COORDS; }
	inline bool separateNormals()       { return flags & Flag::SEPARATE_NORMALS;        }
	inline bool separateTangents()      { return flags & Flag::SEPARATE_TANGENTS;       }
	inline bool separateBitangents()    { return flags & Flag::SEPARATE_BITANGENTS;     }

	/* Methods to check whether data has been given */
	inline bool hasPositions()     { return numPositions > 0;     }
	inline bool hasColours()       { return numColours > 0;       }
	inline bool hasTextureCoords() { return numTextureCoords > 0; }
	inline bool hasNormals()       { return numNormals > 0;       }
	inline bool hasTangents()      { return numTangents > 0;      }
	inline bool hasBitangents()    { return numBitangents > 0;    }
	inline bool hasOthers()        { return others.size() > 0;    }
	inline bool hasIndices()       { return numIndices > 0;       }
	inline bool hasBones()         { return numBones > 0;         }

	/* Methods to get the data */
	unsigned int getNumPositions()     { return numPositions;     }
	unsigned int getNumColours()       { return numColours;       }
	unsigned int getNumTextureCoords() { return numTextureCoords; }
	unsigned int getNumNormals()       { return numNormals;       }
	unsigned int getNumTangents()      { return numTangents;      }
	unsigned int getNumBitangents()    { return numBitangents;    }
	unsigned int getNumIndices()       { return numIndices;       }
	unsigned int getNumBones()         { return numBones;         }
	unsigned int getNumDimensions()    { return numDimensions;    }

	std::vector<float>& getPositions()      { return positions;     }
	std::vector<float>& getColours()        { return colours;       }
	std::vector<float>& getTextureCoords()  { return textureCoords; }
	std::vector<float>& getNormals()        { return normals;       }
	std::vector<float>& getTangents()       { return tangents;      }
	std::vector<float>& getBitangents()     { return bitangents;    }
	std::vector<float>& getOthers()         { return others;        }
	std::vector<unsigned int>& getIndices() { return indices;       }
	std::vector<unsigned int>& getBoneIDs() { return boneIDs;       }
	std::vector<float>& getBoneWeights()    { return boneWeights;   }
	inline bool hasSubData() { return subData.size() > 0; }
	inline unsigned int getSubDataCount() { return subData.size(); }
	inline SubData& getSubData(unsigned int index) { return subData[index]; }
private:
	/* The flags being used */
	Flag flags = Flag::NONE;
};

inline constexpr MeshData::Flag operator|(MeshData::Flag a, MeshData::Flag b) {
	return static_cast<MeshData::Flag>(static_cast<int>(a) | static_cast<int>(b));
}

/*****************************************************************************
 * The MeshRenderData class stores rendering information about a Mesh
 *****************************************************************************/

class MeshRenderData {
private:
	/* The render data for this mesh */
	RenderData* renderData = NULL;

	/* The various Vertex Buffer Objects for this mesh */
	VBO<GLfloat>* vboPositions     = NULL;
	VBO<GLfloat>* vboColours       = NULL;
	VBO<GLfloat>* vboTextureCoords = NULL;
	VBO<GLfloat>* vboNormals       = NULL;
	VBO<GLfloat>* vboTangents      = NULL;
	VBO<GLfloat>* vboBitangents    = NULL;
	VBO<unsigned int>* vboIndices  = NULL;
	VBO<GLfloat>* vboOthers        = NULL;

	VBO<unsigned int>* vboBoneIDs = NULL;
	VBO<GLfloat>*  vboBoneWeights = NULL;

	/* Usage of each VBO, the default is GL_STATIC_DRAW */
	GLenum usagePositions     = GL_STATIC_DRAW;
	GLenum usageColours       = GL_STATIC_DRAW;
	GLenum usageTextureCoords = GL_STATIC_DRAW;
	GLenum usageNormals       = GL_STATIC_DRAW;
	GLenum usageTangents      = GL_STATIC_DRAW;
	GLenum usageBitangents    = GL_STATIC_DRAW;
	GLenum usageOthers        = GL_STATIC_DRAW;
	GLenum usageIndices       = GL_STATIC_DRAW;

	/* The number of vertices that this class stores data about */
	int numVertices = 0;

	/* Used to identify whether Mesh is indexed */
	bool hasIndices = false;
public:
	MeshRenderData() {}
	MeshRenderData(MeshData* data, RenderShader* renderShader) { setup(data, renderShader); }

	virtual ~MeshRenderData() { destroy(); }

	/* Setups this structure for rendering using OpenGL */
	void setup(MeshData* data, RenderShader* renderShader);

	/* Method to render using the data */
	void render();

	/* Various methods to completely update a set of values (assuming it is in its own separate VBO) */
	void updatePositions(MeshData* data);
	void updateColours();
	void updateTextureCoords();
	void updateNormals();
	void updateTangents();
	void updateBitangents();
	void updateIndices(MeshData* data);

	/* Method to destroy the OpenGL data */
	void destroy();

	/* Setters and getters */
	inline RenderData* getRenderData() { return renderData; }
};

/*****************************************************************************
 * The Mesh class manages both the MeshData and MeshRenderData to create a
 * Mesh that can be rendered
 *****************************************************************************/

class Mesh {
private:
	/* The MeshData and MeshRenderData for this Mesh */
	MeshData* data;
	MeshRenderData* renderData = NULL;

	/* The transform matrix for this mesh */
	Matrix4f transform;

	/* The materials */
	std::vector<Material*> materials;

	/* The skeleton for this mesh */
	Skeleton* skeleton = NULL;

	/* The bounding sphere for this mesh */
	Sphere boundingSphere;

	/* Boolean that states whether this mesh should be culled where possible */
	bool culling = true;
public:
	/* The constructor */
	Mesh(MeshData* data);

	/* The destructor */
	virtual ~Mesh();

	/* Method called to setup this mesh for rendering */
	inline void setup(RenderShader* renderShader) {
		this->renderData = new MeshRenderData(this->data, renderShader);
	}

	/* Method to add a material */
	inline void addMaterial(Material* material) { materials.push_back(material); }

	/* The setters and getters */
	inline void setMatrix(const Matrix4f& transform) { this->transform = transform; }
	inline void setMaterial(Material* material) { this->materials[0] = material; }
	inline void setMaterial(unsigned int index, Material* material) {
		if (index == materials.size())
			addMaterial(material);
		else
			materials[index] = material;
	}
	inline void setMaterials(std::vector<Material*>& materials) { this->materials = materials; }
	inline void setSkeleton(Skeleton* skeleton) { this->skeleton = skeleton; }
	inline void setBoundingSphere(Sphere sphere) { this->boundingSphere = sphere; }
	inline void setCullingEnabled(bool enabled) { this->culling = enabled; }

	inline Matrix4f getMatrix() { return transform; }
	inline MeshData* getData() { return data; }
	inline bool hasData() { return data; }
	inline MeshRenderData* getRenderData() { return renderData; }
	inline bool hasRenderData() { return renderData; }
	inline unsigned int getNumMaterials() { return materials.size(); }
	inline Material* getMaterial(unsigned int index = 0) { return materials[index]; }
	inline std::vector<Material*>& getMaterials() { return materials; }
	inline bool hasMaterial() { return materials.size() > 0; }
	inline Skeleton* getSkeleton() { return skeleton; }
	inline bool hasSkeleton() { return skeleton; }
	inline Vector3f getBoundingSphereCentre() { return boundingSphere.centre; }
	inline float getBoundingSphereRadius() { return boundingSphere.radius; }
	inline bool cullingEnabled() { return data->getNumDimensions() == MeshData::DIMENSIONS_3D && culling; }
	inline bool isCullingEnabled() { return culling; }
};

/*****************************************************************************
 * The MeshBuilder class contains methods to create Meshes
 *****************************************************************************/

class MeshBuilder {
public:

	/* 2D Stuff */

	/* Method used to create a MeshData instance for a triangle given its 3 corners */
	static MeshData* createTriangle(Vector2f v1, Vector2f v2, Vector2f v3, MeshData::Flag flags = MeshData::NONE);

	/* Method used to create a MeshData instance for a quad, given its 4 corners */
	static MeshData* createQuad(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags = MeshData::NONE);
	/* Method used to create a MeshData instance for a quad (rectangle/square in this case) given its width and height */
	static MeshData* createQuad(float width, float height, MeshData::Flag flags = MeshData::NONE);
	/* Method used to create a MeshData instance for a textured quad (rectangle/square in this case) given its width and height */
	static MeshData* createQuad(float width, float height, Texture* texture, MeshData::Flag flags = MeshData::NONE);

	/* Method used to add the required data for a quad to a MeshData instance given its 4 corners */
	static void addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	/* Method used to add the required data for a textured quad to a MeshData instance given its 4 corners */
	static void addQuadData(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture);
	/* Method used to add the indices for a quad to a MeshData instance */
	static void addQuadI(MeshData* data);
	/* Method used to add the texture coordinates for a quad to a MeshData instance */
	static void addQuadT(MeshData* data, float top, float left, float bottom, float right);

	/* 3D Stuff */

	/* Method used to create a MeshData instance for a quad, given its 4 corners */
	static MeshData* createQuad3D(Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, MeshData::Flag flags = MeshData::NONE);
	/* Method used to create a MeshData instance for a quad (rectangle/square in this case) given its width and height */
	static MeshData* createQuad3D(float width, float height, MeshData::Flag flags = MeshData::NONE);
	/* Method used to create a MeshData instance for a textured quad (rectangle/square in this case) given its width and height */
	static MeshData* createQuad3D(float width, float height, Texture* texture, MeshData::Flag flags = MeshData::NONE);

	/* Method used to add the required data for a quad to a MeshData instance given its 4 corners */
	static void addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4);
	/* Method used to add the required data for a textured quad to a MeshData instance given its 4 corners */
	static void addQuadData3D(MeshData* data, Vector2f v1, Vector2f v2, Vector2f v3, Vector2f v4, Texture* texture);

	/* Method used to create a MeshData instance for a cube given its width, height and depth */
	static MeshData* createCube(float width, float height, float depth, MeshData::Flag flags = MeshData::NONE);

	/* Method used to add the required data for a quad to a MeshData instance given its width, height and depth */
	static void addCubeData(MeshData* data, float width, float height, float depth);
	/* Method used to add the indices for a cube to a MeshData instance */
	static void addCubeI(MeshData* data);
};

#endif /* CORE_RENDER_MESH_H_ */
