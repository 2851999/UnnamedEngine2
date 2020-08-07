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

#include <GL/glew.h>

#include "RenderShader.h"
#include "../vulkan/VulkanBuffer.h"
#include "DataUsage.h"

/*****************************************************************************
 * The VBO class is used to manage a vertex buffer object
 *****************************************************************************/

enum class DataUsage;

template <typename T>
class VBO {
public:
	/* A structure containing information for supplying VBO data to an
	 * attribute within a shader */
	struct Attribute {
		/* Various pieces of data for OpenGL */
		int  location;
		int  size; //This is not literal size, but the number of
				   //components
		unsigned int offset;

		unsigned int divisor;

		GLuint type;
	};
private:
	/* Buffer handle for OpenGL */
	GLuint bufferGL = 0;

	/* The Vulkan buffer for this VBO */
	VulkanBufferObject* vulkanBuffer = NULL;

	/* Data for this VBO */
	uint32_t        size;
	std::vector<T>& data;
	DataUsage       usage;

	/* The stride for data in this VBO */
	unsigned int    stride = 0;

	/* States whether this VBO will be used in instanced rendering */
	bool            instanced;

	/* The attributes this VBO supplies */
	std::vector<Attribute> attributes;

	/* The binding/attribute descriptions for Vulkan */
	VkVertexInputBindingDescription vulkanVertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vulkanAttributeDescriptions;
public:
	/* The constructors */
	VBO(uint32_t size, std::vector<T>& data, DataUsage usage, bool instanced = false) :
		size(size), data(data), usage(usage), instanced(instanced) {}

	/* The destructor */
	virtual ~VBO() {
		if (bufferGL > 0)
			glDeleteBuffers(1, &bufferGL);
		delete vulkanBuffer;
	}

	/* Various OpenGL methods */
	inline void bindGL() { glBindBuffer(GL_ARRAY_BUFFER, bufferGL); }

	/* Used to add an attribute */
	void addAttributeWithType(GLuint type, GLint location, int size, GLuint divisor = 0);
	inline void addAttribute(int location, int size, unsigned int divisor = 0) { addAttributeWithType(GL_FLOAT, location, size, divisor); }

	/* Used to create and setup this VBO */
	void setup(unsigned int binding); //Binding for Vulkan

	/* Used to setup an attribute */
	void setupAttribute(unsigned int binding, unsigned int index);

	/* Methods used to setup this VBO just before/after rendering */
	void startRendering();
	void stopRendering();

	/* Methods used to update the VBO's data */
	void update();
	void updateFrame();
	void updateStream(uint32_t size);

	/* Setters and getters */
	inline std::vector<T>& getData() { return data; }
	inline VulkanBuffer* getVkCurrentBuffer() { return vulkanBuffer->getCurrentBuffer(); }
	inline VkVertexInputBindingDescription getVkBindingDescription() { return vulkanVertexInputBindingDescription; }
	inline std::vector<VkVertexInputAttributeDescription> getVkAttributeDescriptions() { return vulkanAttributeDescriptions; }
};
