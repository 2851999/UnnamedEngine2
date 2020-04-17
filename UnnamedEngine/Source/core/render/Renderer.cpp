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
Texture* Renderer::blank;

std::vector<unsigned int> Renderer::boundTexturesOldSize;

GraphicsState* Renderer::currentGraphicsState = nullptr;
bool Renderer::shouldIgnoreGraphicsStates = false;

const unsigned int Renderer::SHADER_MATERIAL          = 1;
const unsigned int Renderer::SHADER_SKY_BOX           = 2;
const unsigned int Renderer::SHADER_FONT              = 3;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING   = 4;

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
	addRenderShader(SHADER_MATERIAL,        "MaterialShader");
	addRenderShader(SHADER_SKY_BOX,         "SkyBoxShader");
	addRenderShader(SHADER_VULKAN_LIGHTING, "VulkanLightingShader");
	addRenderShader(SHADER_FONT,            "FontShader");
}

void Renderer::useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material) {
	//Bind the material descriptor set
	material->getDescriptorSet()->bind();
}

void Renderer::stopUsingMaterial(Material* material) {
	if (! shouldIgnoreGraphicsStates)
		//Unbind the textures
		material->getDescriptorSet()->unbind();
}

void Renderer::useGraphicsState(GraphicsState* graphicsState) {
	//Ensure using OpenGL
	if (! BaseEngine::usingVulkan() && ! shouldIgnoreGraphicsStates) {
		//Apply the new state
		graphicsState->applyGL(currentGraphicsState);
		currentGraphicsState = graphicsState;
	}
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

		//Use the correct graphics state
		useGraphicsState(renderShader->getGraphicsState());

		renderData->getShaderBlock_Model().ue_mvpMatrix = (getCamera()->getProjectionViewMatrix() * modelMatrix);

		shaderModelUBO->updateFrame(&renderData->getShaderBlock_Model(), 0, sizeof(ShaderBlock_Model));

		renderData->getDescriptorSet()->bind();

		if (mesh->hasData() && mesh->hasRenderData()) {
			MeshData* data = mesh->getData();
			MeshRenderData* meshRenderData = mesh->getRenderData();

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
	//Assign the graphics state for the render shader
	assignGraphicsState(renderShader->getGraphicsState(), id);
	//Setup
	renderShader->setup();

	//Add the shader
	addRenderShader(renderShader);
}

void Renderer::assignGraphicsState(GraphicsState* state, unsigned int shaderID) {
	//Check the shader ID
	if (shaderID == SHADER_SKY_BOX)
		//Assign the state to use
		state->depthWriteEnable = true; //???? Broken if not used
	else if (shaderID == SHADER_FONT)
		state->alphaBlending = true;
	else if (shaderID == SHADER_MATERIAL)
		state->alphaBlending = true;
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
