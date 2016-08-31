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

#include "Cubemap.h"

/*****************************************************************************
 * The Cubemap class
 *****************************************************************************/

Cubemap::Cubemap(std::string path, std::vector<std::string> faces) : Texture() {
	//Assign the texture parameters
	parameters.setTarget(GL_TEXTURE_CUBE_MAP);
	parameters.setFilter(GL_LINEAR);
	parameters.setClamp(GL_CLAMP_TO_EDGE);
	parameters.setShouldClamp(true);
	//Bind this cubemap
	bind();

	//Assign the sides
	int numComponents, width, height, format;
	unsigned char* image;

	for (unsigned int i = 0; i < faces.size(); i++) {
		image = Texture::loadTexture(path + faces[i], numComponents, width, height, format);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			format,
			width,
			height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			image
		);
	}

	applyParameters(false, true);
}
