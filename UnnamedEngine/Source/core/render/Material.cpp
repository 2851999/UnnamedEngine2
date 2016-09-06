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

#include "Material.h"
#include "Renderer.h"

/*****************************************************************************
 * The Material class
 *****************************************************************************/

void Material::setUniforms(Shader* shader, std::string shaderName) {
	shader->setUniformColourRGBA("Material_DiffuseColour", diffuseColour);

	if (diffuseTexture)
		shader->setUniformi("Material_DiffuseTexture", Renderer::bindTexture(diffuseTexture));
	else
		shader->setUniformi("Material_DiffuseTexture", Renderer::bindTexture(Renderer::getBlankTexture()));

	//Check to see whether the shader is for lighting
	if (shaderName == "Lighting") {
		//Assign other lighting specific properties
		shader->setUniformColourRGB("Material_AmbientColour", ambientColour);
		shader->setUniformColourRGB("Material_SpecularColour", specularColour);

		if (specularTexture)
			shader->setUniformi("Material_SpecularTexture", Renderer::bindTexture(specularTexture));
		else
			shader->setUniformi("Material_SpecularTexture", Renderer::bindTexture(Renderer::getBlankTexture()));

		if (normalMap) {
			shader->setUniformi("Material_NormalMap", Renderer::bindTexture(normalMap));
			shader->setUniformi("UseNormalMap", 1);
		} else
			shader->setUniformi("UseNormalMap", 0);

		shader->setUniformf("Material_Shininess", shininess);
	}
}
