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

ShaderInterface::ShaderInterface() {
	//Add all of the required descriptor set layouts for the default shaders

	//Core
	DescriptorSetLayout* cameraLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_CAMERA);
	cameraLayout->addUBO(sizeof(ShaderBlock_Camera), DataUsage::STATIC, UBO_BINDING_LOCATION_CAMERA);
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

	materialLayout->addUBO(sizeof(ShaderBlock_Material), DataUsage::STATIC, UBO_BINDING_LOCATION_MATERIAL);

	materialLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MATERIAL, materialLayout);

	//Model
	DescriptorSetLayout* modelLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
	modelLayout->addUBO(sizeof(ShaderBlock_Model), DataUsage::STATIC, UBO_BINDING_LOCATION_MODEL);
	modelLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MODEL, modelLayout);

	//Light batch
	DescriptorSetLayout* lightBatchLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	//Add shadow map textures
	lightBatchLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 7, 6);
	lightBatchLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_CUBE, 13, 6);
	lightBatchLayout->addUBO(sizeof(ShaderBlock_LightBatch), DataUsage::STATIC, UBO_BINDING_LOCATION_LIGHT_BATCH);
	lightBatchLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH, lightBatchLayout);

	//Model skinning
	DescriptorSetLayout* modelSkinningLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_MODEL);
	modelSkinningLayout->addUBO(sizeof(ShaderBlock_Model), DataUsage::STATIC, UBO_BINDING_LOCATION_MODEL);
	modelSkinningLayout->addUBO(sizeof(ShaderBlock_Skinning), DataUsage::STATIC, UBO_BINDING_LOCATION_SKINNING);
	modelSkinningLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_MODEL_SKINNING, modelSkinningLayout);

	//Shadow cubemap
	DescriptorSetLayout* shadowCubemapLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	shadowCubemapLayout->addUBO(sizeof(ShaderBlock_ShadowCubemap), DataUsage::STATIC, UBO_BINDING_LOCATION_SHADOW_CUBEMAP);
	shadowCubemapLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP, shadowCubemapLayout);

	//Gamma correction FXAA
	DescriptorSetLayout* gammaCorrectionFXAALayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	gammaCorrectionFXAALayout->addUBO(sizeof(ShaderBlock_GammaCorrectionFXAA), DataUsage::STATIC, UBO_BINDING_LOCATION_GAMMA_CORRECTION_FXAA);
	gammaCorrectionFXAALayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_GAMMA_CORRECTION_FXAA, gammaCorrectionFXAALayout);

	//Deferred lighting
	DescriptorSetLayout* deferredLightingLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
	deferredLightingLayout->addTexture2D(0);
	deferredLightingLayout->addTexture2D(1);
	deferredLightingLayout->addTexture2D(2);

	deferredLightingLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_DEFERRED_LIGHTING, deferredLightingLayout);

	//Deferred PBR lighting
	DescriptorSetLayout* pbrDeferredLightingLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
	pbrDeferredLightingLayout->addTexture2D(0);
	pbrDeferredLightingLayout->addTexture2D(1);
	pbrDeferredLightingLayout->addTexture2D(2);
	pbrDeferredLightingLayout->addTexture2D(3);

	pbrDeferredLightingLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING, pbrDeferredLightingLayout);

	//Deferred PBR SSR lighting
	DescriptorSetLayout* pbrDeferredLightingSSRLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH); //Don't use light batches, so need to change set number
	pbrDeferredLightingSSRLayout->addTexture2D(0);
	pbrDeferredLightingSSRLayout->addTexture2D(1);
	pbrDeferredLightingSSRLayout->addTexture2D(2);
	pbrDeferredLightingSSRLayout->addTexture2D(3);

	pbrDeferredLightingSSRLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_DEFERRED_PBR_SSR, pbrDeferredLightingSSRLayout);

	//Billboard
	DescriptorSetLayout* billboardLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	billboardLayout->addUBO(sizeof(ShaderBlock_Billboard), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_BILLBOARD);
	billboardLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_BILLBOARD, billboardLayout);

	//CDLOD terrain
	DescriptorSetLayout* cdlodTerrainLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);

	cdlodTerrainLayout->addTexture2D(6); //Height map

	cdlodTerrainLayout->addUBO(sizeof(ShaderBlock_Terrain), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_TERRAIN);
	cdlodTerrainLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_TERRAIN, cdlodTerrainLayout);

	//PBR environment
	DescriptorSetLayout* pbrEnvironmentLayout = new DescriptorSetLayout(5);
	pbrEnvironmentLayout->addTextureCube(20);
	pbrEnvironmentLayout->addTextureCube(21);
	pbrEnvironmentLayout->addTexture2D(22);

	pbrEnvironmentLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT, pbrEnvironmentLayout);

	//SDF Text
	DescriptorSetLayout* sdfTextLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_LIGHT_BATCH);
	sdfTextLayout->addUBO(sizeof(ShaderBlock_SDFText), DataUsage::STATIC, UBO_BINDING_LOCATION_SDF_TEXT);
	sdfTextLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_SDF_TEXT, sdfTextLayout);

	//Billboarded SDF Text
	DescriptorSetLayout* billboardSDFTextLayout = new DescriptorSetLayout(DESCRIPTOR_SET_NUMBER_PER_SCENE);
	billboardSDFTextLayout->addUBO(sizeof(ShaderBlock_Billboard), DataUsage::DYNAMIC, UBO_BINDING_LOCATION_BILLBOARD);
	billboardSDFTextLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_BILLBOARD_SDF_TEXT, billboardSDFTextLayout);

	//PBR GenEquiToCubeMap
	DescriptorSetLayout* pbrGenEquiToCubeMapLayout = new DescriptorSetLayout(0);

	pbrGenEquiToCubeMapLayout->addTexture2D(0);

	pbrGenEquiToCubeMapLayout->addUBO(sizeof(ShaderBlock_PBRGenEnvMap), DataUsage::STATIC, 0);

	pbrGenEquiToCubeMapLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP, pbrGenEquiToCubeMapLayout);

	//PBR GenIrradianceMap
	DescriptorSetLayout* pbrGenIrradianceMapLayout = new DescriptorSetLayout(0);

	pbrGenIrradianceMapLayout->addTextureCube(0);

	pbrGenIrradianceMapLayout->addUBO(sizeof(ShaderBlock_PBRGenEnvMap), DataUsage::STATIC, 0);

	pbrGenIrradianceMapLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP, pbrGenIrradianceMapLayout);

	//PBR GenPrefilterMap
	DescriptorSetLayout* pbrGenPrefilterMapLayout = new DescriptorSetLayout(1);

	pbrGenPrefilterMapLayout->addUBO(sizeof(ShaderBlock_PBRGenPrefilterMap), DataUsage::STATIC, 1);

	pbrGenPrefilterMapLayout->setup();

	add(DESCRIPTOR_SET_DEFAULT_PBR_GEN_PREFILTER_MAP, pbrGenPrefilterMapLayout);
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