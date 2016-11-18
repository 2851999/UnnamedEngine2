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

struct Material {
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
};

#endif /* CORE_RENDER_MATERIAL_H_ */
