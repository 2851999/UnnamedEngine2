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

#include "SSBO.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/VulkanUtils.h"

/*****************************************************************************
 * The SSBO class
 *****************************************************************************/

SSBO::SSBO(void* data, unsigned int size, DataUsage usage, unsigned int blockBinding) {
	this->size = size;
	this->blockBinding = blockBinding;

	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		//Setup the UBO
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, Renderer::convertToGL(usage));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, blockBinding, buffer);
	} else {
		//Apply the offset for Vulkan
		this->blockBinding += VULKAN_BINDING_OFFSET;

		//Setup the buffer for Vulkan
		vulkanBuffer = new VulkanBufferObject(size, Vulkan::getDevice(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, true); //Always allow UBO's to be updated
	}
}

SSBO::~SSBO() {
	if (buffer > 0)
		glDeleteBuffers(1, &buffer);
	else if (vulkanBuffer) {
		delete vulkanBuffer;
	}
}

void SSBO::bindGL() {
	//Bind the buffer
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, blockBinding, buffer);
}

void SSBO::updateFrame(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	} else {
		//Update the current buffer
		vulkanBuffer->updateFrame(data, offset, size);
	}
}

void SSBO::update(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	} else
		//Update the current buffer
		vulkanBuffer->update(data, offset, size);
}

VkWriteDescriptorSet SSBO::getVkWriteDescriptorSet(unsigned int frame, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo* bufferInfo) {
	VkWriteDescriptorSet writeDescriptor = utils_vulkan::initWriteDescriptorSet(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorSet, blockBinding);
	writeDescriptor.descriptorCount  = 1;
	writeDescriptor.pBufferInfo      = bufferInfo;

	return writeDescriptor;
}
