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

/*****************************************************************************
 * The Material class
 ******************************************************************************/

void Material::setDefault(bool pbr) {
	if (! pbr) {
		ambientColour  = Colour(0.1f, 0.1f, 0.1f);
		diffuseColour  = Colour::WHITE;
		specularColour = Colour::WHITE;
		shininess = 32.0f;
	} else {
		ambientColour  = Colour(0.0f);
		diffuseColour  = Colour::WHITE;
		specularColour = Colour(1.0f);
		shininess = 0.0f;
	}

	parallaxScale = 0.05f;
}
