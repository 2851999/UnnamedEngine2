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

#include "VBO.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"

#include <type_traits>

/*****************************************************************************
 * The VBO class
 *****************************************************************************/
template <typename T>
void VBO<T>::addAttributeWithType(GLuint type, GLint location, GLint size, GLuint divisor) {
	Attribute attrib;
	attrib.location = location;
	attrib.size     = size;
	attrib.offset   = 0;
	attrib.divisor  = divisor;
	attrib.type     = type;
	attributes.push_back(attrib);
}

template <typename T>
void VBO<T>::setup(unsigned int binding) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		//Get OpenGL to generate the buffer
		glGenBuffers(1, &bufferGL);
		//Bind the buffer
		bindGL();
		//Pass the data to OpenGL
		if (data.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, size, &data.front(), Renderer::convertToGL(usage));
		else
			glBufferData(GL_ARRAY_BUFFER, size, NULL, Renderer::convertToGL(usage));
	} else
		vulkanAttributeDescriptions.resize(attributes.size());

	//Check to see whether the Attributes is > 1, as if this is the case
	//this VBO is interleaved
	if (attributes.size() > 1) {
		//In this case the offset and stride of the attributes need to be
		//calculated

		//The stride to use
		stride = 0;

		//Go through all of their attributes
		for (unsigned int i = 0; i < attributes.size(); ++i)
			stride += attributes[i].size;
		stride *= sizeof(data[0]);

		//Now to setup and assign the attributes values

		//The current offset is incremented so it is defined outside the
		//for loop
		GLint currentOffset = 0;

		//Go through all of the attributes
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			//Assign the offset and stride
			attributes[i].offset = currentOffset;

			//Setup the current attribute
			setupAttribute(binding, i);

			//Increment the current offset
			currentOffset += attributes[i].size * sizeof(data[0]);
		}
	} else {
		//Go through all of the attributes
		for (unsigned int i = 0; i < attributes.size(); i++)
			//Setup the current attribute
			setupAttribute(binding, i);
	}
	if (BaseEngine::usingVulkan()) {
		//Calculate the correct stride if it hasn't been assigned (In OpenGL stride == 0 means tightly packed, but in Vulkan require offset in bytes to next element)
		if (attributes.size() == 1)
			stride = sizeof(T) *  attributes[0].size;

		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (Window::getCurrentInstance()->getSettings().videoRaytracing)
			usageFlags = usageFlags | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

		//Create the Vulkan buffer
		vulkanBuffer = new VulkanBufferObject(data.data(), sizeof(T) * data.size(), Vulkan::getDevice(), usageFlags, usage == DataUsage::STATIC, usage != DataUsage::STATIC); //Assume wont be updated if static

		//Assign the vertex input binding description
		vulkanVertexInputBindingDescription.binding   = binding; //like glVertexAttrib binding
		vulkanVertexInputBindingDescription.stride    = stride;
		vulkanVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //Move to the next data entry after each vertex
	}
}

template <typename T>
void VBO<T>::setupAttribute(unsigned int binding, unsigned int index) {
	//Get a reference to the attribute being assigned
	Attribute& attribute = attributes[index];
	if (! BaseEngine::usingVulkan()) {
		//Ensure the location is valid
		if (attribute.location != -1) {
			//Enable that vertex attribute array and setup its pointer
			glEnableVertexAttribArray(attribute.location);
			if (attribute.type == GL_UNSIGNED_INT || attribute.type == GL_INT)
				glVertexAttribIPointer(attribute.location, attribute.size, attribute.type, stride, (static_cast<const char*>(0) + attribute.offset));
			else
				glVertexAttribPointer(attribute.location, attribute.size, attribute.type, GL_FALSE, stride, (static_cast<const char*>(0) + attribute.offset));
		}
	} else {
		//Setup the attribute description for the attribute
		vulkanAttributeDescriptions[index].binding  = binding;
		vulkanAttributeDescriptions[index].location = attribute.location; //Location for shader
		VkFormat format;
		if (attribute.type == GL_INT) {
			if (attribute.size == 1)
				format = VK_FORMAT_R32_SINT;
			else if (attribute.size == 2)
				format = VK_FORMAT_R32G32_SINT;
			else if (attribute.size == 3)
				format = VK_FORMAT_R32G32B32_SINT;
			else if (attribute.size == 4)
				format = VK_FORMAT_R32G32B32A32_SINT;
		} else {
			if (attribute.size == 1)
				format = VK_FORMAT_R32_SFLOAT;
			else if (attribute.size == 2)
				format = VK_FORMAT_R32G32_SFLOAT;
			else if (attribute.size == 3)
				format = VK_FORMAT_R32G32B32_SFLOAT;
			else if (attribute.size == 4)
				format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		vulkanAttributeDescriptions[index].format = format;
		vulkanAttributeDescriptions[index].offset = attribute.offset;
	}
}

template <typename T>
void VBO<T>::startRendering() {
	if (! BaseEngine::usingVulkan()) {
		bindGL();

		//Enable the vertex attribute arrays
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			glEnableVertexAttribArray(attributes[i].location);
			//Check for instancing
			if (instanced)
				glVertexAttribDivisor(attributes[i].location, attributes[i].divisor);
		}
	}
}

template <typename T>
void VBO<T>::stopRendering() {
	if (! BaseEngine::usingVulkan()) {
		//Disable the vertex attribute arrays
		for (unsigned int i = 0; i < attributes.size(); ++i)
			glDisableVertexAttribArray(attributes[i].location);
	}
}

template <typename T>
void VBO<T>::updateFrame() {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), Renderer::convertToGL(usage));
	} else
		vulkanBuffer->updateFrame(data.data(), 0, data.size() * sizeof(T));
}

template <typename T>
void VBO<T>::update() {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), Renderer::convertToGL(usage));
	} else
		vulkanBuffer->update(data.data(), 0, data.size() * sizeof(T));
}

template <typename T>
void VBO<T>::updateStream(uint32_t size) {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		//Buffer orphaning
		glBufferData(GL_ARRAY_BUFFER, this->size, NULL, Renderer::convertToGL(usage));
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, &data.front());
	} else {
		//MAY BE BETTER WAY

		vulkanBuffer->updateFrame(data.data(), 0, size);
	}
}

template class VBO<float>;
template class VBO<unsigned int>;
