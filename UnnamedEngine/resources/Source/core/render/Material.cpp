/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#include "Material.h"

#include "Renderer.h"
#include "../BaseEngine.h"

/*****************************************************************************
 * The Material class
 ******************************************************************************/

Material::Material(bool pbr) {
	setDefault(pbr);

	//Add the textures required for the texture set
	descriptorSet = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_MATERIAL));
}

Material::~Material() {
	delete descriptorSet;
}

void Material::setDefault(bool pbr) {
	if (! pbr) {
		shaderData.ambientColour  = Colour(0.1f, 0.1f, 0.1f);
		shaderData.diffuseColour  = Colour::WHITE;
		shaderData.specularColour = Colour::WHITE;
		shaderData.shininess = 32.0f;
	} else {
		shaderData.ambientColour  = Colour(0.0f);
		shaderData.diffuseColour  = Colour::WHITE;
		shaderData.specularColour = Colour(1.0f);
		shaderData.shininess = 0.0f;
	}

	shaderData.parallaxScale = 0.05f;

	shaderData.hasAmbientTexture = false;
	shaderData.hasDiffuseTexture = false;
	shaderData.hasSpecularTexture = false;
	shaderData.hasShininessTexture = false;
	shaderData.hasNormalMap = false;
	shaderData.hasParallaxMap = false;
}

void Material::setup() {
	//Update the material UBO
	descriptorSet->getUBO(0)->update(&shaderData, 0, sizeof(ShaderBlock_Material));
	//Setup the descriptor set
	descriptorSet->setup();
}

void Material::update() {
	//Update the material UBO
	descriptorSet->getUBO(0)->update(&shaderData, 0, sizeof(ShaderBlock_Material));
	//Update the descriptor set
	descriptorSet->update();
}
