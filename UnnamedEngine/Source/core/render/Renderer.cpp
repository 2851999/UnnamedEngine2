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
std::vector<Texture*> Renderer::boundTextures;
std::unordered_map<unsigned int, std::string> Renderer::renderShaderPaths;
std::unordered_map<unsigned int, RenderShader*> Renderer::loadedRenderShaders;
std::unordered_map<unsigned int, RenderPipeline*> Renderer::renderPipelines;
Texture* Renderer::blank;

std::vector<unsigned int> Renderer::boundTexturesOldSize;

const unsigned int Renderer::SHADER_MATERIAL                  = 1;
const unsigned int Renderer::SHADER_SKY_BOX                   = 2;
const unsigned int Renderer::SHADER_FONT                      = 3;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING           = 4;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING_SKINNING  = 5;

const unsigned int Renderer::PIPELINE_MATERIAL                = 1;
const unsigned int Renderer::PIPELINE_SKY_BOX                 = 2;
const unsigned int Renderer::PIPELINE_FONT                    = 3;
const unsigned int Renderer::PIPELINE_LIGHTING                = 4;
const unsigned int Renderer::PIPELINE_LIGHTING_BLEND          = 5;
const unsigned int Renderer::PIPELINE_LIGHTING_SKINNING       = 6;
const unsigned int Renderer::PIPELINE_LIGHTING_SKINNING_BLEND = 7;

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

GLuint Renderer::bindTexture(Texture* texture) {
	//Try and locate it if it has already been bound
	unsigned int loc = std::find(boundTextures.begin(), boundTextures.end(), texture) - boundTextures.begin();
	//Check whether it has already been bound
	if (loc < boundTextures.size()) {
		boundTextures.push_back(texture);
		//It has so return the correct active texture
		return loc + 10;
	} else {
		glActiveTexture(GL_TEXTURE10 + boundTextures.size());
		texture->bind();
		boundTextures.push_back(texture);
		return boundTextures.size() + 10 - 1;
	}
}

void Renderer::unbindTexture() {
	glActiveTexture(GL_TEXTURE10 + boundTextures.size() - 1);
	boundTextures[boundTextures.size() - 1]->unbind();
	boundTextures.pop_back();
}

void Renderer::saveTextures() {
	boundTexturesOldSize.push_back(boundTextures.size());
}

void Renderer::releaseNewTextures() {
	//Get the previous size
	unsigned int previousSize = boundTexturesOldSize[boundTexturesOldSize.size() - 1];
	boundTexturesOldSize.pop_back();

	while (boundTextures.size() > previousSize)
		unbindTexture();
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
	RenderPipeline::ColourBlendState defaultBlendState;

	//Colour blend state for transparency
	RenderPipeline::ColourBlendState alphaBlendState;
	alphaBlendState.blendEnabled = true;
	alphaBlendState.srcRGB = RenderPipeline::BlendFactor::SRC_ALPHA;
	alphaBlendState.dstRGB = RenderPipeline::BlendFactor::ONE_MINUS_SRC_ALPHA;
	alphaBlendState.srcAlpha = RenderPipeline::BlendFactor::ONE;
	alphaBlendState.dstAlpha = RenderPipeline::BlendFactor::ZERO;

	//Colour blend state for blending lighting
	RenderPipeline::ColourBlendState alphaLightBlendState;
	alphaLightBlendState.blendEnabled = true;
	alphaLightBlendState.srcRGB = RenderPipeline::BlendFactor::ONE;
	alphaLightBlendState.dstRGB = RenderPipeline::BlendFactor::ONE;
	alphaLightBlendState.srcAlpha = RenderPipeline::BlendFactor::ONE;
	alphaLightBlendState.dstAlpha = RenderPipeline::BlendFactor::ZERO;

	//Depth state for normal depth testing
	RenderPipeline::DepthState defaultDepthState;

	//Depth state for lighting
	RenderPipeline::DepthState lightDepthState;
	lightDepthState.depthCompareOp = RenderPipeline::CompareOperation::LESS;

	//Depth state for light blending
	RenderPipeline::DepthState lightBlendDepthState;
	lightBlendDepthState.depthCompareOp   = RenderPipeline::CompareOperation::LESS_OR_EQUAL;
	lightBlendDepthState.depthWriteEnable = false;

	//Setup the default pipelines
	addPipeline(PIPELINE_MATERIAL,                new RenderPipeline(getRenderShader(SHADER_MATERIAL), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, defaultDepthState));
	addPipeline(PIPELINE_SKY_BOX,                 new RenderPipeline(getRenderShader(SHADER_SKY_BOX), MeshData::computeVertexInputData(3, { MeshData::POSITION }, MeshData::Flag::NONE), defaultBlendState, defaultDepthState));
	addPipeline(PIPELINE_FONT,                    new RenderPipeline(getRenderShader(SHADER_FONT), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD }, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS), alphaBlendState, defaultDepthState));
	addPipeline(PIPELINE_LIGHTING,                new RenderPipeline(getRenderShader(SHADER_VULKAN_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaBlendState, lightDepthState));
	addPipeline(PIPELINE_LIGHTING_BLEND,          new RenderPipeline(getRenderShader(SHADER_VULKAN_LIGHTING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState));
	addPipeline(PIPELINE_LIGHTING_SKINNING,       new RenderPipeline(getRenderShader(SHADER_VULKAN_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaBlendState, lightDepthState));
	addPipeline(PIPELINE_LIGHTING_SKINNING_BLEND, new RenderPipeline(getRenderShader(SHADER_VULKAN_LIGHTING_SKINNING), MeshData::computeVertexInputData(3, { MeshData::POSITION, MeshData::TEXTURE_COORD, MeshData::NORMAL, MeshData::TANGENT, MeshData::BITANGENT, MeshData::BONE_ID, MeshData::BONE_WEIGHT }, MeshData::NONE), alphaLightBlendState, lightBlendDepthState));
}

void Renderer::useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material) {
	//Bind the material descriptor set
	material->getDescriptorSet()->bind();
}

void Renderer::stopUsingMaterial(Material* material) {
	//Unbind the textures
	material->getDescriptorSet()->unbind();
}

void Renderer::preRender() {
	//Update required data for this frame

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
	for (auto element : renderPipelines)
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

void Renderer::addPipeline(unsigned int id, RenderPipeline* pipeline) {
	renderPipelines.insert(std::pair<unsigned int, RenderPipeline*>(id, pipeline));
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

RenderPipeline* Renderer::getPipeline(unsigned int id) {
	if (renderPipelines.count(id) > 0)
		return renderPipelines.at(id);
	else {
		Logger::log("The RenderPipeline with the id '" + utils_string::str(id) + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}
