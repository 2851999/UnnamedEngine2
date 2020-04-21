/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "Text.h"

#include "../BaseEngine.h"
#include "../render/Renderer.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The Text class
 *****************************************************************************/

unsigned int Text::DEFAULT_MAX_CHARACTERS = 1000;

Text::Text(Font* font, Colour colour, unsigned int maxCharacters, bool billboarded) {
	this->billboarded = billboarded;
	this->maxCharacters = maxCharacters;

	if (maxCharacters == 0)
		//Use the default number
		maxCharacters = DEFAULT_MAX_CHARACTERS;

	//The shader type to use
	unsigned int shaderType;
	if (! billboarded)
		shaderType = Renderer::SHADER_FONT;
	else {
		//shaderType = Renderer::SHADER_BILLBOARDED_FONT;
		//Get the UBO for the billboard
		//shaderBillboardUBO = Renderer::getShaderInterface()->getUBO(ShaderInterface::BLOCK_BILLBOARD);
	}

	//Create the Mesh instance and assign the texture
	Texture* fontTexture = font->getTexture();

	MeshData* meshData;
	if (! BaseEngine::usingVulkan())
		meshData = MeshBuilder::createQuad3D(fontTexture->getWidth(), fontTexture->getHeight(), fontTexture, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS);
	else {
		meshData = new MeshData(3, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS);

		unsigned int numPositions = maxCharacters * 12;;
		unsigned int numTextureCoords = maxCharacters * 8;
		unsigned int numIndices = maxCharacters * 6;
		meshData->getPositions().resize(numPositions);
		meshData->getTextureCoords().resize(numTextureCoords);
		meshData->getIndices().resize(numIndices);
		meshData->setNumPositions(maxCharacters);
		meshData->setNumTextureCoords(numTextureCoords);
		meshData->setNumIndices(numIndices);
	}

	Mesh* mesh = new Mesh(meshData);
	//Assign the font
	this->font = font;
	mesh->getMaterial()->setDiffuse(fontTexture);
	mesh->getMaterial()->update();
	setMesh(mesh, Renderer::getRenderShader(shaderType));

	//Assign the colour and other properties
	setColour(colour);
	setScale(1.0f / (float) Font::RENDER_SCALE, 1.0f / (float) Font::RENDER_SCALE, 1.0f);
	getMesh()->setCullingEnabled(false);

	GameObject3D::update();

	//Obtain the graphics pipeline used for rendering
	pipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_FONT), Renderer::getDefaultRenderPass());
}

Text::~Text() {
	delete pipeline;
}

void Text::update(std::string text) {
	if ((! BaseEngine::usingVulkan()) || text.size() <= maxCharacters) {
		//Assign the current text
		currentText = text;

		//The MeshData to update
		MeshData* data = getMesh()->getData();

		//Clear the previous data
		data->clearPositions();
		data->clearTextureCoords();
		data->clearIndices();

		//Assign the new data
		font->assignMeshData(data, text, billboarded);

		//Update the data
		getMesh()->getRenderData()->updatePositions(data);
		getMesh()->getRenderData()->updateTextureCoords();
		getMesh()->getRenderData()->updateIndices(data);
	} else
		Logger::log("Cannot update text as the requested text exceeds the maximum number of characters", "Text", LogType::Warning);
}

void Text::update(std::string text, Vector2f position) {
	update(text);

	//Update the position
	update(position);
}

void Text::update(std::string text, Vector3f position) {
	update(text);

	//Update the position
	update(position);
}

void Text::update(Vector3f position) {
	setPosition(position);
	GameObject3D::update();
}

void Text::render() {
	//Bind the pipeline
	pipeline->bind();

	if (billboarded) {
		Shader* shader = getShader();
		shader->use();

		Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

		shaderBillboardData.ue_cameraRight = Vector4f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2), 0.0f);
		shaderBillboardData.ue_cameraUp = Vector4f(-matrix.get(1, 0), -matrix.get(1, 1), -matrix.get(1, 2), 0.0f);
		shaderBillboardData.ue_billboardSize = Vector2f(0.005f, 0.005f);
		shaderBillboardData.ue_billboardCentre = Vector4f(getPosition(), 0.0f);

		shaderBillboardData.ue_projectionViewMatrix = (Renderer::getCamera()->getProjectionViewMatrix());

		shaderBillboardUBO->update(&shaderBillboardData, 0, sizeof(ShaderBlock_Billboard));

		GameObject3D::render();

		shader->stopUsing();
	} else
		GameObject3D::render();
}

void Text::setFont(Font* font) {
	this->font = font;
	getMaterial()->setDiffuse(font->getTexture());
}

void Text::setColour(Colour colour) {
	getMaterial()->setDiffuse(colour);
}
