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
	} else if (shaderID == Renderer::SHADER_FONT_SDF) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_SDF_TEXT));
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
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_LIGHTING_SKINNING) {
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
	} else if (shaderID == Renderer::SHADER_GAMMA_CORRECTION_FXAA) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_GAMMA_CORRECTION_FXAA));
	} else if (shaderID == Renderer::SHADER_DEFERRED_LIGHTING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_DEFERRED_LIGHTING_SKINNING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
	} else if (shaderID == Renderer::SHADER_DEFERRED_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_DEFERRED_LIGHTING));
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_DEFERRED_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING));
	} else if (shaderID == Renderer::SHADER_BASIC_PBR_DEFERRED_LIGHTING_BLOOM) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING));
	} else if (shaderID == Renderer::SHADER_DEFERRED_PBR_SSR) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_DEFERRED_PBR_SSR));
	} else if (shaderID == Renderer::SHADER_TILEMAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_PARTICLE_SYSTEM) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BILLBOARD));
	} else if (shaderID == Renderer::SHADER_TERRAIN) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_TERRAIN));
	} else if (shaderID == Renderer::SHADER_DEFERRED_TERRAIN_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH)); //Shouldn't really be here but Vulkan won't complain if it is
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_TERRAIN));
	} else if (shaderID == Renderer::SHADER_PBR_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));

		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT));
	} else if (shaderID == Renderer::SHADER_PBR_LIGHTING_SKINNING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));

		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT));
	} else if (shaderID == Renderer::SHADER_PBR_DEFERRED_LIGHTING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING));
	} else if (shaderID == Renderer::SHADER_PBR_DEFERRED_LIGHTING) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT));
	} else if (shaderID == Renderer::SHADER_PBR_DEFERRED_LIGHTING_BLOOM) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT));
	} else if (shaderID == Renderer::SHADER_BILLBOARDED_FONT) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BILLBOARD));
	} else if (shaderID == Renderer::SHADER_BILLBOARDED_FONT_SDF) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_SDF_TEXT));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_BILLBOARD_SDF_TEXT));
	} else if (shaderID == Renderer::SHADER_PBR_GEN_EQUI_TO_CUBE_MAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP));
	} else if (shaderID == Renderer::SHADER_PBR_GEN_IRRADIANCE_MAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP));
	} else if (shaderID == Renderer::SHADER_PBR_GEN_PREFILTER_MAP) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_PBR_GEN_PREFILTER_MAP));
	} else if (shaderID == Renderer::SHADER_GAUSSIAN_BLUR) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_GAUSSIAN_BLUR));
	} else if (shaderID == Renderer::SHADER_BLOOM) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	} else if (shaderID == Renderer::SHADER_TEXTURE_PASSTHROUGH) {
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_CAMERA));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MATERIAL));
		renderShader->add(getDescriptorSetLayout(DESCRIPTOR_SET_DEFAULT_MODEL));
	}
}

DescriptorSetLayout* ShaderInterface::createDescriptorSetLayout(unsigned int id) {
	//DescriptorSetLayout to return
	DescriptorSetLayout* layout = NULL;

	//Shader stage flags (for Vulkan RT)
	VkShaderStageFlags shaderRTStageFlags = Window::getCurrentInstance()->getSettings().videoRaytracing ? (VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_RAYGEN_BIT_KHR) : VK_SHADER_STAGE_ALL_GRAPHICS;

	//Assign the layout based on the ID
	switch (id) {
		case DESCRIPTOR_SET_DEFAULT_CAMERA:
			//Camera
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_CAMERA);
			layout->addUBO(sizeof(ShaderBlock_Camera), DataUsage::STATIC, UBO_BINDING_LOCATION_CAMERA, shaderRTStageFlags);
			break;
		case DESCRIPTOR_SET_DEFAULT_MATERIAL:
			//Material
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MATERIAL);
			layout->addTexture2D(0);
			layout->addTexture2D(1);
			layout->addTexture2D(2);
			layout->addTexture2D(3);
			layout->addTexture2D(4);
			layout->addTexture2D(5);

			layout->addUBO(sizeof(ShaderBlock_Material), DataUsage::STATIC, UBO_BINDING_LOCATION_MATERIAL);
			break;
		case DESCRIPTOR_SET_DEFAULT_MODEL:
			//Model
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
			layout->addUBO(sizeof(ShaderBlock_Model), DataUsage::STATIC, UBO_BINDING_LOCATION_MODEL);
			break;
		case DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH:
			//Light batch
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			//Add shadow map textures
			layout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 7, 6);
			layout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_CUBE, 13, 6);
			layout->addUBO(sizeof(ShaderBlock_LightBatch), DataUsage::STATIC, UBO_BINDING_LOCATION_LIGHT_BATCH);
			break;
		case DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING:
			//Model skinning
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
			layout->addUBO(sizeof(ShaderBlock_Model), DataUsage::STATIC, UBO_BINDING_LOCATION_MODEL);
			layout->addUBO(sizeof(ShaderBlock_Skinning), DataUsage::STATIC, UBO_BINDING_LOCATION_SKINNING);
			break;
		case DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP:
			//Shadow cubemap
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			layout->addUBO(sizeof(ShaderBlock_ShadowCubemap), DataUsage::STATIC, UBO_BINDING_LOCATION_SHADOW_CUBEMAP);
			break;
		case DESCRIPTOR_SET_DEFAULT_GAMMA_CORRECTION_FXAA:
			//Gamma correction FXAA
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			layout->addUBO(sizeof(ShaderBlock_GammaCorrectionFXAA), DataUsage::STATIC, UBO_BINDING_LOCATION_GAMMA_CORRECTION_FXAA);
			break;
		case DESCRIPTOR_SET_DEFAULT_DEFERRED_LIGHTING:
			//Deferred lighting
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
			layout->addTexture2D(0);
			layout->addTexture2D(1);
			layout->addTexture2D(2);
			break;
		case DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING:
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
			layout->addTexture2D(0);
			layout->addTexture2D(1);
			layout->addTexture2D(2);
			layout->addTexture2D(3);
			break;
		case DESCRIPTOR_SET_DEFAULT_DEFERRED_PBR_SSR:
			//Deferred PBR SSR lighting
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH); //Don't use light batches, so need to change set number
			layout->addTexture2D(0);
			layout->addTexture2D(1);
			layout->addTexture2D(2);
			layout->addTexture2D(3);
			break;
		case DESCRIPTOR_SET_DEFAULT_BILLBOARD:
			//Billboard
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			layout->addUBO(sizeof(ShaderBlock_Billboard), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_BILLBOARD);
			break;
		case DESCRIPTOR_SET_DEFAULT_TERRAIN:
			//CDLOD terrain
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);

			layout->addTexture2D(6); //Height map

			layout->addUBO(sizeof(ShaderBlock_Terrain), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_TERRAIN);
			break;
		case DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT:
			//PBR environment
			layout = new DescriptorSetLayout(5);
			layout->addTextureCube(20);
			layout->addTextureCube(21);
			layout->addTexture2D(22);
			break;
		case DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT_NO_DEFERRED:
			//PBR environment
			layout = new DescriptorSetLayout(4);
			layout->addTextureCube(20);
			layout->addTextureCube(21);
			layout->addTexture2D(22);

			break;
		case DESCRIPTOR_SET_DEFAULT_SDF_TEXT:
			//SDF Text
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			layout->addUBO(sizeof(ShaderBlock_SDFText), DataUsage::STATIC, UBO_BINDING_LOCATION_SDF_TEXT);
			break;
		case DESCRIPTOR_SET_DEFAULT_BILLBOARD_SDF_TEXT:
			//Billboarded SDF Text
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
			layout->addUBO(sizeof(ShaderBlock_Billboard), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_BILLBOARD);
			break;
		case DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP:
			//PBR GenEquiToCubeMap
			layout = new DescriptorSetLayout(0);

			layout->addTexture2D(0);

			layout->addUBO(sizeof(ShaderBlock_PBRGenEnvMap), DataUsage::STATIC, 0);

			break;
		case DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP:
			//PBR GenIrradianceMap
			layout = new DescriptorSetLayout(0);

			layout->addTextureCube(0);

			layout->addUBO(sizeof(ShaderBlock_PBRGenEnvMap), DataUsage::STATIC, 0);

			break;
		case DESCRIPTOR_SET_DEFAULT_PBR_GEN_PREFILTER_MAP:
			//PBR GenPrefilterMap
			layout = new DescriptorSetLayout(1);

			layout->addUBO(sizeof(ShaderBlock_PBRGenPrefilterMap), DataUsage::STATIC, 1);

			break;
		case DESCRIPTOR_SET_DEFAULT_GAUSSIAN_BLUR:
			//Gaussian blur
			layout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
			layout->addUBO(sizeof(ShaderBlock_GaussianBlur), DataUsage::STATIC, UBO_BINDING_LOCATION_GAUSSIAN_BLUR);
			break;
		case DESCRIPTOR_SET_DEFAULT_RAYTRACING:
			//Default raytracing (experimental)
			layout = new DescriptorSetLayout(1);
			layout->addAccelerationStructure(0);
			layout->addStorageTexture(1);
			break;
	}

	//Setup the layout if found
	if (layout != NULL)
		layout->setup();

	//Return the layout
	return layout;
}

DescriptorSetLayout* ShaderInterface::getDescriptorSetLayout(unsigned int id) {
	auto descriptorSetLayoutIt = descriptorSetLayouts.find(id);
	if (descriptorSetLayoutIt != descriptorSetLayouts.end())
		return descriptorSetLayoutIt->second;
	else {
		//Attempt to create one
		DescriptorSetLayout* layout = createDescriptorSetLayout(id);

		if (layout != NULL)
			//Add the layout
			add(id, layout);
		else
			//Failed to create one
			Logger::log("The DescriptorSetLayout with the id '" + utils_string::str(id) + "' could not be found", "ShaderInterface", LogType::Error);

		//Return either the created layout, or NULL
		return layout;
	}
}