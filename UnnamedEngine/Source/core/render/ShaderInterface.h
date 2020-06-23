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

#pragma once

#include "UBO.h"
#include "Skinning.h"
#include "DescriptorSet.h"
#include "RenderShader.h"

/*****************************************************************************
 * Various structures for data in shaders (must follow std140 layout rules)
 *****************************************************************************/

//https://stackoverflow.com/questions/7451476/opengl-uniform-buffer-std140-layout-a-driver-bug-or-did-i-misunderstand-the-spe

struct ShaderBlock_Camera {
	Matrix4f ue_viewMatrix;
	Matrix4f ue_projectionMatrix;

	Vector4f ue_cameraPosition;
};

struct ShaderBlock_Model {
	Matrix4f ue_mvpMatrix;
	Matrix4f ue_modelMatrix;
	Matrix4f ue_normalMatrix;
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

	void updateUseSkinning(UBO* ubo) { ubo->updateFrame(&ue_useSkinning, sizeof(ue_bones), sizeof(ue_useSkinning)); };
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

struct ShaderBlock_LightBatch {
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

struct ShaderBlock_GammaCorrectionFXAA {
	Vector2f inverseTextureSize;
	int gammaCorrect;
	float exposureIn;
	int fxaa;
};

struct ShaderBlock_GenPBREnvMap {
	Matrix4f projection;
	Matrix4f view;
};

struct ShaderBlock_PBRGenEnvMap {
	Matrix4f projectionViewMatrices[6];
};

struct ShaderBlock_PBRGenPrefilterMap {
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

struct ShaderBlock_SDFText {
	Vector4f outlineColour;
	Vector4f shadowColour;
	float smoothing;
	float outline;
	float shadow;
	float shadowSmoothing;
	Vector2f shadowOffset;
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
	/* Map storing descriptor set layouts */
	std::unordered_map<unsigned int, DescriptorSetLayout*> descriptorSetLayouts;
public:
	/* Set numbers used for specific kinds of descriptor sets*/
	static const unsigned int DESCRIPTOR_SET_NUMBER_PER_CAMERA      = 0;
	static const unsigned int DESCRIPTOR_SET_NUMBER_PER_MATERIAL    = 1;
	static const unsigned int DESCRIPTOR_SET_NUMBER_PER_MODEL       = 2;
	static const unsigned int DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH = 3;
	static const unsigned int DESCRIPTOR_SET_NUMBER_PER_SCENE       = 4;

	/* IDs for descriptor set layouts */
	static const unsigned int DESCRIPTOR_SET_DEFAULT_CAMERA                      = 0;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_MATERIAL                    = 1;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_MODEL                       = 2;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH                 = 3;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING              = 4;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP              = 5;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_GAMMA_CORRECTION_FXAA       = 6;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_DEFERRED_LIGHTING           = 7;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING = 8;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_DEFERRED_PBR_SSR            = 9;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_BILLBOARD                   = 10;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_TERRAIN                     = 11;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT             = 12;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_SDF_TEXT                    = 13;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_BILLBOARD_SDF_TEXT          = 14;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP    = 15;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP      = 16;
	static const unsigned int DESCRIPTOR_SET_DEFAULT_PBR_GEN_PREFILTER_MAP       = 17;

	/* The locations for attributes in the shaders */
	static const unsigned int ATTRIBUTE_LOCATION_POSITION      = 0;
	static const unsigned int ATTRIBUTE_LOCATION_TEXTURE_COORD = 1;
	static const unsigned int ATTRIBUTE_LOCATION_NORMAL        = 2;
	static const unsigned int ATTRIBUTE_LOCATION_TANGENT       = 3;
	static const unsigned int ATTRIBUTE_LOCATION_BITANGENT     = 4;
	static const unsigned int ATTRIBUTE_LOCATION_BONE_IDS      = 5;
	static const unsigned int ATTRIBUTE_LOCATION_BONE_WEIGHTS  = 6;

	/* The ids for particular shader blocks */
	static const unsigned int BLOCK_PBR_ENV_MAP_GEN        = 8;
	static const unsigned int BLOCK_PBR_PREFILTER_MAP_GEN  = 9;
	static const unsigned int BLOCK_PBR_LIGHTING_CORE      = 10;

	/* Binding locations for shader blocks */
	static const unsigned int UBO_BINDING_LOCATION_CAMERA                 = 1;
	static const unsigned int UBO_BINDING_LOCATION_MODEL                  = 2;
	static const unsigned int UBO_BINDING_LOCATION_MATERIAL               = 3;
	static const unsigned int UBO_BINDING_LOCATION_SKINNING               = 4;
	static const unsigned int UBO_BINDING_LOCATION_LIGHT_BATCH            = 5;
	static const unsigned int UBO_BINDING_LOCATION_TERRAIN                = 6;
	static const unsigned int UBO_BINDING_LOCATION_GAMMA_CORRECTION_FXAA  = 7;
	static const unsigned int UBO_BINDING_LOCATION_GEN_PBR_ENV_MAP        = 8;
	static const unsigned int UBO_BINDING_LOCATION_GEN_PBR_PREFILTER_MAP  = 9;
	static const unsigned int UBO_BINDING_LOCATION_PBR_LIGHTING_CORE      = 10;
	static const unsigned int UBO_BINDING_LOCATION_BILLBOARD              = 11;
	static const unsigned int UBO_BINDING_LOCATION_SHADOW_CUBEMAP         = 12;
	static const unsigned int UBO_BINDING_LOCATION_SDF_TEXT               = 4;

	/* Constructor */
	ShaderInterface();

	/* Destructor */
	virtual ~ShaderInterface();

	/* Method to add a descriptor set layout to this interface */
	void add(unsigned int id, DescriptorSetLayout* layout);

	/* Sets up a render shader for use (e.g. adds required DescriptorSetLayout instances) */
	void setup(unsigned int shaderID, RenderShader* renderShader);

	/* Method used to obtain a descriptor set layout from this interface */
	DescriptorSetLayout* getDescriptorSetLayout(unsigned int id);
};


