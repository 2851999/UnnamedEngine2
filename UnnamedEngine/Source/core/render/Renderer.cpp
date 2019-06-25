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

#include "../../utils/Logging.h"
#include "RenderScene.h"
#include "../vulkan/Vulkan.h"

/*****************************************************************************
 * The Renderer class
 *****************************************************************************/

ShaderInterface* Renderer::shaderInterface;

ShaderBlock_Core     Renderer::shaderCoreData;
ShaderBlock_Material Renderer::shaderMaterialData;
ShaderBlock_Skinning Renderer::shaderSkinningData;

std::vector<Camera*> Renderer::cameras;
std::vector<Texture*> Renderer::boundTextures;
std::unordered_map<unsigned int, std::vector<std::string>> Renderer::renderShaderPaths;
std::unordered_map<unsigned int, RenderShader*> Renderer::loadedRenderShaders;
Texture* Renderer::blank;

MeshRenderData* Renderer::screenTextureMesh;

std::vector<unsigned int> Renderer::boundTexturesOldSize;

GraphicsState* Renderer::currentGraphicsState = nullptr;

const unsigned int Renderer::SHADER_MATERIAL          = 1;
const unsigned int Renderer::SHADER_SKY_BOX           = 2;
const unsigned int Renderer::SHADER_FONT              = 3;
const unsigned int Renderer::SHADER_BILLBOARD         = 4;
const unsigned int Renderer::SHADER_PARTICLE          = 5;
const unsigned int Renderer::SHADER_LIGHTING          = 6;
const unsigned int Renderer::SHADER_FRAMEBUFFER       = 7;
const unsigned int Renderer::SHADER_ENVIRONMENT_MAP   = 8;
const unsigned int Renderer::SHADER_SHADOW_MAP        = 9;
const unsigned int Renderer::SHADER_SHADOW_CUBEMAP    = 10;
const unsigned int Renderer::SHADER_BILLBOARDED_FONT  = 11;
const unsigned int Renderer::SHADER_TERRAIN           = 12;
const unsigned int Renderer::SHADER_PLAIN_TEXTURE     = 13;
const unsigned int Renderer::SHADER_DEFERRED_LIGHTING = 14;
const unsigned int Renderer::SHADER_TILEMAP			  = 15;
const unsigned int Renderer::SHADER_VULKAN			  = 16;
const unsigned int Renderer::SHADER_VULKAN_LIGHTING   = 17;

const unsigned int Renderer::SHADER_PBR_EQUI_TO_CUBE_GEN         = 18;
const unsigned int Renderer::SHADER_PBR_IRRADIANCE_MAP_GEN       = 19;
const unsigned int Renderer::SHADER_PBR_PREFILTER_MAP_GEN        = 20;
const unsigned int Renderer::SHADER_PBR_BRDF_INTEGRATION_MAP_GEN = 21;
const unsigned int Renderer::SHADER_PBR_LIGHTING                 = 22;
const unsigned int Renderer::SHADER_PBR_DEFERRED_LIGHTING        = 23;

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
	addRenderShader(SHADER_MATERIAL,                     "MaterialShader",                   "");
	addRenderShader(SHADER_SKY_BOX,                      "SkyBoxShader",                     "");
	addRenderShader(SHADER_VULKAN,				         "VulkanShader",				     "");
	addRenderShader(SHADER_LIGHTING,                     "lighting/LightingShader",          "lighting/LightingDeferredGeom");
	addRenderShader(SHADER_VULKAN_LIGHTING,              "VulkanLightingShader",             "");

	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		addRenderShader(SHADER_FONT,                         "FontShader",                       "");
		addRenderShader(SHADER_BILLBOARD,                    "billboard/BillboardShader",        "");
		addRenderShader(SHADER_PARTICLE,                     "ParticleShader",                   "");
		addRenderShader(SHADER_FRAMEBUFFER,                  "FramebufferShader",                "");
		addRenderShader(SHADER_ENVIRONMENT_MAP,              "EnvironmentMapShader",             "");
		addRenderShader(SHADER_SHADOW_MAP,                   "lighting/ShadowMapShader",         "");
		addRenderShader(SHADER_SHADOW_CUBEMAP,               "lighting/ShadowCubemapShader",     "");
		addRenderShader(SHADER_BILLBOARDED_FONT,             "billboard/BillboardedFontShader",  "");
		addRenderShader(SHADER_TERRAIN,                      "terrain/Terrain",                  "terrain/TerrainDeferredGeom");
		addRenderShader(SHADER_PLAIN_TEXTURE,                "PlainTexture",                     "");
		addRenderShader(SHADER_DEFERRED_LIGHTING,            "lighting/DeferredLighting",        "");
		addRenderShader(SHADER_TILEMAP,				         "TilemapShader",				     "");
		addRenderShader(SHADER_PBR_EQUI_TO_CUBE_GEN,         "pbr/EquiToCubeGen",                "");
		addRenderShader(SHADER_PBR_IRRADIANCE_MAP_GEN,       "pbr/IrradianceMapGen",             "");
		addRenderShader(SHADER_PBR_PREFILTER_MAP_GEN,        "pbr/PrefilterMapGen",              "");
		addRenderShader(SHADER_PBR_BRDF_INTEGRATION_MAP_GEN, "pbr/BRDFIntegrationMapGen",        "");
		addRenderShader(SHADER_PBR_LIGHTING,                 "pbr/PBRShader",                    "pbr/PBRDeferredGeom");
		addRenderShader(SHADER_PBR_DEFERRED_LIGHTING,        "pbr/PBRDeferredLighting",          "");

		//Setup the screen texture mesh
		MeshData* meshData = new MeshData(MeshData::DIMENSIONS_2D);
		meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
		meshData->addPosition(Vector2f(-1.0f, -1.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
		meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
		meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
		meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
		meshData->addPosition(Vector2f(1.0f, 1.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
		screenTextureMesh = new MeshRenderData(meshData, getRenderShader(SHADER_FRAMEBUFFER));
		std::vector<Material*> materials;
		screenTextureMesh->setup(meshData, materials);
	}
}

void Renderer::useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material, UBO* materialUBO) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		//Bind the required textures
		material->getTextureSet()->bindGLTextures();
	else
		//Bind the required descriptor set
		vkCmdBindDescriptorSets(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderData->getVkGraphicsPipeline()->getLayout(), 0, 1, renderData->getVkDescriptorSet(materialIndex), 0, nullptr);

	//Update the material UBO (if there is one)
	if (materialUBO)
		materialUBO->update(&material->getShaderData(), 0, sizeof(ShaderBlock_Material));
}

void Renderer::stopUsingMaterial(Material* material) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		//Unbind the textures
		material->getTextureSet()->unbindGLTextures();
}

void Renderer::useGraphicsState(GraphicsState* graphicsState) {
	//Ensure using OpenGL
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Apply the new state
		graphicsState->applyGL(currentGraphicsState);
		currentGraphicsState = graphicsState;
	}
}

void Renderer::render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* renderShader) {
	//Ensure there is a Shader and Camera instance for rendering
	if (renderShader && getCamera()) {
		//Get the render data for rendering
		RenderData* renderData = mesh->getRenderData()->getRenderData();

		//Obtain the required UBO's for rendering
		UBO* shaderCoreUBO     = renderData->getUBO(ShaderInterface::BLOCK_CORE);
		UBO* shaderSkinningUBO = renderData->getUBO(ShaderInterface::BLOCK_SKINNING);
		UBO* materialUBO       = renderData->getUBO(ShaderInterface::BLOCK_MATERIAL);

		//Use the correct graphics state
		useGraphicsState(renderShader->getGraphicsState());

		shaderCoreData.ue_mvpMatrix = (getCamera()->getProjectionViewMatrix() * modelMatrix);
		shaderCoreUBO->update(&shaderCoreData, 0, sizeof(ShaderBlock_Core));
		if (mesh->hasData() && mesh->hasRenderData()) {
			MeshData* data = mesh->getData();
			MeshRenderData* renderData = mesh->getRenderData();

			if (shaderSkinningUBO) {
				if (mesh->hasSkeleton()) {
					for (unsigned int i = 0; i < mesh->getSkeleton()->getNumBones(); ++i)
						shaderSkinningData.ue_bones[i] = mesh->getSkeleton()->getBone(i)->getFinalTransform();
					shaderSkinningData.ue_useSkinning = true;
					shaderSkinningUBO->update(&shaderSkinningData, 0, sizeof(ShaderBlock_Skinning));
				} else {
					shaderSkinningData.ue_useSkinning = false;
					shaderSkinningData.updateUseSkinning(shaderSkinningUBO);
				}
			}

			//Bind the pipeline to use to render (for Vulkan)
			if (Window::getCurrentInstance()->getSettings().videoVulkan)
				vkCmdBindPipeline(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderData->getRenderData()->getVkGraphicsPipeline()->getInstance());

			if (data->hasSubData()) {
				renderData->getRenderData()->bindBuffers();

				//Go through each sub data instance
				for (unsigned int i = 0; i < data->getSubDataCount(); ++i) {
					if (mesh->hasMaterial())
						useMaterial(renderData->getRenderData(), data->getSubData(i).materialIndex, mesh->getMaterial(data->getSubData(i).materialIndex), materialUBO);
					renderData->getRenderData()->renderBaseVertex(data->getSubData(i).count, data->getSubData(i).baseIndex, data->getSubData(i).baseVertex);
					if (mesh->hasMaterial())
						stopUsingMaterial(mesh->getMaterial(data->getSubData(i).materialIndex));
				}

				renderData->getRenderData()->unbindBuffers();
			} else {
				if (mesh->hasMaterial())
					useMaterial(renderData->getRenderData(), 0, mesh->getMaterial(), materialUBO);
				renderData->render();
				if (mesh->hasMaterial())
					stopUsingMaterial(mesh->getMaterial());
			}
		}
	}
}

void Renderer::render(FramebufferStore* texture, Shader* shader) {
	if (shader == NULL)
		shader = getRenderShader(SHADER_PLAIN_TEXTURE)->getShader();

	shader->use();

	shader->setUniformi("Texture", bindTexture(texture));

	screenTextureMesh->render();

	shader->stopUsing();

	unbindTexture();
}

void Renderer::destroy() {
	delete shaderInterface;
	for (auto element : loadedRenderShaders)
		delete element.second;
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		delete screenTextureMesh;
}

using namespace utils_string;

Shader* Renderer::loadEngineShader(std::string path) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		return Shader::loadShader("resources/shaders/" + path);
	else
		return Shader::loadShader("resources/shaders-vulkan/" + path);
}

void Renderer::prepareForwardShader(unsigned int id, Shader* shader) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		shader->use();
		if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_DEFERRED_LIGHTING || id == SHADER_PBR_LIGHTING || id == SHADER_PBR_DEFERRED_LIGHTING) {
			shader->addUniform("ShadowMap", "ue_shadowMap");

			for (unsigned int i = 0; i < RenderScene3D::NUM_LIGHTS_IN_SET; i++) {
				shader->addUniform("Light_ShadowMap["      + str(i) + "]", "ue_lightTexturesShadowMap[" + str(i) + "]");
				shader->addUniform("Light_ShadowCubemap["  + str(i) + "]", "ue_lightTexturesShadowCubemap[" + str(i) + "]");
			}

			shader->addUniform("EnvironmentMap", "ue_environmentMap");
			shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
		}

		shader->stopUsing();
	}
}

void Renderer::prepareDeferredGeomShader(unsigned int id, Shader* shader) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		shader->use();
		if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_PBR_LIGHTING) {
			shader->addUniform("ShadowMap", "ue_shadowMap");

			shader->addUniform("EnvironmentMap", "ue_environmentMap");
			shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
		}

		shader->stopUsing();
	}
}

void Renderer::addRenderShader(unsigned int id, std::string forwardShaderPath, std::string deferredGeomShaderPath) {
	renderShaderPaths.insert(std::pair<unsigned int, std::vector<std::string>>(id, { forwardShaderPath, deferredGeomShaderPath }));
}

void Renderer::loadRenderShader(unsigned int id) {
	//Get the paths
	std::vector<std::string> shaderPaths = renderShaderPaths.at(id);
	Shader* forwardShader = NULL;
	Shader* deferredGeomShader = NULL;

	//Load the shaders if the paths have been assigned
	//Setup the shader
	if (shaderPaths[0] != "") {
		forwardShader = loadEngineShader(shaderPaths[0]);
		prepareForwardShader(id, forwardShader);
	}
	if (shaderPaths[1] != "" && (! Window::getCurrentInstance()->getSettings().videoVulkan)) { //Don't load this if using Vulkan yet
		deferredGeomShader = loadEngineShader(shaderPaths[1]);
		prepareDeferredGeomShader(id, deferredGeomShader);
	}
	//Create the shader
	RenderShader* renderShader = new RenderShader(id, forwardShader, deferredGeomShader);
	//Assign the graphics state for the render shader
	assignGraphicsState(renderShader->getGraphicsState(), id);
	//Add the shader
	addRenderShader(renderShader);
}

void Renderer::assignGraphicsState(GraphicsState* state, unsigned int shaderID) {
	//Check the shader ID
	if (shaderID == SHADER_SKY_BOX) {
		//Assign the state to use
		state->depthWriteEnable = false;
	}
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
