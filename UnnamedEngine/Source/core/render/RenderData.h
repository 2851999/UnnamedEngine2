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

#pragma once

#include "VBO.h"
#include "IBO.h"
#include "UBO.h"
#include "Texture.h"
#include "ShaderInterface.h"

/*****************************************************************************
 * The RenderData class is used for rendering
 *****************************************************************************/

class RenderData {
private:
	/* The VAO */
	GLuint vao = 0;
	/* The VBO's used for rendering */
	std::vector<VBO<float>*>         vbosFloat;
	std::vector<VBO<unsigned int>*>  vbosUInteger;
	IBO*                             ibo = NULL;

	/* States the 'count' used for rendering - the number of positions/
	 * indices */
	int count;

	/* States the number of instances to render, instancing is only used if
	 * this value is greater than zero */
	int primcount = -1;

	/* The vertex buffer instances and offsets for Vulkan */
	std::vector<VkBuffer> vboVkInstances;
	std::vector<VkDeviceSize> vboVkOffsets;

	/* The binding and attribute descriptions for Vulkan */
	std::vector<VkVertexInputBindingDescription> bindingVkDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeVkDescriptions;

	/* The descriptor set corresponding to this render data (model) */
	DescriptorSet* descriptorSetModel = NULL;

	/* Data for the above descriptor set */
	ShaderBlock_Model modelData;
public:
	/* The constructor */
	RenderData(GLsizei count) : count(count) {}

	/* The destructor */
	virtual ~RenderData();

	/* The method used to setup this data for rendering */
	void setup(RenderShader* renderShader, VkBufferUsageFlags additionalVkUsageFlags = 0);

	/* Methods to add a UBO or Texture to this instance */
	void addTexture(Texture* texture, unsigned int binding);

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

	/* Method to render this data using glDrawElementsBaseVertex (indices offset should be index offset) */
	void renderBaseVertex(unsigned int count, unsigned int indicesOffset, unsigned int baseVertex);

	/* The setters and getters */
	inline void addVBO(VBO<float>* vbo) { vbosFloat.push_back(vbo); }
	inline void addVBO(VBO<unsigned int>* vbo) { vbosUInteger.push_back(vbo); }
	inline void setIBO(IBO* ibo) { this->ibo = ibo; }
	inline void setCount(int count) { this->count = count; }
	inline void setNumInstances(int primcount) { this->primcount = primcount; }

	inline GLuint getVAO() { return vao; }
	inline std::vector<VkVertexInputBindingDescription> getVkBindingDescriptions() { return bindingVkDescriptions; }
	inline std::vector<VkVertexInputAttributeDescription> getVkAttributeDescriptions() { return attributeVkDescriptions; }
	inline DescriptorSet* getDescriptorSet() { return descriptorSetModel; }
	inline ShaderBlock_Model& getShaderBlock_Model() { return modelData; }
};

