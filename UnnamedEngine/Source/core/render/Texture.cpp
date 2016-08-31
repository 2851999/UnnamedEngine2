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

#include <stb/stb_image.h>

#include "../../utils/Logging.h"
#include "Texture.h"
#include "../Window.h"

/*****************************************************************************
 * The TextureParameters class
 *****************************************************************************/

GLuint TextureParameters::DEFAULT_TARGET = GL_TEXTURE_2D;
GLuint TextureParameters::DEFAULT_FILTER = GL_NEAREST;
GLuint TextureParameters::DEFAULT_CLAMP  = GL_CLAMP_TO_EDGE;
bool   TextureParameters::DEFAULT_SHOULD_CLAMP = false;

void TextureParameters::apply(GLuint texture, bool bind, bool unbind) {
	if (bind)
		glBindTexture(target, texture);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	if (shouldClamp) {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, clamp);
		if (target == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(target, GL_TEXTURE_WRAP_R, clamp);
	}
	if (filter == GL_NEAREST_MIPMAP_NEAREST ||
			filter == GL_NEAREST_MIPMAP_LINEAR ||
			filter == GL_LINEAR_MIPMAP_NEAREST ||
			filter == GL_LINEAR_MIPMAP_LINEAR) {
		glGenerateMipmap(target);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples);
	}
	if (unbind)
		glBindTexture(target, 0);
}

/*****************************************************************************
 * The Texture class stores the data required to render a texture
 *****************************************************************************/

unsigned char* Texture::loadTexture(std::string path, int& numComponents, GLsizei& width, GLsizei& height, GLint& format) {
	int w, h, numC;
	unsigned char* image = stbi_load(path.c_str(), &w, &h, &numC, 0);
	if (image == nullptr) {
		Logger::log("Failed to load the image from the path '" + path + "'");
		return NULL;
	}
	GLint colourMode = 0;
	if (numC == 1)
		colourMode = GL_RED;
	else if (numC == 2)
		colourMode = GL_RG;
	else if (numC == 3)
		colourMode = GL_RGB;
	else if (numC == 4)
		colourMode = GL_RGBA;

	numComponents = numC;
	width = w;
	height = h;
	format = colourMode;

	return image;
}

Texture* Texture::loadTexture(std::string path, TextureParameters parameters, bool applyParameters) {
	int numComponents, w, h, format;
	unsigned char* image = loadTexture(path, numComponents, w, h, format);

	Texture* texture = new Texture(parameters);
	texture->setWidth(w);
	texture->setHeight(h);
	texture->setNumComponents(numComponents);

	texture->bind();

	glTexImage2D(parameters.getTarget(), 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);

	if (applyParameters)
		texture->applyParameters(false, true);

	stbi_image_free(image);
	return texture;
}
