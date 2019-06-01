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
};

/*****************************************************************************
 * The ShaderInterface class handles data transfer to shaders via UBO's
 *****************************************************************************/

class ShaderInterface {
private:
	/* Map used to store UBO's with keys for accessing them */
	std::unordered_map<std::string, UBO*> ubos;
public:
	/* The ids for particular shader blocks */
	static const std::string BLOCK_MATERIAL;
	static const std::string BLOCK_SKINNING;
	static const std::string BLOCK_LIGHTING;

	/* Binding locations for shader blocks */
	static const unsigned int UBO_BINDING_LOCATION_MATERIAL;
	static const unsigned int UBO_BINDING_LOCATION_SKINNING;
	static const unsigned int UBO_BINDING_LOCATION_LIGHTING;

	/* Constructor */
	ShaderInterface();

	/* Destructor */
	virtual ~ShaderInterface();

	/* Method to add a UBO to this interface */
	inline void add(std::string id, UBO* ubo) { ubos.insert(std::pair<std::string, UBO*>(id, ubo)); }

	/* Method to obtain a UBO from this interface */
	UBO* getUBO(std::string id);
};


#endif /* CORE_RENDER_SHADERINTERFACE_H_ */
