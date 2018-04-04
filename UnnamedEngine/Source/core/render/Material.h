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

/*****************************************************************************
 * The Material structure stores data about a material and can apply them to a
 * shader
 *****************************************************************************/

//struct Material {
//	/* The colours */
//	Colour ambientColour  = Colour(0.1f, 0.1f, 0.1f);
//	Colour diffuseColour  = Colour::WHITE;
//	Colour specularColour = Colour::WHITE;
//
//	/* The textures */
//	Texture* ambientTexture  = NULL;
//	Texture* diffuseTexture  = NULL;
//	Texture* specularTexture = NULL;
//
//	Texture* normalMap = NULL;
//	Texture* parallaxMap = NULL;
//
//	float parallaxScale = 0.05f;
//
//	/* The shininess value */
//	float shininess = 32.0f;
//};

class Material {
public:
	/* The colours */
	Colour ambientColour  = Colour(0.1f, 0.1f, 0.1f);
	Colour diffuseColour  = Colour::WHITE;
	Colour specularColour = Colour::WHITE;

	/* The textures */
	Texture* ambientTexture   = NULL;
	Texture* diffuseTexture   = NULL;
	Texture* specularTexture  = NULL;
	Texture* shininessTexture = NULL;

	Texture* normalMap = NULL;
	Texture* parallaxMap = NULL;

	float parallaxScale = 0.05f;

	/* The shininess value */
	float shininess = 32.0f;

	/* The constructor */
	Material() {}

	/* The destructor */
	virtual ~Material() {}

	/* Getters and setters */
	void setAmbient(Colour ambientColour) { this->ambientColour = ambientColour; }
	void setDiffuse(Colour diffuseColour) { this->diffuseColour = diffuseColour; }
	void setSpecular(Colour specularColour) { this->specularColour = specularColour; }
	void setAmbient(Texture* ambientTexture) { this->ambientTexture = ambientTexture; }
	void setDiffuse(Texture* diffuseTexture) { this->diffuseTexture = diffuseTexture; }
	void setSpecular(Texture* specularTexture) { this->specularTexture = specularTexture; }
	void setShininess(Texture* shininessTexture) { this->shininessTexture = shininessTexture; }
	void setNormalMap(Texture* normalMap) { this->normalMap = normalMap; }
	void setParallaxMap(Texture* parallaxMap) { this->parallaxMap = parallaxMap; }
	void setParallaxScale(float parallaxScale) { this->parallaxScale = parallaxScale; }
	void setShininess(float shininess) { this->shininess = shininess; }

	Colour getAmbientColour() { return ambientColour; }
	Colour getDiffuseColour() { return diffuseColour; }
	Colour getSpecularColour() { return specularColour; }
	Texture* getAmbientTexture() { return ambientTexture; }
	Texture* getDiffuseTexture() { return diffuseTexture; }
	Texture* getSpecularTexture() { return specularTexture; }
	Texture* getShininessTexture() { return shininessTexture; }
	Texture* getNormalMap() { return normalMap; }
	Texture* getParallaxMap() { return parallaxMap; }
	float getParallaxScale() { return parallaxScale; }
	float getShininess() { return shininess; }

	/* Setters for PBR properties (Although they use the same properties, the naming could be confusing) */
	void setMetalness(float metalness) { this->ambientColour = Colour(metalness); }
	void setAlbedo(Colour albedoColour) { this->diffuseColour = albedoColour; }
	void setRoughness(float roughness) { this->shininess = roughness; }
	void setAO(float ao) { this->specularColour = Colour(ao); }

	void setMetalness(Texture* metalnessTexture) { this->ambientTexture = metalnessTexture; }
	void setAlbedo(Texture* albedoTexture) { this->diffuseTexture = albedoTexture; }
	void setRoughness(Texture* roughnessTexture) { this->shininessTexture = roughnessTexture; }
	void setAO(Texture* aoTexture) { this->shininessTexture = aoTexture; }
};



#endif /* CORE_RENDER_MATERIAL_H_ */
