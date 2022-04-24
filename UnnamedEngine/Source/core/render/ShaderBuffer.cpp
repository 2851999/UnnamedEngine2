/*****************************************************************************
 *
 *   Copyright 2022 Joel Davies
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

#include "ShaderBuffer.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/VulkanUtils.h"

 /*****************************************************************************
  * The ShaderBuffer class
  *****************************************************************************/

ShaderBuffer::ShaderBuffer(Type type, void* data, unsigned int size, DataUsage usage, unsigned int blockBinding) {
	this->size = size;
	this->blockBinding = blockBinding;

	//Check the type and assign the appropriate OpenGL/Vulkan usage
	if (type == Type::UBO) {
		glTarget           = GL_UNIFORM_BUFFER;
		vkBufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		vkDescriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	} else if (type == Type::SSBO) {
		glTarget           = GL_SHADER_STORAGE_BUFFER;
		vkBufferUsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		vkDescriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	}


	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		//Setup the UBO
		glGenBuffers(1, &buffer);
		glBindBuffer(glTarget, buffer);
		glBufferData(glTarget, size, data, Renderer::convertToGL(usage));
		glBindBuffer(glTarget, 0);

		glBindBufferBase(glTarget, blockBinding, buffer);
	} else {
		//Apply the offset for Vulkan
		this->blockBinding += VULKAN_BINDING_OFFSET;

		//Setup the buffer for Vulkan
		vulkanBuffer = new VulkanBufferObject(size, Vulkan::getDevice(), vkBufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, true); //Always allow UBO's to be updated
	}
}

ShaderBuffer::~ShaderBuffer() {
	if (buffer > 0)
		glDeleteBuffers(1, &buffer);
	else if (vulkanBuffer) {
		delete vulkanBuffer;
	}
}

void ShaderBuffer::bindGL() {
	//Bind the buffer
	glBindBufferBase(glTarget, blockBinding, buffer);
}

void ShaderBuffer::updateFrame(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (!BaseEngine::usingVulkan()) {
		glBindBuffer(glTarget, buffer);
		glBufferSubData(glTarget, offset, size, data);
		glBindBuffer(glTarget, 0);
	}
	else {
		//Update the current buffer
		vulkanBuffer->updateFrame(data, offset, size);
	}
}

void ShaderBuffer::update(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (!BaseEngine::usingVulkan()) {
		glBindBuffer(glTarget, buffer);
		glBufferSubData(glTarget, offset, size, data);
		glBindBuffer(glTarget, 0);
	}
	else
		//Update the current buffer
		vulkanBuffer->update(data, offset, size);
}

VkWriteDescriptorSet ShaderBuffer::getVkWriteDescriptorSet(unsigned int frame, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo* bufferInfo) {
	VkWriteDescriptorSet writeDescriptor = utils_vulkan::initWriteDescriptorSet(vkDescriptorType, descriptorSet, blockBinding);
	writeDescriptor.descriptorCount = 1;
	writeDescriptor.pBufferInfo = bufferInfo;

	return writeDescriptor;
}

VkDescriptorType ShaderBuffer::convertToVkDescriptorType(Type type) {
	if (type == Type::UBO)
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	else if (type == Type::SSBO)
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

	//Default
	return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}
