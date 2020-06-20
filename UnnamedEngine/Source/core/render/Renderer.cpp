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

#include "Renderer.h"

#include <algorithm>

#include "../BaseEngine.h"
#include "RenderScene.h"
#include "Tilemap.h"
#include "../particles/ParticleSystem.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"
#include "../../utils/VulkanUtils.h"

 /*****************************************************************************
  * The Renderer class
  *****************************************************************************/

ShaderInterface* Renderer::shaderInterface;

ShaderBlock_Skinning Renderer::shaderSkinningData;

std::vector<Camera*> Renderer::cameras;
std::unordered_map<unsigned int, Renderer::UnloadedShaderInfo> Renderer::unloadedShaders;
std::unordered_map<unsigned int, RenderShader*> Renderer::loadedRenderShaders;
std::unordered_map<unsigned int, Renderer::UnloadedGraphicsPipelineLayoutInfo> Renderer::unloadedGraphicsPipelineLayouts;
std::unordered_map<unsigned int, GraphicsPipelineLayout*> Renderer::loadedGraphicsPipelineLayouts;
std::unordered_map<unsigned int, std::vector<GraphicsPipeline*>> Renderer::queuePipelines;
Texture* Renderer::blank;
Cubemap* Renderer::blankCubemap;

GraphicsPipeline* Renderer::currentGraphicsPipeline = NULL;
RenderPass*       Renderer::defaultRenderPass       = NULL;

void Renderer::addCamera(Camera* camera) {
	cameras.push_back(camera);
}

void Renderer::removeCamera() {
	cameras.pop_back();
}

Camera* Renderer::getCamera() {
	if (cameras.size() > 0)
		return cameras.back();
	else {
		Logger::log("No Camera added, nothing will render", "Renderer", LogType::Warning);
		return NULL;
	}
}

void Renderer::initialise() {
	//Create the shader interface
	shaderInterface = new ShaderInterface();

	blank = Texture::loadTexture("resources/textures/blank.png");
	blankCubemap = new Cubemap("resources/textures/", { "blank.png", "blank.png", "blank.png", "blank.png", "blank.png", "blank.png" });

	//Setup the shaders
	addRenderShader(SHADER_MATERIAL, "MaterialShader");
	addRenderShader(SHADER_SKY_BOX, "SkyBoxShader");
	addRenderShader(SHADER_FONT, "FontShader");
	addRenderShader(SHADER_FONT_SDF, "FontSDFShader");
	addRenderShader(SHADER_LIGHTING, "lighting/LightingShader");
	addRenderShader(SHADER_LIGHTING_SKINNING, "lighting/LightingShader", { "UE_SKINNING" });
	addRenderShader(SHADER_BASIC_PBR_LIGHTING, "basicpbr/PBRShader");
	addRenderShader(SHADER_BASIC_PBR_LIGHTING_SKINNING, "basicpbr/PBRShader", { "UE_SKINNING" });
	addRenderShader(SHADER_FRAMEBUFFER, "FramebufferShader");
	addRenderShader(SHADER_SHADOW_MAP, "lighting/ShadowMapShader");
	addRenderShader(SHADER_SHADOW_MAP_SKINNING, "lighting/ShadowMapShader", { "UE_SKINNING" });
	addRenderShader(SHADER_SHADOW_CUBEMAP, "lighting/ShadowCubemapShader");
	addRenderShader(SHADER_SHADOW_CUBEMAP_SKINNING, "lighting/ShadowCubemapShader", { "UE_SKINNING" });
	addRenderShader(SHADER_GAMMA_CORRECTION_FXAA, "postprocessing/GammaCorrectionFXAAShader");
	addRenderShader(SHADER_DEFERRED_LIGHTING_GEOMETRY, "lighting/DeferredLightingGeometry", { "UE_GEOMETRY_ONLY" });
	addRenderShader(SHADER_DEFERRED_LIGHTING_SKINNING_GEOMETRY, "lighting/DeferredLightingGeometry", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });
	addRenderShader(SHADER_DEFERRED_LIGHTING, "lighting/DeferredLighting");
	addRenderShader(SHADER_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY, "basicpbr/PBRDeferredGeometry", { "UE_GEOMETRY_ONLY" });
	addRenderShader(SHADER_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, "basicpbr/PBRDeferredGeometry", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });
	addRenderShader(SHADER_BASIC_PBR_DEFERRED_LIGHTING, "basicpbr/PBRDeferredLighting");
	addRenderShader(SHADER_DEFERRED_PBR_SSR, "postprocessing/SSRShader");
	addRenderShader(SHADER_TILEMAP, "TilemapShader");
	addRenderShader(SHADER_PARTICLE_SYSTEM, "ParticleShader");
	addRenderShader(SHADER_TERRAIN, "terrain/Terrain");
	addRenderShader(SHADER_DEFERRED_TERRAIN_GEOMETRY, "terrain/DeferredTerrainGeometry", { "UE_GEOMETRY_ONLY" });
	addRenderShader(SHADER_PBR_GEN_EQUI_TO_CUBE_MAP, "pbr/GenEquiToCube");
	addRenderShader(SHADER_PBR_GEN_IRRADIANCE_MAP, "pbr/GenIrradianceMap");
	addRenderShader(SHADER_PBR_GEN_PREFILTER_MAP, "pbr/GenPrefilterMap");
	addRenderShader(SHADER_PBR_GEN_BRDF_INTEGRATION_MAP, "pbr/GenBRDFIntegrationMap");
	addRenderShader(SHADER_PBR_LIGHTING, "pbr/PBRShader");
	addRenderShader(SHADER_PBR_LIGHTING_SKINNING, "pbr/PBRShader", { "UE_SKINNING" });
	addRenderShader(SHADER_PBR_DEFERRED_LIGHTING_GEOMETRY, "pbr/PBRDeferredGeometry", { "UE_GEOMETRY_ONLY" });
	addRenderShader(SHADER_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, "pbr/PBRDeferredGeometry", { "UE_GEOMETRY_ONLY", "UE_SKINNING" });
	addRenderShader(SHADER_PBR_DEFERRED_LIGHTING, "pbr/PBRDeferredLighting");
	addRenderShader(SHADER_BILLBOARDED_FONT, "billboard/BillboardedFontShader");
	addRenderShader(SHADER_BILLBOARDED_FONT_SDF, "billboard/BillboardedFontSDFShader");

	//Default colour blend state
	GraphicsPipeline::ColourBlendState defaultBlendState;

	//Colour blend state for transparency
	GraphicsPipeline::ColourBlendState alphaBlendState;
	alphaBlendState.blendEnabled = true;
	alphaBlendState.srcRGB = GraphicsPipeline::BlendFactor::SRC_ALPHA;
	alphaBlendState.dstRGB = GraphicsPipeline::BlendFactor::ONE_MINUS_SRC_ALPHA;
	alphaBlendState.srcAlpha = GraphicsPipeline::BlendFactor::ONE;
	alphaBlendState.dstAlpha = GraphicsPipeline::BlendFactor::ZERO;

	//Colour blend state for blending lighting
	GraphicsPipeline::ColourBlendState alphaLightBlendState;
	alphaLightBlendState.blendEnabled = true;
	alphaLightBlendState.srcRGB = GraphicsPipeline::BlendFactor::ONE;
	alphaLightBlendState.dstRGB = GraphicsPipeline::BlendFactor::ONE;
	alphaLightBlendState.srcAlpha = GraphicsPipeline::BlendFactor::ONE;
	alphaLightBlendState.dstAlpha = GraphicsPipeline::BlendFactor::ZERO;

	//Depth state for normal depth testing
	GraphicsPipeline::DepthState defaultDepthState;

	//Depth state for skyboxes
	GraphicsPipeline::DepthState skyboxDepthState;
	skyboxDepthState.depthTestEnable = true;
	skyboxDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	skyboxDepthState.depthWriteEnable = false;

	//Depth state for font
	GraphicsPipeline::DepthState fontDepthState;
	fontDepthState.depthTestEnable = false;
	fontDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS;
	fontDepthState.depthWriteEnable = true;

	//Depth state for lighting
	GraphicsPipeline::DepthState lightDepthState;
	lightDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS;

	//Depth state for light blending
	GraphicsPipeline::DepthState lightBlendDepthState;
	lightBlendDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	lightBlendDepthState.depthWriteEnable = true;

	//Default cull state
	GraphicsPipeline::CullState defaultCullState;

	//Lighting cull state
	GraphicsPipeline::CullState lightingCullState;
	lightingCullState.mode      = GraphicsPipeline::CullMode::BACK;
	lightingCullState.frontFace = GraphicsPipeline::FrontFace::COUNTER_CLOCKWISE;

	//Post processing depth state
	GraphicsPipeline::DepthState postProcessDepthState;
	postProcessDepthState.depthTestEnable = false;
	postProcessDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	postProcessDepthState.depthWriteEnable = false;

	//Depth state for particle systems
	GraphicsPipeline::DepthState particleSystemDepthState;
	particleSystemDepthState.depthTestEnable = true;
	particleSystemDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS;
	particleSystemDepthState.depthWriteEnable = false;

	//Depth state for billboarded font
	GraphicsPipeline::DepthState billboardedFontDepthState;
	billboardedFontDepthState.depthTestEnable = true;
	billboardedFontDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	billboardedFontDepthState.depthWriteEnable = false;

	//Obtain the window width and height
	uint32_t windowWidth = Window::getCurrentInstance()->getSettings().windowWidth;
	uint32_t windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;

	//Setup the default pipelines
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_MATERIAL, SHADER_MATERIAL, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, defaultDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SKY_BOX, SHADER_SKY_BOX, MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::Flag::NONE), defaultBlendState, skyboxDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_FONT, SHADER_FONT, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, fontDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_FONT_SDF, SHADER_FONT_SDF, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, fontDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING, SHADER_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_BLEND, SHADER_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_SKINNING, SHADER_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND, SHADER_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING, SHADER_BASIC_PBR_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_BLEND, SHADER_BASIC_PBR_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING, SHADER_BASIC_PBR_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING_BLEND, SHADER_BASIC_PBR_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_MAP, SHADER_SHADOW_MAP, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING, SHADER_SHADOW_MAP_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_CUBEMAP,SHADER_SHADOW_CUBEMAP, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_CUBEMAP_SKINNING, SHADER_SHADOW_CUBEMAP_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_GUI, SHADER_MATERIAL, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, defaultDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_GAMMA_CORRECTION_FXAA, SHADER_GAMMA_CORRECTION_FXAA, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaBlendState, postProcessDepthState, defaultCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_LIGHTING_GEOMETRY, SHADER_DEFERRED_LIGHTING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_LIGHTING_SKINNING_GEOMETRY, SHADER_DEFERRED_LIGHTING_SKINNING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_LIGHTING, SHADER_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_LIGHTING_BLEND, SHADER_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaLightBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY, SHADER_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, SHADER_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING, SHADER_BASIC_PBR_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLEND, SHADER_BASIC_PBR_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaLightBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_PBR_SSR, SHADER_DEFERRED_PBR_SSR, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SPRITE, SHADER_MATERIAL, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, defaultDepthState, defaultCullState, windowWidth, windowHeight, true);
	
	GraphicsPipeline::VertexInputData tilemapVertexInputData;
	tilemapVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ShaderInterface::ATTRIBUTE_LOCATION_POSITION, 0, VK_FORMAT_R32G32_SFLOAT, 0));
	tilemapVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(0, 2 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX));
	tilemapVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ShaderInterface::ATTRIBUTE_LOCATION_TEXTURE_COORD, 1, VK_FORMAT_R32G32_SFLOAT, 0));
	tilemapVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(1, 2 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX));
	tilemapVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(TilemapLayer::ATTRIBUTE_LOCATION_VISIBILITY, 2, VK_FORMAT_R32_SFLOAT, 0));
	tilemapVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(2, 1 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX));

	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_TILEMAP, SHADER_TILEMAP, tilemapVertexInputData, alphaBlendState, defaultDepthState, defaultCullState, windowWidth, windowHeight, true);

	GraphicsPipeline::VertexInputData particleSystemVertexInputData;
	particleSystemVertexInputData.primitiveTopology = GraphicsPipeline::PrimitiveTopology::TRIANGLE_STRIP;
	particleSystemVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ShaderInterface::ATTRIBUTE_LOCATION_POSITION, 0, VK_FORMAT_R32G32B32_SFLOAT, 0));
	particleSystemVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(0, 3 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX));
	particleSystemVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ParticleSystem::ATTRIBUTE_LOCATION_POSITION_DATA, 1, VK_FORMAT_R32G32B32A32_SFLOAT, 0));
	particleSystemVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(1, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_INSTANCE));
	particleSystemVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ParticleSystem::ATTRIBUTE_LOCATION_COLOUR, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 0));
	particleSystemVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(2, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_INSTANCE));
	particleSystemVertexInputData.attributes.push_back(utils_vulkan::initVertexAttributeDescription(ParticleSystem::ATTRIBUTE_LOCATION_TEXTURE_DATA, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0));
	particleSystemVertexInputData.bindings.push_back(utils_vulkan::initVertexInputBindings(3, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_INSTANCE));

	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PARTICLE_SYSTEM, SHADER_PARTICLE_SYSTEM, particleSystemVertexInputData, alphaBlendState, particleSystemDepthState, defaultCullState, windowWidth, windowHeight, true);

	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_TERRAIN, SHADER_TERRAIN, MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::SEPARATE_POSITIONS), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_TERRAIN_BLEND, SHADER_TERRAIN, MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::SEPARATE_POSITIONS), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);

	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_DEFERRED_TERRAIN_GEOMETRY, SHADER_DEFERRED_TERRAIN_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::SEPARATE_POSITIONS), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);

	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_LIGHTING, SHADER_PBR_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_LIGHTING_BLEND, SHADER_PBR_LIGHTING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING, SHADER_PBR_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING_BLEND, SHADER_PBR_LIGHTING_SKINNING, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_GEOMETRY, SHADER_PBR_DEFERRED_LIGHTING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, SHADER_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), defaultBlendState, lightDepthState, lightingCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING, SHADER_PBR_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLEND, SHADER_PBR_DEFERRED_LIGHTING, MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::NONE), alphaLightBlendState, postProcessDepthState, lightingCullState, windowWidth, windowHeight, false);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BILLBOARDED_FONT, SHADER_BILLBOARDED_FONT, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, billboardedFontDepthState, defaultCullState, windowWidth, windowHeight, true);
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BILLBOARDED_FONT_SDF, SHADER_BILLBOARDED_FONT_SDF, MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, billboardedFontDepthState, defaultCullState, windowWidth, windowHeight, true);

	//Create the default render pass
	defaultRenderPass = new RenderPass();

	//Setup swap chain with the default render pass if necessary
	if (BaseEngine::usingVulkan())
		Vulkan::getSwapChain()->setupDefaultFramebuffers(defaultRenderPass);
}

void Renderer::useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material) {
	//Bind the material descriptor set
	material->getDescriptorSet()->bind();
}

void Renderer::stopUsingMaterial(Material* material) {
	//Unbind the textures
	material->getDescriptorSet()->unbind();
}

void Renderer::render(RenderData* renderData, Matrix4f& modelMatrix, Material* material) {
	//Obtain the required UBO's for rendering
	UBO* shaderModelUBO = renderData->getDescriptorSet()->getUBO(0);
	UBO* shaderSkinningUBO = NULL;
	if (renderData->getDescriptorSet()->getNumUBOs() > 1 && renderData->getDescriptorSet()->getUBO(1)->getBinding() == ShaderInterface::UBO_BINDING_LOCATION_SKINNING + (BaseEngine::usingVulkan() ? UBO::VULKAN_BINDING_OFFSET : 0))
		shaderSkinningUBO = renderData->getDescriptorSet()->getUBO(1);

	renderData->getShaderBlock_Model().ue_mvpMatrix = (getCamera()->getProjectionViewMatrix() * modelMatrix);
	renderData->getShaderBlock_Model().ue_modelMatrix = modelMatrix;
	renderData->getShaderBlock_Model().ue_normalMatrix = Matrix4f(modelMatrix.to3x3().inverse().transpose());

	shaderModelUBO->updateFrame(&renderData->getShaderBlock_Model(), 0, sizeof(ShaderBlock_Model));

	renderData->getDescriptorSet()->bind();

	if (material)
		useMaterial(renderData, 0, material);
	renderData->render();
	if (material)
		stopUsingMaterial(material);
}

void Renderer::render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* renderShader) {
	//Ensure there is a Shader and Camera instance for rendering
	if (renderShader && getCamera()) {
		//Only use materials if necessary
		bool shouldUseMaterial = (renderShader->getID() != SHADER_SHADOW_MAP) && (renderShader->getID() != SHADER_SHADOW_MAP_SKINNING) && (renderShader->getID() != SHADER_SHADOW_CUBEMAP) && (renderShader->getID() != SHADER_SHADOW_CUBEMAP_SKINNING);

		//Get the render data for rendering
		RenderData* renderData = mesh->getRenderData()->getRenderData();

		//Obtain the required UBO's for rendering
		UBO* shaderModelUBO = renderData->getDescriptorSet()->getUBO(0);
		UBO* shaderSkinningUBO = NULL;
		if (renderData->getDescriptorSet()->getNumUBOs() > 1 && renderData->getDescriptorSet()->getUBO(1)->getBinding() == ShaderInterface::UBO_BINDING_LOCATION_SKINNING + (BaseEngine::usingVulkan() ? UBO::VULKAN_BINDING_OFFSET : 0))
			shaderSkinningUBO = renderData->getDescriptorSet()->getUBO(1);

		renderData->getShaderBlock_Model().ue_mvpMatrix = (getCamera()->getProjectionViewMatrix() * modelMatrix);
		renderData->getShaderBlock_Model().ue_modelMatrix = modelMatrix;
		renderData->getShaderBlock_Model().ue_normalMatrix = Matrix4f(modelMatrix.to3x3().inverse().transpose());

		shaderModelUBO->updateFrame(&renderData->getShaderBlock_Model(), 0, sizeof(ShaderBlock_Model));

		renderData->getDescriptorSet()->bind();

		if (mesh->hasData() && mesh->hasRenderData() && shouldUseMaterial) {
			MeshData* data = mesh->getData();
			MeshRenderData* meshRenderData = mesh->getRenderData();

			if (shaderSkinningUBO) {
				if (mesh->hasSkeleton()) {
					for (unsigned int i = 0; i < mesh->getSkeleton()->getNumBones(); ++i)
						shaderSkinningData.ue_bones[i] = mesh->getSkeleton()->getBone(i)->getFinalTransform();
					shaderSkinningData.ue_useSkinning = true;
					shaderSkinningUBO->updateFrame(&shaderSkinningData, 0, sizeof(ShaderBlock_Skinning));
				} else {
					shaderSkinningData.ue_useSkinning = false;
					shaderSkinningData.updateUseSkinning(shaderSkinningUBO);
				}
			}

			if (data->hasSubData()) { //Render in one if only being used for shadows
				renderData->bindBuffers();

				//Go through each sub data instance
				for (unsigned int i = 0; i < data->getSubDataCount(); ++i) {
					if (mesh->hasMaterial())
						useMaterial(renderData, data->getSubData(i).materialIndex, mesh->getMaterial(data->getSubData(i).materialIndex));
					renderData->renderBaseVertex(data->getSubData(i).count, data->getSubData(i).baseIndex, data->getSubData(i).baseVertex);
					if (mesh->hasMaterial())
						stopUsingMaterial(mesh->getMaterial(data->getSubData(i).materialIndex));
				}

				renderData->unbindBuffers();
			} else if (shouldUseMaterial) {
				if (mesh->hasMaterial())
					useMaterial(renderData, 0, mesh->getMaterial());
				renderData->render();
				if (mesh->hasMaterial())
					stopUsingMaterial(mesh->getMaterial());
			} else
				renderData->render();
		}
	}
}

void Renderer::destroy() {
	delete defaultRenderPass;
	delete shaderInterface;
	for (auto element : loadedRenderShaders)
		delete element.second;
	for (auto element : loadedGraphicsPipelineLayouts)
		delete element.second;
}

using namespace utils_string;

Shader* Renderer::loadEngineShader(UnloadedShaderInfo& shaderInfo) {
	if (!BaseEngine::usingVulkan())
		return Shader::loadShader("resources/shaders/" + shaderInfo.path, shaderInfo.defines);
	else
		return Shader::loadShader("resources/shaders-vulkan/" + shaderInfo.path, shaderInfo.defines);
}

void Renderer::addRenderShader(unsigned int id, std::string forwardShaderPath, std::vector<std::string> defines) {
	unloadedShaders.insert(std::pair<unsigned int, UnloadedShaderInfo>(id, { forwardShaderPath, defines }));
}

void Renderer::addGraphicsPipelineLayout(unsigned int id, unsigned int renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState, GraphicsPipeline::CullState cullState, uint32_t viewportWidth, uint32_t viewportHeight, bool viewportFlippedVk) {
	unloadedGraphicsPipelineLayouts.insert(std::pair<unsigned int, UnloadedGraphicsPipelineLayoutInfo>(id, { renderShader, vertexInputData, colourBlendState, depthState, cullState, viewportWidth, viewportHeight, viewportFlippedVk }));
}

void Renderer::setCurrentGraphicsPipeline(GraphicsPipeline* pipeline) {
	currentGraphicsPipeline = pipeline;
}

void Renderer::loadRenderShader(unsigned int id) {
	//Get the paths
	UnloadedShaderInfo shaderInfo = unloadedShaders.at(id);
	Shader* forwardShader = NULL;

	//Load the shaders if the path has been assigned
	//Setup the shader
	if (shaderInfo.path != "")
		forwardShader = loadEngineShader(shaderInfo);

	//Create the shader
	RenderShader* renderShader = new RenderShader(id, forwardShader);
	//Add required structures
	shaderInterface->setup(id, renderShader);
	//Setup
	renderShader->setup();

	//Add the shader
	addRenderShader(renderShader);
}

void Renderer::addRenderShader(RenderShader* renderShader) {
	loadedRenderShaders.insert(std::pair<unsigned int, RenderShader*>(renderShader->getID(), renderShader));
}

void Renderer::addGraphicsPipelineQueue(unsigned int id, GraphicsPipeline* pipeline) {
	if (queuePipelines.count(id) == 0)
		queuePipelines.insert(std::pair<unsigned int, std::vector<GraphicsPipeline*>>(id, { pipeline }));
	else
		queuePipelines.at(id).push_back(pipeline);
}

void Renderer::removeGraphicsPipelineQueue(unsigned int id) {
	queuePipelines.at(id).pop_back();
}

GraphicsPipeline* Renderer::getGraphicsPipelineQueue(unsigned int id) {
	std::vector<GraphicsPipeline*>& pipelines = queuePipelines.at(id);
	return pipelines.at(pipelines.size() - 1);
}

RenderShader* Renderer::getRenderShader(unsigned int id) {
	if (loadedRenderShaders.count(id) > 0)
		return loadedRenderShaders.at(id);
	else if (unloadedShaders.count(id) > 0) {
		//Load the render shader then return it
		loadRenderShader(id);
		return loadedRenderShaders.at(id);
	} else {
		Logger::log("The RenderShader with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}

GraphicsPipelineLayout* Renderer::getGraphicsPipelineLayout(unsigned int id) {
	if (loadedGraphicsPipelineLayouts.count(id) > 0)
		return loadedGraphicsPipelineLayouts.at(id);
	else if (unloadedGraphicsPipelineLayouts.count(id) > 0) {
		//Create the layout then return it
		UnloadedGraphicsPipelineLayoutInfo info = unloadedGraphicsPipelineLayouts.at(id);
		GraphicsPipelineLayout* layout = new GraphicsPipelineLayout(getRenderShader(info.renderShader), info.vertexInputData, info.colourBlendState, info.depthState, info.cullState, info.viewportWidth, info.viewportHeight, info.viewportFlippedVk);
		loadedGraphicsPipelineLayouts.insert(std::pair<unsigned int, GraphicsPipelineLayout*>(id, layout));
		return layout;
	} else {
		Logger::log("The GraphicsPipelineLayout with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}