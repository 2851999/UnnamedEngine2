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

#include "Texture.h"

#include "../Window.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The TextureParameters class
 *****************************************************************************/

/* Define the default parameters */
GLuint TextureParameters::DEFAULT_TARGET = GL_TEXTURE_2D;
GLuint TextureParameters::DEFAULT_FILTER = GL_NEAREST;
GLuint TextureParameters::DEFAULT_CLAMP  = GL_CLAMP_TO_EDGE;
bool   TextureParameters::DEFAULT_SHOULD_CLAMP = false;

void TextureParameters::apply(GLuint texture, bool bind, bool unbind) {
	//Bind the texture if necessary
	if (bind)
		glBindTexture(target, texture);
	//Setup the filter
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	//Setup texture clamping if necessary
	if (shouldClamp) {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, clamp);
		//One more value for cube maps
		if (target == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(target, GL_TEXTURE_WRAP_R, clamp);
	}
	//Sets up mipmapping if requested
	if (filter == GL_NEAREST_MIPMAP_NEAREST ||
			filter == GL_NEAREST_MIPMAP_LINEAR ||
			filter == GL_LINEAR_MIPMAP_NEAREST ||
			filter == GL_LINEAR_MIPMAP_LINEAR) {
		glGenerateMipmap(target);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples);
	}
	//Unbind the texture if necessary
	if (unbind)
		glBindTexture(target, 0);
}

/*****************************************************************************
 * The Texture class
 *****************************************************************************/

unsigned char* Texture::loadTexture(std::string path, int& numComponents, GLsizei& width, GLsizei& height, GLint& format) {
	//Load the data using stb_image
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &numComponents, 0);

	//Check that the data was loaded
	if (image == nullptr) {
		//Log an error if not
		Logger::log("Failed to load the image from the path '" + path + "'", "Texture", LogType::Error);
		return NULL;
	}

	//Check the number of components and assign the right OpenGL format
	if (numComponents == 1)
		format = GL_RED;
	else if (numComponents == 2)
		format = GL_RG;
	else if (numComponents == 3)
		format = GL_RGB;
	else if (numComponents == 4)
		format = GL_RGBA;

	return image;
}

void Texture::freeTexture(unsigned char* texture) {
	stbi_image_free(texture);
}

Texture* Texture::loadTexture(std::string path, TextureParameters parameters, bool applyParameters) {
	//The data needed for the texture
	int numComponents, w, h, format;
	//Obtain the texture data
	unsigned char* image = loadTexture(path, numComponents, w, h, format);

	if (image == NULL)
		return NULL;

	//Create the Texture instance and set it up
	Texture* texture = new Texture(parameters);
	texture->setWidth(w);
	texture->setHeight(h);
	texture->setNumComponents(numComponents);
	texture->setPath(path);

	//Bind the texture and then pass the texture data to OpenGL
	texture->bind();

	glTexImage2D(parameters.getTarget(), 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);

	//Apply the parameters if requested, but don't need to bind the texture again,
	//and it doesn't need to unbind either
	if (applyParameters)
		texture->applyParameters(false, false);

	texture->unbind();

	//Free the image data as it is no longer needed
	stbi_image_free(image);

	return texture;
}

/*****************************************************************************
 * The Cubemap class
 *****************************************************************************/

Cubemap::Cubemap(std::string path, std::vector<std::string> faces) : Texture() {
	//Ensure the correct number of textures are present
	if (faces.size() == 6) {
		//Assign the texture parameters
		parameters.setTarget(GL_TEXTURE_CUBE_MAP);
		parameters.setFilter(GL_LINEAR);
		parameters.setClamp(GL_CLAMP_TO_EDGE);
		parameters.setShouldClamp(true);
		//Bind this cubemap
		bind();

		//Data required for setting up
		int numComponents, width, height, format;
		//The current texture
		unsigned char* image;

		//Go through each face
		for (unsigned int i = 0; i < faces.size(); i++) {
			//Load the image for the current face
			image = Texture::loadTexture(path + faces[i], numComponents, width, height, format);
			//Setup the texture
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
			//Free the loaded image data
			Texture::freeTexture(image);
		}

		//Now that the cubemap is fully loaded, apply the texture parameters
		applyParameters(false, true);
	} else {
		//Log an error
		Logger::log("Need 6 faces for a cubemap texture", "Cubemap", LogType::Error);
	}
}
