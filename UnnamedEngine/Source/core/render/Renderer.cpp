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

/*****************************************************************************
 * The Renderer class
 *****************************************************************************/

ShaderInterface* Renderer::shaderInterface;

ShaderBlock_Core Renderer::shaderCoreData;
UBO* Renderer::shaderCoreUBO;

ShaderBlock_Material Renderer::shaderMaterialData;
UBO* Renderer::shaderMaterialUBO;

ShaderBlock_Skinning Renderer::shaderSkinningData;
UBO* Renderer::shaderSkinningUBO;

std::vector<Camera*> Renderer::cameras;
std::vector<Texture*> Renderer::boundTextures;
std::unordered_map<std::string, RenderShader*> Renderer::renderShaders;
Texture* Renderer::blank;

MeshRenderData* Renderer::screenTextureMesh;

std::vector<unsigned int> Renderer::boundTexturesOldSize;

const std::string Renderer::SHADER_MATERIAL          = "Material";
const std::string Renderer::SHADER_SKY_BOX           = "SkyBox";
const std::string Renderer::SHADER_FONT              = "Font";
const std::string Renderer::SHADER_BILLBOARD         = "Billboard";
const std::string Renderer::SHADER_PARTICLE          = "Particle";
const std::string Renderer::SHADER_LIGHTING          = "Lighting";
const std::string Renderer::SHADER_FRAMEBUFFER       = "Framebuffer";
const std::string Renderer::SHADER_ENVIRONMENT_MAP   = "EnvironmentMap";
const std::string Renderer::SHADER_SHADOW_MAP        = "ShadowMap";
const std::string Renderer::SHADER_SHADOW_CUBEMAP    = "ShadowCubemap";
const std::string Renderer::SHADER_BILLBOARDED_FONT  = "BillboardedFont";
const std::string Renderer::SHADER_TERRAIN           = "Terrain";
const std::string Renderer::SHADER_PLAIN_TEXTURE     = "PlainTexture";
const std::string Renderer::SHADER_DEFERRED_LIGHTING = "DeferredLighting";
const std::string Renderer::SHADER_TILEMAP			 = "Tilemap";

const std::string Renderer::SHADER_PBR_EQUI_TO_CUBE_GEN         = "PBREquiToCubeGen";
const std::string Renderer::SHADER_PBR_IRRADIANCE_MAP_GEN       = "PBRIrradianceMapGen";
const std::string Renderer::SHADER_PBR_PREFILTER_MAP_GEN        = "PBRPrefilterMapGen";
const std::string Renderer::SHADER_PBR_BRDF_INTEGRATION_MAP_GEN = "PBRBRDFIntegrationMapGen";
const std::string Renderer::SHADER_PBR_LIGHTING                 = "PBRLighting";
const std::string Renderer::SHADER_PBR_DEFERRED_LIGHTING        = "PBRDeferredLighting";

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
		return loc;
	} else {
		glActiveTexture(GL_TEXTURE0 + boundTextures.size());
		texture->bind();
		boundTextures.push_back(texture);
		return boundTextures.size() - 1;
	}
}

void Renderer::unbindTexture() {
	glActiveTexture(GL_TEXTURE0 + boundTextures.size() - 1);
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
	glewInit();

	blank = Texture::loadTexture("resources/textures/blank.png");

	//Setup the UBOs
	shaderInterface = new ShaderInterface();
	shaderCoreUBO = shaderInterface->getUBO(ShaderInterface::BLOCK_CORE);
	shaderMaterialUBO = shaderInterface->getUBO(ShaderInterface::BLOCK_MATERIAL);
	shaderSkinningUBO = shaderInterface->getUBO(ShaderInterface::BLOCK_SKINNING);

	//Setup the shaders
	addRenderShader(SHADER_MATERIAL,              loadEngineShader("MaterialShader"),                   NULL);
	addRenderShader(SHADER_SKY_BOX,               loadEngineShader("SkyBoxShader"),                     NULL);
	addRenderShader(SHADER_FONT,                  loadEngineShader("FontShader"),                       NULL);
	addRenderShader(SHADER_BILLBOARD,             loadEngineShader("billboard/BillboardShader"),        NULL);
	addRenderShader(SHADER_PARTICLE,              loadEngineShader("ParticleShader"),                   NULL);
	addRenderShader(SHADER_LIGHTING,              loadEngineShader("lighting/LightingShader"),          loadEngineShader("lighting/LightingDeferredGeom"));
	addRenderShader(SHADER_FRAMEBUFFER,           loadEngineShader("FramebufferShader"),                NULL);
	addRenderShader(SHADER_ENVIRONMENT_MAP,       loadEngineShader("EnvironmentMapShader"),             NULL);
	addRenderShader(SHADER_SHADOW_MAP,            loadEngineShader("lighting/ShadowMapShader"),         NULL);
	addRenderShader(SHADER_SHADOW_CUBEMAP,        loadEngineShader("lighting/ShadowCubemapShader"),     NULL);
	addRenderShader(SHADER_BILLBOARDED_FONT,      loadEngineShader("billboard/BillboardedFontShader"),  NULL);
	addRenderShader(SHADER_TERRAIN,               loadEngineShader("terrain/Terrain"),                  loadEngineShader("terrain/TerrainDeferredGeom"));
	addRenderShader(SHADER_PLAIN_TEXTURE,         loadEngineShader("PlainTexture"),                     NULL);
	addRenderShader(SHADER_DEFERRED_LIGHTING,     loadEngineShader("lighting/DeferredLighting"),        NULL);
	addRenderShader(SHADER_TILEMAP,				  loadEngineShader("TilemapShader"),				    NULL);
	addRenderShader(SHADER_PBR_EQUI_TO_CUBE_GEN,         loadEngineShader("pbr/EquiToCubeGen"),         NULL);
	addRenderShader(SHADER_PBR_IRRADIANCE_MAP_GEN,       loadEngineShader("pbr/IrradianceMapGen"),      NULL);
	addRenderShader(SHADER_PBR_PREFILTER_MAP_GEN,        loadEngineShader("pbr/PrefilterMapGen"),       NULL);
	addRenderShader(SHADER_PBR_BRDF_INTEGRATION_MAP_GEN, loadEngineShader("pbr/BRDFIntegrationMapGen"), NULL);
	addRenderShader(SHADER_PBR_LIGHTING,          loadEngineShader("pbr/PBRShader"),                    loadEngineShader("pbr/PBRDeferredGeom"));
	addRenderShader(SHADER_PBR_DEFERRED_LIGHTING, loadEngineShader("pbr/PBRDeferredLighting"),          NULL);

	//Setup the screen texture mesh
	MeshData* meshData = new MeshData(MeshData::DIMENSIONS_2D);
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(-1.0f, -1.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, 1.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
	screenTextureMesh = new MeshRenderData(meshData, getRenderShader(SHADER_FRAMEBUFFER));
}

void Renderer::assignMatTexture(Shader* shader, std::string type, Texture* texture) {
	type += "Texture";
	if (texture)
		//Bind the texture
		shader->setUniformi("Material_" + type, bindTexture(texture));
}

void Renderer::setMaterialUniforms(Shader* shader, std::string shaderName, Material* material) {
	assignMatTexture(shader, "Diffuse", material->getDiffuseTexture());

	//Check to see whether the shader is for lighting
	if (shaderName == SHADER_LIGHTING || shaderName == SHADER_TERRAIN || shaderName == SHADER_PBR_LIGHTING) {
		//Assign other lighting specific properties
		assignMatTexture(shader, "Ambient", material->getAmbientTexture());
		assignMatTexture(shader, "Specular", material->getSpecularTexture());
		assignMatTexture(shader, "Shininess", material->getShininessTexture());

		if (material->getNormalMap())
			shader->setUniformi("Material_NormalMap", bindTexture(material->getNormalMap()));
		if (material->getParallaxMap())
			shader->setUniformi("Material_ParallaxMap", bindTexture(material->getParallaxMap()));
	}

	//Update the material UBO
	shaderMaterialUBO->update(&material->getShaderData(), 0, sizeof(ShaderBlock_Material));
}

void Renderer::render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* renderShader) {
	//Ensure there is a Shader and Camera instance for rendering
	if (renderShader && getCamera()) {
		//Get the shader for rendering
		Shader* shader = renderShader->getShader();

//		//Get the map of uniforms within the Shader
//		std::map<std::string, GLint>& uniforms = shader->getUniforms();
//
//		//Go through each uniform in the map
//		for (auto& iterator : uniforms) {
//			//Check the uniform name
//			if (iterator.first == "MVPMatrix")
//				shader->setUniformMatrix4("MVPMatrix", (getCamera()->getProjectionViewMatrix() * modelMatrix));
//		}
		shaderCoreData.ue_mvpMatrix = (getCamera()->getProjectionViewMatrix() * modelMatrix);
		shaderCoreUBO->update(&shaderCoreData, 0, sizeof(ShaderBlock_Core));
		if (mesh->hasData() && mesh->hasRenderData()) {
			MeshData* data = mesh->getData();
			MeshRenderData* renderData = mesh->getRenderData();

			if (mesh->hasSkeleton()) {
				for (unsigned int i = 0; i < mesh->getSkeleton()->getNumBones(); i++)
					shaderSkinningData.ue_bones[i] = mesh->getSkeleton()->getBone(i)->getFinalTransform();
				shaderSkinningData.ue_useSkinning = true;
				shaderSkinningUBO->update(&shaderSkinningData, 0, sizeof(ShaderBlock_Skinning));
			} else {
				shaderSkinningData.ue_useSkinning = false;
				shaderSkinningData.updateUseSkinning(shaderSkinningUBO);
			}

			if (data->hasSubData()) {
				renderData->getRenderData()->bindVAO();

				//Go through each sub data instance
				for (unsigned int i = 0; i < data->getSubDataCount(); i++) {
					saveTextures();

					if (mesh->hasMaterial())
						setMaterialUniforms(shader, renderShader->getName(), mesh->getMaterial(data->getSubData(i).materialIndex));
					renderData->getRenderData()->renderBaseVertex(data->getSubData(i).count, data->getSubData(i).baseIndex * sizeof(unsigned int), data->getSubData(i).baseVertex);

					releaseNewTextures();
				}

				renderData->getRenderData()->unbindVAO();
			} else {
				saveTextures();
				if (mesh->hasMaterial())
					setMaterialUniforms(renderShader->getShader(), renderShader->getName(), mesh->getMaterial());
				renderData->render();
				releaseNewTextures();
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
	delete screenTextureMesh;
}

using namespace utils_string;

Shader* Renderer::loadEngineShader(std::string path) {
	return Shader::loadShader("resources/shaders/" + path);
}

void Renderer::prepareForwardShader(std::string id, Shader* shader) {
	shader->use();
	if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_DEFERRED_LIGHTING || id == SHADER_PBR_LIGHTING || id == SHADER_PBR_DEFERRED_LIGHTING) {
		shader->addUniform("ShadowMap", "ue_shadowMap");

		for (unsigned int i = 0; i < RenderScene3D::NUM_LIGHTS_IN_SET; i++) {
			shader->addUniform("Light_ShadowMap["      + str(i) + "]", "ue_lightsTextures[" + str(i) + "].shadowMap");
			shader->addUniform("Light_ShadowCubemap["  + str(i) + "]", "ue_lightsTextures[" + str(i) + "].shadowCubemap");
		}

		shader->addUniform("EnvironmentMap", "ue_environmentMap");
		shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
	}

	shader->stopUsing();
}

void Renderer::prepareDeferredGeomShader(std::string id, Shader* shader) {
	shader->use();
	if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_PBR_LIGHTING) {
		shader->addUniform("ShadowMap", "ue_shadowMap");

		shader->addUniform("EnvironmentMap", "ue_environmentMap");
		shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
	}

	shader->stopUsing();
}

void Renderer::addRenderShader(std::string id, Shader* forwardShader, Shader* deferredGeomShader) {
	//Setup the shader
	if (forwardShader)
		prepareForwardShader(id, forwardShader);
	if (deferredGeomShader)
		prepareDeferredGeomShader(id, deferredGeomShader);

	//Add the shader
	addRenderShader(new RenderShader(id, forwardShader, deferredGeomShader));
}

void Renderer::addRenderShader(RenderShader* renderShader) {
	renderShaders.insert(std::pair<std::string, RenderShader*>(renderShader->getName(), renderShader));
}

RenderShader* Renderer::getRenderShader(std::string id) {
	if (renderShaders.count(id) > 0)
		return renderShaders.at(id);
	else {
		Logger::log("The RenderShader with the id '" + id + "' could not be found", "Renderer", LogType::Error);
		return NULL;
	}
}
