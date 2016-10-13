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
 * The Material class stores data about a material and can apply them to a
 * shader
 *****************************************************************************/

class Material {
private:
	/* The colours */
	Colour ambientColour  = Colour(0.1f, 0.1f, 0.1f);
	Colour diffuseColour  = Colour::WHITE;
	Colour specularColour = Colour::WHITE;

	/* The textures */
	Texture* ambientTexture  = NULL;
	Texture* diffuseTexture  = NULL;
	Texture* specularTexture = NULL;

	Texture* normalMap = NULL;

	/* The shininess value */
	float shininess = 32.0f;
public:
	/* Various constructors */
	Material() {}
	Material(Colour diffuseColour) : diffuseColour(diffuseColour) {}
	Material(Texture* diffuseTexture) : diffuseTexture(diffuseTexture) {}
	Material(Colour diffuseColour, Texture* diffuseTexture) : diffuseColour(diffuseColour), diffuseTexture(diffuseTexture) {}

	virtual ~Material() {}

	/* The method used to apply the material properties to a shader assuming it is already being used */
	void setUniforms(Shader* shader, std::string shaderName);

	/* The setters and getters */
	inline void setAmbientColour(Colour colour)  { ambientColour = colour;  }
	inline void setDiffuseColour(Colour colour)  { diffuseColour = colour;  }
	inline void setSpecularColour(Colour colour) { specularColour = colour; }
	inline void setAmbientTexture(Texture* texture)  { ambientTexture = texture;  }
	inline void setDiffuseTexture(Texture* texture)  { diffuseTexture = texture;  }
	inline void setSpecularTexture(Texture* texture) { specularTexture = texture; }
	inline void setNormalMap(Texture* texture)   { normalMap = texture;     }
	inline void setShininess(float shininess) { this->shininess = shininess; }

	inline Colour getAmbientColour()  { return ambientColour;  }
	inline Colour getDiffuseColour()  { return diffuseColour;  }
	inline Colour getSpecularColour() { return specularColour; }
	inline Texture* getAmbientTexture()  { return ambientTexture;  }
	inline Texture* getDiffuseTexture()  { return diffuseTexture;  }
	inline Texture* getSpecularTexture() { return specularTexture; }
	inline Texture* getNormalMap()    { return normalMap; }
	inline float getShininess() { return shininess; }

	inline bool hasAmbientTexture()  { return ambientTexture;  }
	inline bool hasDiffuseTexture()  { return diffuseTexture;  }
	inline bool hasSpecularTexture() { return specularTexture; }
};

#endif /* CORE_RENDER_MATERIAL_H_ */
