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

#ifndef CORE_RENDER_TEXTURESET_H_
#define CORE_RENDER_TEXTURESET_H_

#include "Texture.h"
#include "UBO.h"

/*****************************************************************************
 * The TextureSet class is used for binding textures for OpenGL/Creating
 * and managing a descriptor set along with RenderData for Vulkan
 *****************************************************************************/

class RenderData;

class TextureSet {
public:
	/* Holds information about a texture */
	struct TextureInfo {
		/* The binding location of the texture */
		unsigned int binding = 0;
		/* The texture */
		Texture* texture = NULL;
	};
private:
	/* The textures in this set */
	std::vector<TextureInfo> textures;

	/* Descriptor sets for this texture set (one per swap chain image) */
	std::vector<VkDescriptorSet> descriptorSets;
public:
	/* Constructor */
	TextureSet() {}

	/* Destructor */
	virtual ~TextureSet() {}

	/* Method to add a texture to this set */
	inline void add(unsigned int binding, Texture* texture = NULL) {
		TextureInfo info;
		info.binding = binding;
		info.texture = texture;
		textures.push_back(info);
	}

	/* Methods to bind/unbind the textures inside this set (for OpenGL) */
	void bindGLTextures();
	void unbindGLTextures();

	inline std::vector<TextureInfo> getTexturesInfo() { return textures; }
};


#endif /* CORE_RENDER_TEXTURESET_H_ */
