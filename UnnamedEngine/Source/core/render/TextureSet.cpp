/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "TextureSet.h"

#include "RenderData.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The TextureSet class
 *****************************************************************************/

void TextureSet::bindGLTextures() {
	//Go through each texture info and bind the texture to the correct texture unit
	for (unsigned int i = 0; i < textures.size(); ++i) {
		if (textures[i].texture != NULL) {
			glActiveTexture(GL_TEXTURE0 + textures[i].binding);
			textures[i].texture->bind();
		}
	}
}

void TextureSet::unbindGLTextures() {
	//Go through each texture info and unbind the textures
	for (unsigned int i = 0; i < textures.size(); ++i) {
		if (textures[i].texture != NULL) {
			glActiveTexture(GL_TEXTURE0 + textures[i].binding);
			textures[i].texture->unbind();
		}
	}
}
