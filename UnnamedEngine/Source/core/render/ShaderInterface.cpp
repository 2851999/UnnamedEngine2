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

#include "ShaderInterface.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The ShaderInterface class
 *****************************************************************************/

/* The locations for attributes in the shaders */
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_POSITION      = 0;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_TEXTURE_COORD = 1;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_NORMAL        = 2;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_TANGENT       = 3;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BITANGENT     = 4;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BONE_IDS      = 5;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BONE_WEIGHTS  = 6;

/* The ids for particular shader blocks */
const std::string ShaderInterface::BLOCK_CORE                   = "Core";
const std::string ShaderInterface::BLOCK_MATERIAL               = "Material";
const std::string ShaderInterface::BLOCK_SKINNING               = "Skinning";
const std::string ShaderInterface::BLOCK_LIGHTING               = "Lighting";
const std::string ShaderInterface::BLOCK_TERRAIN                = "Terrain";
const std::string ShaderInterface::BLOCK_GAMMA_CORRECTION       = "GammaCorrection";
const std::string ShaderInterface::BLOCK_PBR_ENV_MAP_GEN        = "PBREnvMapGen";
const std::string ShaderInterface::BLOCK_PBR_PREFILTER_MAP_GEN  = "PBRPrefilterMapGen";
const std::string ShaderInterface::BLOCK_PBR_LIGHTING_CORE      = "PBRLightingCore";
const std::string ShaderInterface::BLOCK_BILLBOARD              = "Billboard";
const std::string ShaderInterface::BLOCK_SHADOW_CUBEMAP         = "ShadowCubemap";

/* Binding locations for shader blocks */
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_CORE                  = 1;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_MATERIAL              = 2;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SKINNING              = 3;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_LIGHTING              = 4;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_TERRAIN               = 5;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_GAMMA_CORRECTION      = 6;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN       = 7;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN = 8;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_LIGHTING_CORE     = 9;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_BILLBOARD             = 10;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SHADOW_CUBEMAP        = 11;

ShaderInterface::ShaderInterface() {
	//Add all required UBOs for the default shaders
	add(BLOCK_CORE,                  new UBO(NULL, sizeof(ShaderBlock_Core),               GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_CORE));
	add(BLOCK_MATERIAL,              new UBO(NULL, sizeof(ShaderBlock_Material),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_MATERIAL));
	add(BLOCK_SKINNING,              new UBO(NULL, sizeof(ShaderBlock_Skinning),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_SKINNING));
	add(BLOCK_LIGHTING,              new UBO(NULL, sizeof(ShaderBlock_Lighting),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_LIGHTING));
	add(BLOCK_TERRAIN,               new UBO(NULL, sizeof(ShaderBlock_Terrain),            GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_TERRAIN));
	add(BLOCK_GAMMA_CORRECTION,      new UBO(NULL, sizeof(ShaderBlock_GammaCorrection),    GL_STATIC_DRAW,  UBO_BINDING_LOCATION_GAMMA_CORRECTION));
	add(BLOCK_PBR_ENV_MAP_GEN,       new UBO(NULL, sizeof(ShaderBlock_PBREnvMapGen),       GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN));
	add(BLOCK_PBR_PREFILTER_MAP_GEN, new UBO(NULL, sizeof(ShaderBlock_PBRPrefilterMapGen), GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN));
	add(BLOCK_PBR_LIGHTING_CORE,     new UBO(NULL, sizeof(ShaderBlock_PBRLightingCore),    GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_LIGHTING_CORE));
	add(BLOCK_BILLBOARD,             new UBO(NULL, sizeof(ShaderBlock_Billboard),          GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_BILLBOARD));
	add(BLOCK_SHADOW_CUBEMAP,        new UBO(NULL, sizeof(ShaderBlock_ShadowCubemap),      GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_SHADOW_CUBEMAP));
}

ShaderInterface::~ShaderInterface() {
	//Go through and delete all UBO's
	for (auto it : ubos)
		delete it.second;
	ubos.clear();
}

UBO* ShaderInterface::getUBO(std::string id) {
	if (ubos.count(id) > 0)
		return ubos.at(id);
	else {
		Logger::log("The UBO with the id '" + id + "' could not be found", "ShaderInterface", LogType::Error);
		return NULL;
	}
}
