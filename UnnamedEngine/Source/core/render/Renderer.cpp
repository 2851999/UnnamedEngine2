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
const std::string Renderer::SHADER_BILLBOARDED_FONT  = "BillboardedFont";
const std::string Renderer::SHADER_TERRAIN           = "Terrain";
const std::string Renderer::SHADER_PLAIN_TEXTURE     = "PlainTexture";
const std::string Renderer::SHADER_DEFERRED_LIGHTING = "DeferredLighting";

const std::string Renderer::SHADER_PBR_EQUI_TO_CUBE = "PBREquiToCube";
const std::string Renderer::SHADER_PBR_IRRADIANCE   = "PBRIrradiance";
const std::string Renderer::SHADER_PBR_PREFILTER    = "PBRPrefilter";
const std::string Renderer::SHADER_PBR_BRDF         = "PBRBRDF";
const std::string Renderer::SHADER_PBR_LIGHTING     = "PBRLighting";
const std::string Renderer::SHADER_PBR_DEFERRED_LIGHTING = "PBRDeferredLighting";

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

	//Setup the shaders
	addRenderShader(SHADER_MATERIAL,              loadEngineShader("MaterialShader"),                  NULL);
	addRenderShader(SHADER_SKY_BOX,               loadEngineShader("SkyBoxShader"),                    NULL);
	addRenderShader(SHADER_FONT,                  loadEngineShader("FontShader"),                      NULL);
	addRenderShader(SHADER_BILLBOARD,             loadEngineShader("billboard/BillboardShader"),       NULL);
	addRenderShader(SHADER_PARTICLE,              loadEngineShader("ParticleShader"),                  NULL);
	addRenderShader(SHADER_LIGHTING,              loadEngineShader("lighting/LightingShader"),        loadEngineShader("lighting/LightingDeferredGeom"));
	addRenderShader(SHADER_FRAMEBUFFER,           loadEngineShader("FramebufferShader"),               NULL);
	addRenderShader(SHADER_ENVIRONMENT_MAP,       loadEngineShader("EnvironmentMapShader"),            NULL);
	addRenderShader(SHADER_SHADOW_MAP,            loadEngineShader("lighting/ShadowMapShader"),        NULL);
	addRenderShader(SHADER_BILLBOARDED_FONT,      loadEngineShader("billboard/BillboardedFontShader"), NULL);
	addRenderShader(SHADER_TERRAIN,               loadEngineShader("terrain/Terrain"),                 loadEngineShader("terrain/TerrainDeferredGeom"));
	addRenderShader(SHADER_PLAIN_TEXTURE,         loadEngineShader("PlainTexture"),                    NULL);
	addRenderShader(SHADER_DEFERRED_LIGHTING,     loadEngineShader("lighting/DeferredLighting"),       NULL);
	addRenderShader(SHADER_PBR_EQUI_TO_CUBE,      loadEngineShader("pbr/EquiToCube"),                  NULL);
	addRenderShader(SHADER_PBR_IRRADIANCE,        loadEngineShader("pbr/Irradiance"),                  NULL);
	addRenderShader(SHADER_PBR_PREFILTER,         loadEngineShader("pbr/Prefilter"),                   NULL);
	addRenderShader(SHADER_PBR_BRDF,              loadEngineShader("pbr/BRDF"),                        NULL);
	addRenderShader(SHADER_PBR_LIGHTING,          loadEngineShader("pbr/PBRShader"),                   loadEngineShader("pbr/PBRDeferredGeom"));
	addRenderShader(SHADER_PBR_DEFERRED_LIGHTING, loadEngineShader("pbr/PBRDeferredLighting"),         NULL);


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
	shader->setUniformi("Material_Has" + type, texture != NULL);
}

void Renderer::setMaterialUniforms(Shader* shader, std::string shaderName, Material* material) {
	shader->setUniformColourRGBA("Material_DiffuseColour", material->diffuseColour);

	assignMatTexture(shader, "Diffuse", material->diffuseTexture);
	if (material->diffuseTexture)
		shader->setUniformi("Material_DiffuseTextureSRGB", material->diffuseTexture->getParameters().getSRGB());
	else
		shader->setUniformi("Material_DiffuseTextureSRGB", 0);

	//Check to see whether the shader is for lighting
	if (shaderName == SHADER_LIGHTING || shaderName == SHADER_TERRAIN || shaderName == SHADER_PBR_LIGHTING) {
		//Assign other lighting specific properties
		shader->setUniformColourRGB("Material_AmbientColour", material->ambientColour);
		shader->setUniformColourRGB("Material_SpecularColour", material->specularColour);

		assignMatTexture(shader, "Ambient", material->ambientTexture);
		assignMatTexture(shader, "Specular", material->specularTexture);
		assignMatTexture(shader, "Shininess", material->shininessTexture);

		if (material->normalMap) {
			shader->setUniformi("Material_NormalMap", bindTexture(material->normalMap));
			shader->setUniformi("UseNormalMap", 1);
		} else
			shader->setUniformi("UseNormalMap", 0);

		if (material->parallaxMap) {
			shader->setUniformi("Material_ParallaxMap", bindTexture(material->parallaxMap));
			shader->setUniformf("Material_ParallaxScale", material->parallaxScale);
			shader->setUniformi("UseParallaxMap", 1);
		} else
			shader->setUniformi("UseParallaxMap", 0);

		shader->setUniformf("Material_Shininess", material->shininess);
	}
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
		shader->setUniformMatrix4("MVPMatrix", (getCamera()->getProjectionViewMatrix() * modelMatrix));
		if (mesh->hasData() && mesh->hasRenderData()) {
			MeshData* data = mesh->getData();
			MeshRenderData* renderData = mesh->getRenderData();

			if (mesh->hasSkeleton()) {
				for (unsigned int i = 0; i < mesh->getSkeleton()->getNumBones(); i++)
					shader->setUniformMatrix4("Bones[" + utils_string::str(i) + "]", mesh->getSkeleton()->getBone(i)->getFinalTransform());
				shader->setUniformi("UseSkinning", 1);
			} else
				shader->setUniformi("UseSkinning", 0);

			if (data->hasSubData()) {
				renderData->getRenderData()->bindVAO();

				//Go through each sub data instance
				for (unsigned int i = 0; i < data->getSubDataCount(); i++) {
					saveTextures();

					if (mesh->hasMaterial())
						setMaterialUniforms(renderShader->getShader(), renderShader->getName(), mesh->getMaterial(data->getSubData(i).materialIndex));
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

void Renderer::render(FramebufferTexture* texture, Shader* shader) {
	if (shader == NULL)
		shader = getRenderShader(SHADER_PLAIN_TEXTURE)->getShader();

	shader->use();

	shader->setUniformi("Texture", bindTexture(texture));

	screenTextureMesh->render();

	shader->stopUsing();

	unbindTexture();
}

void Renderer::destroy() {
	delete screenTextureMesh;
}

using namespace utils_string;

Shader* Renderer::loadEngineShader(std::string path) {
	return Shader::loadShader("resources/shaders/" + path);
}

void Renderer::prepareForwardShader(std::string id, Shader* shader) {
	shader->use();
	if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_DEFERRED_LIGHTING || id == SHADER_PBR_LIGHTING || id == SHADER_PBR_DEFERRED_LIGHTING) {
		shader->addUniform("UseNormalMap", "ue_useNormalMap");

		shader->addUniform("UseShadowMap", "ue_useShadowMap");
		shader->addUniform("ShadowMap", "ue_shadowMap");
		shader->addUniform("UseParallaxMap", "ue_useParallaxMap");

		for (unsigned int i = 0; i < RenderScene3D::NUM_LIGHTS_IN_SET; i++) {
			shader->addUniform("LightSpaceMatrix["     + str(i) + "]", "ue_lightSpaceMatrix[" + str(i) + "]");
			shader->addUniform("Light_Type["           + str(i) + "]", "ue_lights[" + str(i) + "].type");
			shader->addUniform("Light_Position["       + str(i) + "]", "ue_lights[" + str(i) + "].position");
			shader->addUniform("Light_Direction["      + str(i) + "]", "ue_lights[" + str(i) + "].direction");
			shader->addUniform("Light_DiffuseColour["  + str(i) + "]", "ue_lights[" + str(i) + "].diffuseColour");
			shader->addUniform("Light_SpecularColour[" + str(i) + "]", "ue_lights[" + str(i) + "].specularColour");
			shader->addUniform("Light_Constant["       + str(i) + "]", "ue_lights[" + str(i) + "].constant");
			shader->addUniform("Light_Linear["         + str(i) + "]", "ue_lights[" + str(i) + "].linear");
			shader->addUniform("Light_Quadratic["      + str(i) + "]", "ue_lights[" + str(i) + "].quadratic");
			shader->addUniform("Light_InnerCutoff["    + str(i) + "]", "ue_lights[" + str(i) + "].innerCutoff");
			shader->addUniform("Light_OuterCutoff["    + str(i) + "]", "ue_lights[" + str(i) + "].outerCutoff");
			shader->addUniform("Light_ShadowMap["      + str(i) + "]", "ue_lights[" + str(i) + "].shadowMap");
			shader->addUniform("Light_UseShadowMap["   + str(i) + "]", "ue_lights[" + str(i) + "].useShadowMap");
		}

		for (unsigned int i = 0; i < 80; i++)
			shader->addUniform("Bones[" + str(i) + "]", "ue_bones[" + str(i) + "]");

		shader->addUniform("NumLights", "ue_numLights");
		shader->addUniform("LightAmbient", "ue_lightAmbient");
		shader->addUniform("CameraPosition", "ue_cameraPosition");

		shader->addUniform("EnvironmentMap", "ue_environmentMap");
		shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
	} else if (id == SHADER_SHADOW_MAP) {
		for (unsigned int i = 0; i < 80; i++)
			shader->addUniform("Bones[" + str(i) + "]", "ue_bones[" + str(i) + "]");
	}

	shader->stopUsing();
}

void Renderer::prepareDeferredGeomShader(std::string id, Shader* shader) {
	shader->use();
	if (id == SHADER_LIGHTING || id == SHADER_TERRAIN || id == SHADER_PBR_LIGHTING) {
		shader->addUniform("UseNormalMap", "ue_useNormalMap");

		shader->addUniform("UseShadowMap", "ue_useShadowMap");
		shader->addUniform("ShadowMap", "ue_shadowMap");
		shader->addUniform("UseParallaxMap", "ue_useParallaxMap");

		for (unsigned int i = 0; i < 80; i++)
			shader->addUniform("Bones[" + str(i) + "]", "ue_bones[" + str(i) + "]");

		shader->addUniform("NumLights", "ue_numLights");
		shader->addUniform("LightAmbient", "ue_lightAmbient");
		shader->addUniform("CameraPosition", "ue_cameraPosition");

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
