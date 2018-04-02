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

#include <stb/stb_image.h>

#include "HDRTexture.h"

HDRTexture::HDRTexture(std::string path) {
	//Assign the texture parameters
	parameters.setTarget(GL_TEXTURE_2D);
	parameters.setFilter(GL_LINEAR);
	parameters.setClamp(GL_CLAMP_TO_EDGE);
	parameters.setShouldClamp(true);
	//Bind this cubemap
	bind();

	stbi_set_flip_vertically_on_load(true);
	//Data required for setting up
	int numComponents, width, height;
	//Load the texture
	float* data = stbi_loadf(path.c_str(), &width, &height, &numComponents, 0);
	//Setup the texture
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB16F,
		width,
		height,
		0,
		GL_RGB,
		GL_FLOAT,
		data
	);
	//Free the loaded image data
	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(false);

	//Now that the cubemap is fully loaded, apply the texture parameters
	applyParameters(false, false);
}


