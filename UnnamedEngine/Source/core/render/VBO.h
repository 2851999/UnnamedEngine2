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

#include "Shader.h"
#include "../vulkan/VulkanBuffer.h"

/*****************************************************************************
 * The VBO class is used to manage a vertex buffer object
 *****************************************************************************/

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
	/* Various pieces of data for OpenGL */
	GLuint                buffer;
	GLenum                target;
	GLsizeiptr            size;
	std::vector<T>&       data;
	GLenum                usage;

	/* The stride for data in this VBO */
	unsigned int          stride = 0;

	bool                  instanced;

	/* The bulkan buffer for use with Vulkan */
	VulkanBuffer* vulkanBuffer = NULL;

	/* The attributes this VBO supplies */
	std::vector<Attribute> attributes;

	/* The binding/attribute descriptions for Vulkan */
	VkVertexInputBindingDescription vulkanVertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vulkanAttributeDescriptions;
public:
	/* The constructors */
	VBO(GLenum target, GLsizeiptr size, std::vector<T>& data, GLenum usage, bool instanced = false) :
		buffer(0), target(target), size(size), data(data), usage(usage), instanced(instanced) {}

	/* The destructor */
	virtual ~VBO() {
		if (buffer > 0)
			glDeleteBuffers(1, &buffer);
		if (vulkanBuffer != NULL) {
			delete vulkanBuffer;
			vulkanBuffer = NULL;
		}
	}

	/* Various OpenGL methods */
	inline void bind() { glBindBuffer(target, buffer); }

	/* Used to add an attribute */
	void addAttributeWithType(GLuint type, GLint location, GLint size, GLuint divisor = 0);
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
	void updateStream(GLsizeiptr size);

	/* Setters and getters */
	inline std::vector<T>& getData() { return data; }
	inline VulkanBuffer* getVkBuffer() { return vulkanBuffer; }
	inline VkVertexInputBindingDescription getVkBindingDescription() { return vulkanVertexInputBindingDescription; }
	inline std::vector<VkVertexInputAttributeDescription> getVkAttributeDescriptions() { return vulkanAttributeDescriptions; }
};

