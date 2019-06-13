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

#ifndef CORE_VULKAN_VULKANRENDERSHADER_H_
#define CORE_VULKAN_VULKANRENDERSHADER_H_

#include "../render/UBO.h"
#include "../render/Texture.h"
#include "../render/Shader.h"

/*****************************************************************************
 * The VulkanRenderShader class
 *****************************************************************************/

class VulkanRenderShader {
private:
	/* UBO's used with this shader (Does not delete these UBOs) */
	std::vector<UBO*> ubos;

	/* Holds information about a texture being used in the shader */
	struct ShaderTextureInfo {
		Texture*     texture;
		unsigned int binding;
	};

	/* Textures used with this shader (Does not delete these textures) */
	std::vector<ShaderTextureInfo> textures;

	/* Shader used */
	Shader* shader;

	/* The descriptor pool for allocation of descriptors */
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	/* The descriptor set layout */
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	/* The descriptor sets */
	std::vector<VkDescriptorSet> descriptorSets;
public:
	struct UBOData {
		Matrix4f mvpMatrix;
	};

	/* Constructor */
	VulkanRenderShader(Shader* shader);

	/* Destructor */
	virtual ~VulkanRenderShader();

	/* Methods to add a UBO or Texture to this instance */
	inline void add(UBO* ubo) { ubos.push_back(ubo); }
	inline void add(Texture* texture, unsigned int binding) {
		ShaderTextureInfo info;
		info.texture = texture;
		info.binding = binding;
		textures.push_back(info);
	}

	/* Method to setup this render shader (Creates descriptor sets and the layout)  */
	void setup();

	/* Getters */
	inline UBO* getUBO(unsigned int index) { return ubos[index]; }
	inline Shader* getShader() { return shader; }
	inline VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
	inline VkDescriptorSet& getDescriptorSet(unsigned int index) { return descriptorSets[index]; }
};


#endif /* CORE_VULKAN_VULKANRENDERSHADER_H_ */
