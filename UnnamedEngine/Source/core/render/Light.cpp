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

#include "Light.h"

/*****************************************************************************
 * The Light class
 *****************************************************************************/

void Light::update() {
	//Check the type of light this is
	if (type == TYPE_DIRECTIONAL) {
		Vector3f right = direction.cross(Vector3f(0.0f, 1.0f, 0.0f)).normalise();
		Vector3f up = right.cross(direction).normalise();

		lightView.initLookAt(direction * -5, (direction * 5) + direction, up);
	}
}

void Light::setUniforms(Shader* shader, std::string suffix) {
	shader->setUniformi("Light_Type" + suffix, type);
	shader->setUniformVector3("Light_Position" + suffix, getPosition());
	shader->setUniformVector3("Light_Direction" + suffix, direction);
	shader->setUniformColourRGB("Light_DiffuseColour"  + suffix, diffuseColour);
	shader->setUniformColourRGB("Light_SpecularColour" + suffix, specularColour);
	shader->setUniformf("Light_Constant" + suffix, constant);
	shader->setUniformf("Light_Linear" + suffix, linear);
	shader->setUniformf("Light_Quadratic" + suffix, quadratic);
	shader->setUniformf("Light_Cutoff" + suffix, cutoff);
	shader->setUniformf("Light_OuterCutoff" + suffix, outerCutoff);
}

