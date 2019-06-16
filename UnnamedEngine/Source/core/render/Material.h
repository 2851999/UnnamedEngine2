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

#ifndef CORE_RENDER_MATERIAL_H_
#define CORE_RENDER_MATERIAL_H_

#include "Colour.h"
#include "Shader.h"
#include "Texture.h"
#include "ShaderInterface.h"

/*****************************************************************************
 * The Material class stores data about a material
 *
 * NOTE:
 * For PBR components are as follows:
 * AMBIENT -> METALNESS
 * DIFFUSE -> ALBEDO
 * NORMAL -> NORMAL
 * SPECULAR -> AO
 * SHININESS -> ROUGHNESS
 *****************************************************************************/

class Material {
private:
	ShaderBlock_Material shaderData;

	/* The textures */
	Texture* ambientTexture   = NULL;
	Texture* diffuseTexture   = NULL;
	Texture* specularTexture  = NULL;
	Texture* shininessTexture = NULL;

	Texture* normalMap   = NULL;
	Texture* parallaxMap = NULL;
public:
	/* The constructor */
	Material(bool pbr = false) { setDefault(pbr); }

	/* The destructor */
	virtual ~Material() {}

	/* Method called to set the default values */
	void setDefault(bool pbr = false);

	/* Getters and setters */
	void setAmbient(Colour ambientColour) { shaderData.ambientColour = ambientColour; }
	void setDiffuse(Colour diffuseColour) { shaderData.diffuseColour = diffuseColour; }
	void setSpecular(Colour specularColour) { shaderData.specularColour = specularColour; }
	void setAmbient(Texture* ambientTexture) { this->ambientTexture = ambientTexture; shaderData.hasAmbientTexture = ambientTexture != NULL; }
	void setDiffuse(Texture* diffuseTexture) {
		this->diffuseTexture = diffuseTexture;
		shaderData.hasDiffuseTexture = diffuseTexture != NULL;
		if (diffuseTexture != NULL)
			shaderData.diffuseTextureSRGB = diffuseTexture->getParameters().getSRGB();
	}
	void setSpecular(Texture* specularTexture) { this->specularTexture = specularTexture; shaderData.hasSpecularTexture = specularTexture != NULL; }
	void setShininess(Texture* shininessTexture) { this->shininessTexture = shininessTexture; shaderData.hasShininessTexture = shininessTexture != NULL; }
	void setNormalMap(Texture* normalMap) { this->normalMap = normalMap; shaderData.hasNormalMap = normalMap != NULL; }
	void setParallaxMap(Texture* parallaxMap) { this->parallaxMap = parallaxMap; shaderData.hasParallaxMap = parallaxMap != NULL; }
	void setParallaxScale(float parallaxScale) { shaderData.parallaxScale = parallaxScale; }
	void setShininess(float shininess) { shaderData.shininess = shininess; }

	Colour getAmbientColour() { return shaderData.ambientColour; }
	Colour getDiffuseColour() { return shaderData.diffuseColour; }
	Colour getSpecularColour() { return shaderData.specularColour; }
	Texture* getAmbientTexture() { return ambientTexture; }
	Texture* getDiffuseTexture() { return diffuseTexture; }
	Texture* getSpecularTexture() { return specularTexture; }
	Texture* getShininessTexture() { return shininessTexture; }
	Texture* getNormalMap() { return normalMap; }
	Texture* getParallaxMap() { return parallaxMap; }
	float getParallaxScale() { return shaderData.parallaxScale; }
	float getShininess() { return shaderData.shininess; }

	/* Setters for PBR properties (Although they use the same properties, the naming could be confusing) */
	void setMetalness(float metalness) { shaderData.ambientColour = Colour(metalness); }
	void setAlbedo(Colour albedoColour) { shaderData.diffuseColour = albedoColour; }
	void setRoughness(float roughness) { shaderData.shininess = roughness; }
	void setAO(float ao) { shaderData.specularColour = Colour(ao); }

	void setMetalness(Texture* metalnessTexture) { setAmbient(metalnessTexture); }
	void setAlbedo(Texture* albedoTexture) { setDiffuse(albedoTexture); }
	void setRoughness(Texture* roughnessTexture) { setShininess(roughnessTexture); }
	void setAO(Texture* aoTexture) { setSpecular(aoTexture); }

	Texture* getMetalnessTexture() { return this->ambientTexture; }
	Texture* getAlbedoTexture() { return this->diffuseTexture; }
	Texture* getRoughnessTexture() { return this->shininessTexture; }
	Texture* getAOTexture() { return this->specularTexture; }

	float getMetalnessValue() { return shaderData.ambientColour.getX(); }
	Colour getAlbedoColour() { return shaderData.diffuseColour; }
	float getRoughnessValue() { return shaderData.shininess; }
	float getAOValue() { return shaderData.specularColour.getX(); }

	ShaderBlock_Material& getShaderData() { return shaderData; }
};

#endif /* CORE_RENDER_MATERIAL_H_ */
