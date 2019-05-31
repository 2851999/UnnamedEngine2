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

#include "../render/Renderer.h"

/*****************************************************************************
 * The Text class
 *****************************************************************************/

Text::Text(Font* font, Colour colour, bool billboarded) {
	this->billboarded = billboarded;

	//The shader type to use
	std::string shaderType;
	if (! billboarded)
		shaderType = Renderer::SHADER_FONT;
	else
		shaderType = Renderer::SHADER_BILLBOARDED_FONT;

	//Create the Mesh instance and assign the texture
	Texture* fontTexture = font->getTexture();

	setMesh(new Mesh(MeshBuilder::createQuad3D(fontTexture->getWidth(), fontTexture->getHeight(), fontTexture, MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(shaderType));

	//Assign the colour and other properties
	setColour(colour);
	setScale(1.0f / (float) Font::RENDER_SCALE, 1.0f / (float) Font::RENDER_SCALE, 1.0f);
	getMesh()->setCullingEnabled(false);

	//Assign the font
	setFont(font);

	GameObject3D::update();
}

void Text::update(std::string text) {
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
	if (billboarded) {
		Shader* shader = getShader();
		shader->use();

		Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

		shader->setUniformVector3("Camera_Right", Vector3f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2)));
		shader->setUniformVector3("Camera_Up", Vector3f(-matrix.get(1, 0), -matrix.get(1, 1), -matrix.get(1, 2)));
		shader->setUniformVector2("Billboard_Size", Vector2f(0.005f, 0.005f));
		shader->setUniformVector3("Billboard_Centre", getPosition());

		shader->setUniformMatrix4("ProjectionViewMatrix", (Renderer::getCamera()->getProjectionViewMatrix()));

		GameObject3D::render();

		shader->stopUsing();
	} else
		GameObject3D::render();
}

void Text::setFont(Font* font) {
	this->font = font;
	getMaterial()->diffuseTexture = font->getTexture();
}

void Text::setColour(Colour colour) {
	getMaterial()->diffuseColour = colour;
}
