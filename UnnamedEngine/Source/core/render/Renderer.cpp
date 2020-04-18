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

ShaderBlock_Material Renderer::shaderMaterialData;
ShaderBlock_Skinning Renderer::shaderSkinningData;

std::vector<Camera*> Renderer::cameras;
std::unordered_map<unsigned int, std::string> Renderer::renderShaderPaths;
std::unordered_map<unsigned int, RenderShader*> Renderer::loadedRenderShaders;
std::unordered_map<unsigned int, GraphicsPipeline*> Renderer::graphicsPipelines;
Texture* Renderer::blank;

GraphicsPipeline* Renderer::currentGraphicsPipeline = NULL;

const unsigned int Renderer::SHADER_MATERIAL                  = 1;
const unsigned int Renderer::SHADER_SKY_BOX                   = 2;
const unsigned int Renderer::SHADER_FONT                      = 3;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING           = 4;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING_SKINNING  = 5;

const unsigned int Renderer::GRAPHICS_PIPELINE_MATERIAL                = 1;
const unsigned int Renderer::GRAPHICS_PIPELINE_SKY_BOX                 = 2;
const unsigned int Renderer::GRAPHICS_PIPELINE_FONT                    = 3;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING                = 4;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_BLEND          = 5;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING       = 6;
const unsigned int Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND = 7;

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

	//Setup the shaders
	addRenderShader(SHADER_MATERIAL,                 "MaterialShader");
	addRenderShader(SHADER_SKY_BOX,                  "SkyBoxShader");
	addRenderShader(SHADER_VULKAN_LIGHTING,          "VulkanLightingShader");
	addRenderShader(SHADER_FONT,                     "FontShader");
	addRenderShader(SHADER_VULKAN_LIGHTING_SKINNING, "VulkanLightingSkinningShader");

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

	//Depth state for lighting
	GraphicsPipeline::DepthState lightDepthState;
	lightDepthState.depthCompareOp = GraphicsPipeline::CompareOperation::LESS;

	//Depth state for light blending
	GraphicsPipeline::DepthState lightBlendDepthState;
	lightBlendDepthState.depthCompareOp   = GraphicsPipeline::CompareOperation::LESS_OR_EQUAL;
	lightBlendDepthState.depthWriteEnable = false;

	//Setup the default pipelines
	addGraphicsPipeline(GRAPHICS_PIPELINE_MATERIAL,                new GraphicsPipeline(getRenderShader(SHADER_MATERIAL), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, defaultDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_SKY_BOX,                 new GraphicsPipeline(getRenderShader(SHADER_SKY_BOX), MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::Flag::NONE), defaultBlendState, defaultDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_FONT,                    new GraphicsPipeline(getRenderShader(SHADER_FONT), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, defaultDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_LIGHTING,                new GraphicsPipeline(getRenderShader(SHADER_VULKAN_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_LIGHTING_BLEND,          new GraphicsPipeline(getRenderShader(SHADER_VULKAN_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_LIGHTING_SKINNING,       new GraphicsPipeline(getRenderShader(SHADER_VULKAN_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState));
	addGraphicsPipeline(GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND, new GraphicsPipeline(getRenderShader(SHADER_VULKAN_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState));
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
		//Get the render data for rendering
		RenderData* renderData = mesh->getRenderData()->getRenderData();

		//Bind the camera descriptor set
		getCamera()->getDescriptorSet()->bind();

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

		if (mesh->hasData() && mesh->hasRenderData()) {
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

			if (data->hasSubData()) {
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
			} else {
				if (mesh->hasMaterial())
					useMaterial(renderData, 0, mesh->getMaterial());
				meshRenderData->render();
				if (mesh->hasMaterial())
					stopUsingMaterial(mesh->getMaterial());
			}
		}
	}
}

void Renderer::destroy() {
	delete shaderInterface;
	for (auto element : loadedRenderShaders)
		delete element.second;
	for (auto element : graphicsPipelines)
		delete element.second;
}

using namespace utils_string;

Shader* Renderer::loadEngineShader(std::string path) {
	if (!BaseEngine::usingVulkan())
		return Shader::loadShader("resources/shaders/" + path);
	else
		return Shader::loadShader("resources/shaders-vulkan/" + path);
}

void Renderer::addRenderShader(unsigned int id, std::string forwardShaderPath) {
	renderShaderPaths.insert(std::pair<unsigned int, std::string>(id, forwardShaderPath));
}

void Renderer::addGraphicsPipeline(unsigned int id, GraphicsPipeline* pipeline) {
	graphicsPipelines.insert(std::pair<unsigned int, GraphicsPipeline*>(id, pipeline));
}

void Renderer::setCurrentGraphicsPipeline(GraphicsPipeline* pipeline) {
	currentGraphicsPipeline = pipeline;
}

void Renderer::loadRenderShader(unsigned int id) {
	//Get the paths
	std::string shaderPath = renderShaderPaths.at(id);
	Shader* forwardShader = NULL;

	//Load the shaders if the path has been assigned
	//Setup the shader
	if (shaderPath != "")
		forwardShader = loadEngineShader(shaderPath);

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

RenderShader* Renderer::getRenderShader(unsigned int id) {
	if (loadedRenderShaders.count(id) > 0)
		return loadedRenderShaders.at(id);
	else if (renderShaderPaths.count(id) > 0) {
		//Load the render shader then return it
		loadRenderShader(id);
		return loadedRenderShaders.at(id);
	} else {
		Logger::log("The RenderShader with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}

GraphicsPipeline* Renderer::getGraphicsPipeline(unsigned int id) {
	if (graphicsPipelines.count(id) > 0)
		return graphicsPipelines.at(id);
	else {
		Logger::log("The RenderPipeline with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}
