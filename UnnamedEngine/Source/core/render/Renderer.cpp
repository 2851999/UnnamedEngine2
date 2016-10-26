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
		Logger::log("No Camera added, nothing will render", "Renderer", Logger::Warning);
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
	addRenderShader("Material", Shader::loadShader("resources/shaders/MaterialShader"));
	addRenderShader("SkyBox", Shader::loadShader("resources/shaders/SkyBoxShader"));
	addRenderShader("Font", Shader::loadShader("resources/shaders/FontShader"));
	addRenderShader("Billboard", Shader::loadShader("resources/shaders/BillboardShader"));
	addRenderShader("Particle", Shader::loadShader("resources/shaders/ParticleShader"));
	addRenderShader("Lighting", Shader::loadShader("resources/shaders/LightingShader"));
	addRenderShader("Framebuffer", Shader::loadShader("resources/shaders/FramebufferShader"));
	addRenderShader("EnvironmentMap", Shader::loadShader("resources/shaders/EnvironmentMapShader"));
	addRenderShader("ShadowMap", Shader::loadShader("resources/shaders/ShadowMapShader"));
	addRenderShader("BillboardedFont", Shader::loadShader("resources/shaders/BillboardedFontShader"));

	//Setup the screen texture mesh
	MeshData* meshData = new MeshData(MeshData::DIMENSIONS_2D);
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(-1.0f, -1.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, 1.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
	screenTextureMesh = new MeshRenderData(meshData, getRenderShader("Framebuffer"));
}

void Renderer::render(std::vector<Mesh*>& meshes, Matrix4f& modelMatrix, RenderShader* renderShader) {
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

		for (unsigned int i = 0; i < meshes.size(); i++) {
			if (meshes[i]->getRenderData()) {

				saveTextures();

				meshes[i]->getMaterial()->setUniforms(renderShader->getShader(), renderShader->getName());

				meshes[i]->getRenderData()->render();

				releaseNewTextures();
			}
		}
	}
}

void Renderer::render(FramebufferTexture* texture, Shader* shader) {
	if (shader == NULL)
		shader = getRenderShader("Framebuffer")->getShader();

	shader->use();

	shader->setUniformi("Texture", bindTexture(texture));

	screenTextureMesh->render();

	shader->stopUsing();

	unbindTexture();
}

void Renderer::destroy() {
	delete screenTextureMesh;
}

using namespace StrUtils;
void Renderer::addRenderShader(std::string id, Shader* shader) {
	shader->use();
	if (id == "Lighting") {
		shader->addUniform("MVPMatrix", "mvpMatrix");
		shader->addUniform("Material_AmbientColour", "material.ambientColour");
		shader->addUniform("Material_DiffuseColour", "material.diffuseColour");
		shader->addUniform("Material_SpecularColour", "material.specularColour");
		shader->addUniform("Material_DiffuseTexture", "material.diffuseTexture");
		shader->addUniform("Material_SpecularTexture", "material.specularTexture");
		shader->addUniform("Material_NormalMap", "material.normalMap");
		shader->addUniform("Material_Shininess", "material.shininess");
		shader->addUniform("UseNormalMap", "useNormalMap");

		shader->addUniform("ModelMatrix", "modelMatrix");
		shader->addUniform("NormalMatrix", "normalMatrix");
		shader->addUniform("LightSpaceMatrix", "lightSpaceMatrix");

		shader->addUniform("UseShadowMap", "useShadowMap");
		shader->addUniform("ShadowMap", "shadowMap");

		for (unsigned int i = 0; i < 6; i++) {
			shader->addUniform("Light_Type["           + str(i) + "]", "lights[" + str(i) + "].type");
			shader->addUniform("Light_Position["       + str(i) + "]", "lights[" + str(i) + "].position");
			shader->addUniform("Light_Direction["      + str(i) + "]", "lights[" + str(i) + "].direction");
			shader->addUniform("Light_DiffuseColour["  + str(i) + "]", "lights[" + str(i) + "].diffuseColour");
			shader->addUniform("Light_SpecularColour[" + str(i) + "]", "lights[" + str(i) + "].specularColour");
			shader->addUniform("Light_Constant["       + str(i) + "]", "lights[" + str(i) + "].constant");
			shader->addUniform("Light_Linear["         + str(i) + "]", "lights[" + str(i) + "].linear");
			shader->addUniform("Light_Quadratic["      + str(i) + "]", "lights[" + str(i) + "].quadratic");
			shader->addUniform("Light_Cutoff["         + str(i) + "]", "lights[" + str(i) + "].cutoff");
			shader->addUniform("Light_OuterCutoff["    + str(i) + "]", "lights[" + str(i) + "].outerCutoff");
		}

		shader->addUniform("NumLights", "numLights");
		shader->addUniform("Light_Ambient", "light_ambient");
		shader->addUniform("Camera_Position", "camera_position");

		shader->addUniform("EnvironmentMap", "environmentMap");
		shader->addUniform("UseEnvironmentMap", "useEnvironmentMap");

		shader->addAttribute("Position", "position");
		shader->addAttribute("TextureCoordinate", "textureCoord");
		shader->addAttribute("Normal", "normal");
		shader->addAttribute("Tangent", "tangent");
		shader->addAttribute("Bitangent", "bitangent");
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
		Logger::log("The RenderShader with the id '" + id + "' could not be found", "Renderer", Logger::Error);
		return NULL;
	}
}
