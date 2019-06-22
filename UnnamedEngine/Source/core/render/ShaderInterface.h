/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#ifndef CORE_RENDER_SHADERINTERFACE_H_
#define CORE_RENDER_SHADERINTERFACE_H_

#include "UBO.h"
#include "Skinning.h"

/*****************************************************************************
 * Various structures for data in shaders (must follow std140 layout rules)
 *****************************************************************************/

//https://stackoverflow.com/questions/7451476/opengl-uniform-buffer-std140-layout-a-driver-bug-or-did-i-misunderstand-the-spe

struct ShaderBlock_Core {
	Matrix4f ue_mvpMatrix;
	Matrix4f ue_modelMatrix;
	Matrix4f ue_viewMatrix;
	Matrix4f ue_projectionMatrix;
	Matrix4f ue_normalMatrix;

	Vector4f ue_cameraPosition;
};

struct ShaderBlock_Material {
	Vector4f ambientColour;
	Vector4f diffuseColour;
	Vector4f specularColour;

	int hasAmbientTexture;
	int hasDiffuseTexture;
	int diffuseTextureSRGB;
	int hasSpecularTexture;
	int hasShininessTexture;
	int hasNormalMap;
	int hasParallaxMap;

	float parallaxScale;
	float shininess;
};

struct ShaderBlock_Skinning {
	Matrix4f ue_bones[Skeleton::SKINNING_MAX_BONES];
	int ue_useSkinning;

	void updateUseSkinning(UBO* ubo) { ubo->update(&ue_useSkinning, sizeof(ue_bones), sizeof(ue_useSkinning)); };
};

struct ShaderStruct_Light {
	int type;
	float padding1[3];

	Vector4f position;
	Vector4f direction;
	Vector4f diffuseColour;
	Vector4f specularColour;

	float constant;

	float linear;
	float quadratic;

	float innerCutoff;
	float outerCutoff;

	int useShadowMap;
	float padding2[2];
};

struct ShaderBlock_Lighting {
	ShaderStruct_Light ue_lights[6];
	Matrix4f ue_lightSpaceMatrix[6];

	Vector4f ue_lightAmbient;
	int ue_numLights;

	int ue_useEnvironmentMap;
};

struct ShaderBlock_Terrain {
	Vector4f ue_translation;
	Vector2f ue_gridSize;
	float ue_scale;
	float ue_range;
	float ue_heightScale;
	float ue_size;
};

struct ShaderBlock_GammaCorrection {
	int gammaCorrect;
	float exposureIn;
};

struct ShaderBlock_PBREnvMapGen {
	Matrix4f projection;
	Matrix4f view;
};

struct ShaderBlock_PBRPrefilterMapGen {
	float envMapSize;
	float roughness;
};

struct ShaderBlock_PBRLightingCore {
	int ue_useAmbient;
};

struct ShaderBlock_Billboard {
	Matrix4f ue_projectionViewMatrix;
	Vector4f ue_cameraRight;
	Vector4f ue_cameraUp;
	Vector4f ue_billboardCentre;
	Vector2f ue_billboardSize;
};

struct ShaderBlock_ShadowCubemap {
	Matrix4f shadowMatrices[6];
	Vector4f lightPos;
};

/*****************************************************************************
 * The ShaderInterface class handles data transfer to shaders via UBO's
 *****************************************************************************/

class RenderData;

class ShaderInterface {
public:
	/* Structure storing the necessary information to create a UBO */
	struct UBOInfo {
		unsigned int size;
		unsigned int usage;
		unsigned int binding;
	};
private:
	/* Map storing UBO information */
	std::unordered_map<unsigned int, UBOInfo> ubosInfo;
	/* Map used to store UBO's with keys for accessing them */
	std::unordered_map<unsigned int, UBO*> ubos;
public:
	/* The locations for attributes in the shaders */
	static const unsigned int ATTRIBUTE_LOCATION_POSITION;
	static const unsigned int ATTRIBUTE_LOCATION_TEXTURE_COORD;
	static const unsigned int ATTRIBUTE_LOCATION_NORMAL;
	static const unsigned int ATTRIBUTE_LOCATION_TANGENT;
	static const unsigned int ATTRIBUTE_LOCATION_BITANGENT;
	static const unsigned int ATTRIBUTE_LOCATION_BONE_IDS;
	static const unsigned int ATTRIBUTE_LOCATION_BONE_WEIGHTS;

	/* The ids for particular shader blocks */
	static const unsigned int BLOCK_CORE;
	static const unsigned int BLOCK_MATERIAL;
	static const unsigned int BLOCK_SKINNING;
	static const unsigned int BLOCK_LIGHTING;
	static const unsigned int BLOCK_TERRAIN;
	static const unsigned int BLOCK_GAMMA_CORRECTION;
	static const unsigned int BLOCK_PBR_ENV_MAP_GEN;
	static const unsigned int BLOCK_PBR_PREFILTER_MAP_GEN;
	static const unsigned int BLOCK_PBR_LIGHTING_CORE;
	static const unsigned int BLOCK_BILLBOARD;
	static const unsigned int BLOCK_SHADOW_CUBEMAP;

	/* Binding locations for shader blocks */
	static const unsigned int UBO_BINDING_LOCATION_CORE;
	static const unsigned int UBO_BINDING_LOCATION_MATERIAL;
	static const unsigned int UBO_BINDING_LOCATION_SKINNING;
	static const unsigned int UBO_BINDING_LOCATION_LIGHTING;
	static const unsigned int UBO_BINDING_LOCATION_TERRAIN;
	static const unsigned int UBO_BINDING_LOCATION_GAMMA_CORRECTION;
	static const unsigned int UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN;
	static const unsigned int UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN;
	static const unsigned int UBO_BINDING_LOCATION_PBR_LIGHTING_CORE;
	static const unsigned int UBO_BINDING_LOCATION_BILLBOARD;
	static const unsigned int UBO_BINDING_LOCATION_SHADOW_CUBEMAP;

	/* Constructor */
	ShaderInterface();

	/* Destructor */
	virtual ~ShaderInterface();

	/* Method to add a UBO to this interface */
	void add(unsigned int id, unsigned int size, unsigned int usage, unsigned int binding);

	/* Method to add required UBO's and data to a particular RenderData instance ready for rendering with a particular shader */
	void setup(RenderData* renderData, unsigned int shaderID);

	/* Method to obtain a UBO from this interface (Should only call once per object - for Vulkan this will return a new UBO each time) */
	UBO* getUBO(unsigned int id);
};


#endif /* CORE_RENDER_SHADERINTERFACE_H_ */
