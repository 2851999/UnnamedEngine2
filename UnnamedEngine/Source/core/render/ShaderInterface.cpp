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

#include "RenderData.h"
#include "Renderer.h"

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
const unsigned int ShaderInterface::BLOCK_CORE                   = 1;
const unsigned int ShaderInterface::BLOCK_MATERIAL               = 2;
const unsigned int ShaderInterface::BLOCK_SKINNING               = 3;
const unsigned int ShaderInterface::BLOCK_LIGHTING               = 4;
const unsigned int ShaderInterface::BLOCK_TERRAIN                = 5;
const unsigned int ShaderInterface::BLOCK_GAMMA_CORRECTION       = 6;
const unsigned int ShaderInterface::BLOCK_PBR_ENV_MAP_GEN        = 7;
const unsigned int ShaderInterface::BLOCK_PBR_PREFILTER_MAP_GEN  = 8;
const unsigned int ShaderInterface::BLOCK_PBR_LIGHTING_CORE      = 9;
const unsigned int ShaderInterface::BLOCK_BILLBOARD              = 10;
const unsigned int ShaderInterface::BLOCK_SHADOW_CUBEMAP         = 11;

/* Binding locations for shader blocks */
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_CORE                   = 1;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_MATERIAL               = 2;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SKINNING               = 3;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_LIGHTING               = 4;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_TERRAIN                = 5;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_GAMMA_CORRECTION       = 6;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN        = 7;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN  = 8;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_LIGHTING_CORE      = 9;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_BILLBOARD              = 10;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SHADOW_CUBEMAP         = 11;

ShaderInterface::ShaderInterface() {
	//Add all required UBOs for the default shaders
	add(BLOCK_CORE,                  sizeof(ShaderBlock_Core),               GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_CORE);
	add(BLOCK_MATERIAL,              sizeof(ShaderBlock_Material),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_MATERIAL);
	add(BLOCK_SKINNING,              sizeof(ShaderBlock_Skinning),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_SKINNING);
	add(BLOCK_LIGHTING,              sizeof(ShaderBlock_Lighting),           GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_LIGHTING);
	add(BLOCK_TERRAIN,               sizeof(ShaderBlock_Terrain),            GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_TERRAIN);
	add(BLOCK_GAMMA_CORRECTION,      sizeof(ShaderBlock_GammaCorrection),    GL_STATIC_DRAW,  UBO_BINDING_LOCATION_GAMMA_CORRECTION);
	add(BLOCK_PBR_ENV_MAP_GEN,       sizeof(ShaderBlock_PBREnvMapGen),       GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN);
	add(BLOCK_PBR_PREFILTER_MAP_GEN, sizeof(ShaderBlock_PBRPrefilterMapGen), GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN);
	add(BLOCK_PBR_LIGHTING_CORE,     sizeof(ShaderBlock_PBRLightingCore),    GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_PBR_LIGHTING_CORE);
	add(BLOCK_BILLBOARD,             sizeof(ShaderBlock_Billboard),          GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_BILLBOARD);
	add(BLOCK_SHADOW_CUBEMAP,        sizeof(ShaderBlock_ShadowCubemap),      GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_SHADOW_CUBEMAP);
}

ShaderInterface::~ShaderInterface() {
	//Go through and delete all UBO's
	for (auto it : ubos)
		delete it.second;
	for (UBO* ubo : ubosVk)
		delete ubo;
	ubos.clear();
	ubosVk.clear();
}

void ShaderInterface::add(unsigned int id, unsigned int size, unsigned int usage, unsigned int binding) {
	//Add the data to the map
	ubosInfo.insert(std::pair<unsigned int, UBOInfo>(id, { size, usage, binding }));
}

void ShaderInterface::setup(RenderData* renderData, unsigned int shaderID) {
	//Check the shader ID and add the required UBOs/Textures
	if (shaderID == Renderer::SHADER_MATERIAL) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
	} else if (shaderID == Renderer::SHADER_SKY_BOX) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
	} else if (shaderID == Renderer::SHADER_FONT) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
	} else if (shaderID == Renderer::SHADER_BILLBOARD) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_BILLBOARD,         getUBO(BLOCK_BILLBOARD));
	} else if (shaderID == Renderer::SHADER_PARTICLE) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_BILLBOARD,         getUBO(BLOCK_BILLBOARD));
	} else if (shaderID == Renderer::SHADER_LIGHTING) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
	} else if (shaderID == Renderer::SHADER_VULKAN_LIGHTING) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
	} else if (shaderID == Renderer::SHADER_FRAMEBUFFER) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
	} else if (shaderID == Renderer::SHADER_ENVIRONMENT_MAP) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
	} else if (shaderID == Renderer::SHADER_SHADOW_MAP) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
	} else if (shaderID == Renderer::SHADER_SHADOW_CUBEMAP) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_SHADOW_CUBEMAP,    getUBO(BLOCK_SHADOW_CUBEMAP));
	} else if (shaderID == Renderer::SHADER_BILLBOARDED_FONT) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_BILLBOARD,         getUBO(BLOCK_BILLBOARD));
	} else if (shaderID == Renderer::SHADER_TERRAIN) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		//renderData->add(BLOCK_SKINNING,        getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
		renderData->add(BLOCK_TERRAIN,           getUBO(BLOCK_TERRAIN));
	} else if (shaderID == Renderer::SHADER_PLAIN_TEXTURE) {

	} else if (shaderID == Renderer::SHADER_DEFERRED_LIGHTING) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
	} else if (shaderID == Renderer::SHADER_TILEMAP) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
	} else if (shaderID == Renderer::SHADER_VULKAN) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
	} else if (shaderID == Renderer::SHADER_PBR_EQUI_TO_CUBE_GEN) {
		renderData->add(BLOCK_PBR_ENV_MAP_GEN,   getUBO(BLOCK_PBR_ENV_MAP_GEN));
	} else if (shaderID == Renderer::SHADER_PBR_IRRADIANCE_MAP_GEN) {

	} else if (shaderID == Renderer::SHADER_PBR_PREFILTER_MAP_GEN) {
		renderData->add(UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN, getUBO(UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN));
	} else if (shaderID == Renderer::SHADER_PBR_BRDF_INTEGRATION_MAP_GEN) {

	} else if (shaderID == Renderer::SHADER_PBR_LIGHTING) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
		renderData->add(BLOCK_PBR_LIGHTING_CORE, getUBO(BLOCK_PBR_LIGHTING_CORE));
	} else if (shaderID == Renderer::SHADER_PBR_DEFERRED_LIGHTING) {
		renderData->add(BLOCK_CORE,              getUBO(BLOCK_CORE));
		renderData->add(BLOCK_MATERIAL,          getUBO(BLOCK_MATERIAL));
		renderData->add(BLOCK_SKINNING,          getUBO(BLOCK_SKINNING));
		renderData->add(BLOCK_LIGHTING,          getUBO(BLOCK_LIGHTING));
		renderData->add(BLOCK_PBR_LIGHTING_CORE, getUBO(BLOCK_PBR_LIGHTING_CORE));
	}
}

UBO* ShaderInterface::getUBO(unsigned int id) {
	if (ubos.count(id) > 0 && (! Window::getCurrentInstance()->getSettings().videoVulkan))
		return ubos.at(id);
	else if (ubosInfo.count(id) > 0) {
		//Create the UBO and then return it after adding it to the created UBO's
		UBOInfo& info = ubosInfo.at(id);
		UBO* ubo = new UBO(NULL, info.size, info.usage, info.binding);
		//Add it to the correct place
		if (Window::getCurrentInstance()->getSettings().videoVulkan)
			ubosVk.push_back(ubo);
		else
			ubos.insert(std::pair<unsigned int, UBO*>(id, ubo));
		return ubo;
	} else {
		Logger::log("The UBO with the id '" + utils_string::str(id) + "' could not be found", "ShaderInterface", LogType::Error);
		return NULL;
	}
}
