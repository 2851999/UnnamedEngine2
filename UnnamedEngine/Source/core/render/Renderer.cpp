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
#include "../../utils/Logging.h"
#include "RenderScene.h"
#include "../vulkan/Vulkan.h"

/*****************************************************************************
 * The Renderer class
 *****************************************************************************/

ShaderInterface* Renderer::shaderInterface;

ShaderBlock_Skinning Renderer::shaderSkinningData;

std::vector<Camera*> Renderer::cameras;
std::unordered_map<unsigned int, Renderer::UnloadedShaderInfo> Renderer::unloadedShaders;
std::unordered_map<unsigned int, RenderShader*> Renderer::loadedRenderShaders;
std::unordered_map<unsigned int, GraphicsPipelineLayout*> Renderer::graphicsPipelineLayouts;
std::unordered_map<unsigned int, std::vector<GraphicsPipeline*>> Renderer::queuePipelines;
Texture* Renderer::blank;
Cubemap* Renderer::blankCubemap;

GraphicsPipeline* Renderer::currentGraphicsPipeline = NULL;
RenderPass*       Renderer::defaultRenderPass       = NULL;

const unsigned int Renderer::SHADER_MATERIAL                    = 1;
const unsigned int Renderer::SHADER_SKY_BOX                     = 2;
const unsigned int Renderer::SHADER_FONT                        = 3;
const unsigned int Renderer::SHADER_FONT_SDF                    = 4;
const unsigned int Renderer::SHADER_LIGHTING                    = 5;
const unsigned int Renderer::SHADER_LIGHTING_SKINNING           = 6;
const unsigned int Renderer::SHADER_BASIC_PBR_LIGHTING          = 7;
const unsigned int Renderer::SHADER_BASIC_PBR_LIGHTING_SKINNING = 8;
const unsigned int Renderer::SHADER_FRAMEBUFFER                 = 9;
const unsigned int Renderer::SHADER_SHADOW_MAP                  = 10;
const unsigned int Renderer::SHADER_SHADOW_MAP_SKINNING         = 11;
const unsigned int Renderer::SHADER_SHADOW_CUBEMAP              = 12;
const unsigned int Renderer::SHADER_SHADOW_CUBEMAP_SKINNING     = 13;

const unsigned int Renderer::GRAPHICS_PIPELINE_MATERIAL                          = 1;
const unsigned int Renderer::GRAPHICS_PIPELINE_SKY_BOX                           = 2;
const unsigned int Renderer::GRAPHICS_PIPELINE_FONT                              = 3;
const unsigned int Renderer::GRAPHICS_PIPELINE_FONT_SDF                          = 4;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING                          = 5;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_BLEND                    = 6;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING                 = 7;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND           = 8;
const unsigned int Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING                = 9;
const unsigned int Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_BLEND          = 10;
const unsigned int Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING       = 11;
const unsigned int Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING_BLEND = 12;
const unsigned int Renderer::GRAPHICS_PIPELINE_SHADOW_MAP                        = 13;
const unsigned int Renderer::GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING               = 14;
const unsigned int Renderer::GRAPHICS_PIPELINE_SHADOW_CUBEMAP                    = 15;
const unsigned int Renderer::GRAPHICS_PIPELINE_SHADOW_CUBEMAP_SKINNING           = 16;
const unsigned int Renderer::GRAPHICS_PIPELINE_GUI                               = 17;

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
	addRenderShader(SHADER_MATERIAL,                    "MaterialShader");
	addRenderShader(SHADER_SKY_BOX,                     "SkyBoxShader");
	addRenderShader(SHADER_FONT,                        "FontShader");
	addRenderShader(SHADER_FONT_SDF,                    "FontSDFShader");
	addRenderShader(SHADER_LIGHTING,                    "lighting/LightingShader");
	addRenderShader(SHADER_LIGHTING_SKINNING,           "lighting/LightingShader", { "UE_SKINNING" });
	addRenderShader(SHADER_BASIC_PBR_LIGHTING,          "basicpbr/PBRShader");
	addRenderShader(SHADER_BASIC_PBR_LIGHTING_SKINNING, "basicpbr/PBRShader", { "UE_SKINNING" });
	addRenderShader(SHADER_FRAMEBUFFER,                 "FramebufferShader");
	addRenderShader(SHADER_SHADOW_MAP,                  "lighting/ShadowMapShader");
	addRenderShader(SHADER_SHADOW_MAP_SKINNING,         "lighting/ShadowMapShader", { "UE_SKINNING" });
	addRenderShader(SHADER_SHADOW_CUBEMAP,              "lighting/ShadowCubemapShader");
	addRenderShader(SHADER_SHADOW_CUBEMAP_SKINNING,     "lighting/ShadowCubemapShader", { "UE_SKINNING" });

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
	skyboxDepthState.depthTestEnable = false;
	skyboxDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS;
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
	lightBlendDepthState.depthCompareOp   = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	lightBlendDepthState.depthWriteEnable = true;

	//Obtain the window width and height
	uint32_t windowWidth = Window::getCurrentInstance()->getSettings().windowWidth;
	uint32_t windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;

	//Setup the default pipelines
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_MATERIAL,                          new GraphicsPipelineLayout(getRenderShader(SHADER_MATERIAL), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, defaultDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SKY_BOX,                           new GraphicsPipelineLayout(getRenderShader(SHADER_SKY_BOX), MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::Flag::NONE), defaultBlendState, skyboxDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_FONT,                              new GraphicsPipelineLayout(getRenderShader(SHADER_FONT), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, fontDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_FONT_SDF,                          new GraphicsPipelineLayout(getRenderShader(SHADER_FONT_SDF), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, fontDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING,                          new GraphicsPipelineLayout(getRenderShader(SHADER_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_BLEND,                    new GraphicsPipelineLayout(getRenderShader(SHADER_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_SKINNING,                 new GraphicsPipelineLayout(getRenderShader(SHADER_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND,           new GraphicsPipelineLayout(getRenderShader(SHADER_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING,                new GraphicsPipelineLayout(getRenderShader(SHADER_BASIC_PBR_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_BLEND,          new GraphicsPipelineLayout(getRenderShader(SHADER_BASIC_PBR_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING,       new GraphicsPipelineLayout(getRenderShader(SHADER_BASIC_PBR_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING_BLEND, new GraphicsPipelineLayout(getRenderShader(SHADER_BASIC_PBR_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState, windowWidth, windowHeight, true));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_MAP,                        new GraphicsPipelineLayout(getRenderShader(SHADER_SHADOW_MAP), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING,               new GraphicsPipelineLayout(getRenderShader(SHADER_SHADOW_MAP_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_CUBEMAP,                    new GraphicsPipelineLayout(getRenderShader(SHADER_SHADOW_CUBEMAP), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_SHADOW_CUBEMAP_SKINNING,           new GraphicsPipelineLayout(getRenderShader(SHADER_SHADOW_CUBEMAP_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState, Light::SHADOW_MAP_SIZE, Light::SHADOW_MAP_SIZE, false));
	addGraphicsPipelineLayout(GRAPHICS_PIPELINE_GUI,                               new GraphicsPipelineLayout(getRenderShader(SHADER_MATERIAL), MeshData::computeVertexInputData(2, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, defaultDepthState, windowWidth, windowHeight, true));

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
				meshRenderData->render();
				if (mesh->hasMaterial())
					stopUsingMaterial(mesh->getMaterial());
			} else
				meshRenderData->render();
		}
	}
}

void Renderer::destroy() {
	delete defaultRenderPass;
	delete shaderInterface;
	for (auto element : loadedRenderShaders)
		delete element.second;
	for (auto element : graphicsPipelineLayouts)
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

void Renderer::addGraphicsPipelineLayout(unsigned int id, GraphicsPipelineLayout* pipeline) {
	graphicsPipelineLayouts.insert(std::pair<unsigned int, GraphicsPipelineLayout*>(id, pipeline));
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
	if (graphicsPipelineLayouts.count(id) > 0)
		return graphicsPipelineLayouts.at(id);
	else {
		Logger::log("The GraphicsPipelineLayout with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}
