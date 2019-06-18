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

#ifndef CORE_RENDER_RENDERDATA_H_
#define CORE_RENDER_RENDERDATA_H_

#include "VBO.h"
#include "UBO.h"
#include "Texture.h"
#include "../vulkan/VulkanGraphicsPipeline.h"
#include "TextureSet.h"

/*****************************************************************************
 * The RenderData class is used for rendering
 *****************************************************************************/

class RenderData {
private:
	/* The VAO */
	GLuint vao = 0;
	/* The VBO's used for rendering */
	std::vector<VBO<GLfloat>*> vbosFloat;
	std::vector<VBO<GLuint>*>  vbosUInteger;
	VBO<unsigned int>*         vboIndices = NULL;

	/* The render mode */
	GLenum mode;

	/* States the 'count' used for rendering - the number of positions/
	 * indices */
	GLsizei count;

	/* States the number of instances to render, instancing is only used if
	 * this value is greater than zero */
	GLsizei primcount = -1;

	/* The vertex buffer instances and offsets for Vulkan */
	std::vector<VkBuffer> vboVkInstances;
	std::vector<VkDeviceSize> vboVkOffsets;

	/* The pipeline used to render the data */
	VulkanGraphicsPipeline* graphicsVkPipeline = NULL;

	/* UBO's used with to render (Does not delete these UBOs) */
	std::vector<UBO*> ubos;

	/* The texture binding locations used when rendering */
	std::vector<unsigned int> textureBindings;

	/* TextureSet's stored for rendering (one per material) */
	std::vector<TextureSet*> textureSets;

	/* The descriptor pool for allocation of descriptors */
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	/* The descriptor set layout */
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	/* Number of swap chain images when created */
	unsigned int numSwapChainImages = 0;

	/* The descriptor sets */
	std::vector<VkDescriptorSet> descriptorSets;
public:
	/* The constructor */
	RenderData(GLenum mode, GLsizei count) : mode(mode), count(count) {}

	/* The destructor */
	virtual ~RenderData();

	/* The method used to setup this data for rendering */
	void setup(Shader* shader);

	/* The method used to setup this data for rendering (With Vulkan) */
	void setupVulkan(Shader* shader);

	/* Methods to add a UBO or Texture to this instance */
	inline void add(UBO* ubo) { ubos.push_back(ubo); }
	void add(Texture* texture, unsigned int binding);
	inline void addTextureSet(TextureSet* set) { textureSets.push_back(set); }

	/* Method used to bind/unbind the VAO/other buffers before/after rendering */
	void bindBuffers();
	void unbindBuffers();

	/* The method used to render this data */
	inline void render() {
		bindBuffers();
		renderWithoutBinding();
		unbindBuffers();
	}

	/* The method to render this data without binding/unbinding the VAO */
	void renderWithoutBinding();

	/* Method to render this data using glDrawElementsBaseVertex */
	void renderBaseVertex(unsigned int count, unsigned int indicesOffset, unsigned int baseVertex);

	/* The setters and getters */
	inline void addVBO(VBO<GLfloat>* vbo) { vbosFloat.push_back(vbo); }
	inline void addVBO(VBO<GLuint>* vbo) { vbosUInteger.push_back(vbo); }
	inline void setIndicesVBO(VBO<unsigned int>* vboIndices) { this->vboIndices = vboIndices; }
	inline void setMode(GLenum mode) { this->mode = mode; }
	inline void setCount(GLsizei count) { this->count = count; }
	inline void setNumInstances(GLsizei primcount) { this->primcount = primcount; }

	inline GLuint getVAO() { return vao; }
	inline std::vector<UBO*>& getUBOs() { return ubos; }
	inline UBO* getUBO(unsigned int index) { return ubos[index]; }
	inline TextureSet* getTextureSet(unsigned int index) { return textureSets[index]; }
	inline VkDescriptorPool& getVkDescriptorPool() { return descriptorPool; }
	inline VkDescriptorSetLayout& getVkDescriptorSetLayout() { return descriptorSetLayout; }
	inline VkDescriptorSet& getVkDescriptorSet(unsigned int index) { return descriptorSets[index]; }
};

#endif /* CORE_RENDER_RENDERDATA_H_ */
