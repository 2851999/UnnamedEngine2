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

/*****************************************************************************
 * The Renderer class
 *****************************************************************************/

std::vector<Camera*> Renderer::cameras;
std::vector<Texture*> Renderer::boundTextures;
std::map<std::string, RenderShader*> Renderer::renderShaders;
Texture* Renderer::blank;

MeshRenderData* Renderer::screenTextureMesh;

unsigned int Renderer::boundTexturesOldSize;

const std::string Renderer::SHADER_MATERIAL         = "Material";
const std::string Renderer::SHADER_SKY_BOX          = "SkyBox";
const std::string Renderer::SHADER_FONT             = "Font";
const std::string Renderer::SHADER_BILLBOARD        = "Billboard";
const std::string Renderer::SHADER_PARTICLE         = "Particle";
const std::string Renderer::SHADER_LIGHTING         = "Lighting";
const std::string Renderer::SHADER_FRAMEBUFFER      = "Framebuffer";
const std::string Renderer::SHADER_ENVIRONMENT_MAP  = "EnvironmentMap";
const std::string Renderer::SHADER_SHADOW_MAP       = "ShadowMap";
const std::string Renderer::SHADER_BILLBOARDED_FONT = "BillboardedFont";
const std::string Renderer::SHADER_CDLOD_TERRAIN    = "CDLODTerrain";

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

void Renderer::initialise() {
	glewInit();

	blank = Texture::loadTexture("resources/textures/blank.png");

	//Setup the shaders
	addRenderShader(SHADER_MATERIAL,         Shader::loadShader("resources/shaders/MaterialShader"));
	addRenderShader(SHADER_SKY_BOX,          Shader::loadShader("resources/shaders/SkyBoxShader"));
	addRenderShader(SHADER_FONT,             Shader::loadShader("resources/shaders/FontShader"));
	addRenderShader(SHADER_BILLBOARD,        Shader::loadShader("resources/shaders/BillboardShader"));
	addRenderShader(SHADER_PARTICLE,         Shader::loadShader("resources/shaders/ParticleShader"));
	addRenderShader(SHADER_LIGHTING,         Shader::loadShader("resources/shaders/LightingShader"));
	addRenderShader(SHADER_FRAMEBUFFER,      Shader::loadShader("resources/shaders/FramebufferShader"));
	addRenderShader(SHADER_ENVIRONMENT_MAP,  Shader::loadShader("resources/shaders/EnvironmentMapShader"));
	addRenderShader(SHADER_SHADOW_MAP,       Shader::loadShader("resources/shaders/ShadowMapShader"));
	addRenderShader(SHADER_BILLBOARDED_FONT, Shader::loadShader("resources/shaders/BillboardedFontShader"));
	addRenderShader(SHADER_CDLOD_TERRAIN,    Shader::loadShader("resources/shaders/CDLODTerrain"));

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

void Renderer::setMaterialUniforms(Shader* shader, std::string shaderName, Material* material) {
	shader->setUniformColourRGBA("Material_DiffuseColour", material->diffuseColour);

	if (material->diffuseTexture)
		shader->setUniformi("Material_DiffuseTexture", bindTexture(material->diffuseTexture));
	else
		shader->setUniformi("Material_DiffuseTexture", bindTexture(Renderer::getBlankTexture()));

	//Check to see whether the shader is for lighting
	if (shaderName == SHADER_LIGHTING) {
		//Assign other lighting specific properties
		shader->setUniformColourRGB("Material_AmbientColour", material->ambientColour);
		shader->setUniformColourRGB("Material_SpecularColour", material->specularColour);

		if (material->specularTexture)
			shader->setUniformi("Material_SpecularTexture", bindTexture(material->specularTexture));
		else
			shader->setUniformi("Material_SpecularTexture", Renderer::bindTexture(Renderer::getBlankTexture()));

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
		shader = getRenderShader(SHADER_LIGHTING)->getShader();

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
void Renderer::addRenderShader(std::string id, Shader* shader) {
	shader->use();
	if (id == SHADER_LIGHTING) {
		shader->addUniform("UseNormalMap", "ue_useNormalMap");
		shader->addUniform("LightSpaceMatrix", "ue_lightSpaceMatrix");

		shader->addUniform("UseShadowMap", "ue_useShadowMap");
		shader->addUniform("ShadowMap", "ue_shadowMap");
		shader->addUniform("UseParallaxMap", "ue_useParallaxMap");

		for (unsigned int i = 0; i < 6; i++) {
			shader->addUniform("Light_Type["           + str(i) + "]", "ue_lights[" + str(i) + "].type");
			shader->addUniform("Light_Position["       + str(i) + "]", "ue_lights[" + str(i) + "].position");
			shader->addUniform("Light_Direction["      + str(i) + "]", "ue_lights[" + str(i) + "].direction");
			shader->addUniform("Light_DiffuseColour["  + str(i) + "]", "ue_lights[" + str(i) + "].diffuseColour");
			shader->addUniform("Light_SpecularColour[" + str(i) + "]", "ue_lights[" + str(i) + "].specularColour");
			shader->addUniform("Light_Constant["       + str(i) + "]", "ue_lights[" + str(i) + "].constant");
			shader->addUniform("Light_Linear["         + str(i) + "]", "ue_lights[" + str(i) + "].linear");
			shader->addUniform("Light_Quadratic["      + str(i) + "]", "ue_lights[" + str(i) + "].quadratic");
			shader->addUniform("Light_Cutoff["         + str(i) + "]", "ue_lights[" + str(i) + "].cutoff");
			shader->addUniform("Light_OuterCutoff["    + str(i) + "]", "ue_lights[" + str(i) + "].outerCutoff");
		}

		for (unsigned int i = 0; i < 80; i++)
			shader->addUniform("Bones[" + str(i) + "]", "ue_bones[" + str(i) + "]");

		shader->addUniform("NumLights", "ue_numLights");
		shader->addUniform("Light_Ambient", "ue_light_ambient");
		shader->addUniform("Camera_Position", "ue_camera_position");

		shader->addUniform("EnvironmentMap", "ue_environmentMap");
		shader->addUniform("UseEnvironmentMap", "ue_useEnvironmentMap");
	} else if (id == SHADER_SHADOW_MAP) {
		for (unsigned int i = 0; i < 80; i++)
			shader->addUniform("Bones[" + str(i) + "]", "ue_bones[" + str(i) + "]");
	}

	shader->stopUsing();

	//Add the shader
	addRenderShader(new RenderShader(id, shader));
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
