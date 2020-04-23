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

#include "../BaseEngine.h"
#include "RenderData.h"
#include "Renderer.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The ShaderInterface class
 *****************************************************************************/

 /* Set numbers used for specific kinds of descriptor sets*/
const unsigned int ShaderInterface::DESCRIPTOR_SET_NUMBER_PER_CAMERA      = 0;
const unsigned int ShaderInterface::DESCRIPTOR_SET_NUMBER_PER_MATERIAL    = 1;
const unsigned int ShaderInterface::DESCRIPTOR_SET_NUMBER_PER_MODEL       = 2;
const unsigned int ShaderInterface::DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH = 3;

 /* IDs for descriptor set layouts */
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_CAMERA         = 0;
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_MATERIAL       = 1;
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_MODEL          = 2;
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH    = 3;
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING = 4;
const unsigned int ShaderInterface::DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP = 5;

/* The locations for attributes in the shaders */
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_POSITION      = 0;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_TEXTURE_COORD = 1;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_NORMAL        = 2;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_TANGENT       = 3;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BITANGENT     = 4;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BONE_IDS      = 5;
const unsigned int ShaderInterface::ATTRIBUTE_LOCATION_BONE_WEIGHTS  = 6;

/* The ids for particular shader blocks */
const unsigned int ShaderInterface::BLOCK_CAMERA                 = 1;
const unsigned int ShaderInterface::BLOCK_MODEL                  = 2;
const unsigned int ShaderInterface::BLOCK_MATERIAL               = 3;
const unsigned int ShaderInterface::BLOCK_SKINNING               = 4;
const unsigned int ShaderInterface::BLOCK_LIGHT_BATCH            = 5;
const unsigned int ShaderInterface::BLOCK_TERRAIN                = 6;
const unsigned int ShaderInterface::BLOCK_GAMMA_CORRECTION       = 7;
const unsigned int ShaderInterface::BLOCK_PBR_ENV_MAP_GEN        = 8;
const unsigned int ShaderInterface::BLOCK_PBR_PREFILTER_MAP_GEN  = 9;
const unsigned int ShaderInterface::BLOCK_PBR_LIGHTING_CORE      = 10;
const unsigned int ShaderInterface::BLOCK_BILLBOARD              = 11;
const unsigned int ShaderInterface::BLOCK_SHADOW_CUBEMAP         = 12;

/* Binding locations for shader blocks */
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_CAMERA                 = 1;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_MODEL                  = 2;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_MATERIAL               = 3;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SKINNING               = 4;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_LIGHT_BATCH            = 5;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_TERRAIN                = 6;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_GAMMA_CORRECTION       = 7;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_ENV_MAP_GEN        = 8;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_PREFILTER_MAP_GEN  = 9;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_PBR_LIGHTING_CORE      = 10;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_BILLBOARD              = 11;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SHADOW_CUBEMAP         = 12;

ShaderInterface::ShaderInterface() {
	//Add all of the required descriptor set layouts for the default shaders

	//Core
	DescriptorSetLayout* cameraLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_CAMERA);
	cameraLayout->addUBO(sizeof(ShaderBlock_Camera), GL_STATIC_DRAW, UBO_BINDING_LOCATION_CAMERA);
	cameraLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_CAMERA, cameraLayout);

	//Material
	DescriptorSetLayout* materialLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MATERIAL);
	materialLayout->addTexture2D(0);
	materialLayout->addTexture2D(1);
	materialLayout->addTexture2D(2);
	materialLayout->addTexture2D(3);
	materialLayout->addTexture2D(4);
	materialLayout->addTexture2D(5);

	materialLayout->addUBO(sizeof(ShaderBlock_Material), GL_STATIC_DRAW, UBO_BINDING_LOCATION_MATERIAL);

	materialLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MATERIAL, materialLayout);

	//Model
	DescriptorSetLayout* modelLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
	modelLayout->addUBO(sizeof(ShaderBlock_Model), GL_STATIC_DRAW, UBO_BINDING_LOCATION_MODEL);
	modelLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MODEL, modelLayout);

	//Light batch
	DescriptorSetLayout* lightBatchLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	//Add shadow map textures
	lightBatchLayout->addTextureBinding(DescriptorSet::TEXTURE_2D, 7, 6);
	lightBatchLayout->addTextureBinding(DescriptorSet::TEXTURE_CUBE, 13, 6);
	lightBatchLayout->addUBO(sizeof(ShaderBlock_LightBatch), GL_STATIC_DRAW, UBO_BINDING_LOCATION_LIGHT_BATCH);
	lightBatchLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH, lightBatchLayout);

	//Model skinning
	DescriptorSetLayout* modelSkinningLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
	modelSkinningLayout->addUBO(sizeof(ShaderBlock_Model), GL_STATIC_DRAW, UBO_BINDING_LOCATION_MODEL);
	modelSkinningLayout->addUBO(sizeof(ShaderBlock_Skinning), GL_STATIC_DRAW, UBO_BINDING_LOCATION_SKINNING);
	modelSkinningLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING, modelSkinningLayout);

	//Shadow cubemap
	DescriptorSetLayout* shadowCubemapLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	shadowCubemapLayout->addUBO(sizeof(ShaderBlock_ShadowCubemap), GL_STATIC_DRAW, UBO_BINDING_LOCATION_SHADOW_CUBEMAP);
	shadowCubemapLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP, shadowCubemapLayout);
}

ShaderInterface::~ShaderInterface() {
	//Go through and delete all UBO's
	for (auto it : descriptorSetLayouts)
		delete it.second;
	descriptorSetLayouts.clear();
}

void ShaderInterface::add(unsigned int id, DescriptorSetLayout* layout) {
	//Add the layout to the map
	descriptorSetLayouts.insert(std::pair<unsigned int, DescriptorSetLayout*>(id, layout));
}

void ShaderInterface::setup(unsigned int shaderID, RenderShader* renderShader) {
	//Check the shader ID and add the required descriptor set layouts
	if (shaderID == Renderer::SHADER_MATERIAL) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_SKY_BOX) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_FONT) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
	} else if (shaderID == Renderer::SHADER_LIGHTING_SKINNING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
	} else if (shaderID == Renderer::SHADER_FRAMEBUFFER) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_SHADOW_MAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_SHADOW_MAP_SKINNING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
	} else if (shaderID == Renderer::SHADER_SHADOW_CUBEMAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP));
	} else if (shaderID == Renderer::SHADER_SHADOW_CUBEMAP_SKINNING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP));
	}
}

DescriptorSetLayout* ShaderInterface::getDescriptorSetLayout(unsigned int id) {
	if (descriptorSetLayouts.count(id) > 0)
		return descriptorSetLayouts.at(id);
	else {
		Logger::log("The DescriptorSetLayout with the id '" + utils_string::str(id) + "' could not be found", "ShaderInterface", LogType::Error);
		return NULL;
	}
}