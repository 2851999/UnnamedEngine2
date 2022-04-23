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

#include "UBO.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/VulkanUtils.h"

/*****************************************************************************
 * The UBO class
 *****************************************************************************/

UBO::UBO(void* data, unsigned int size, DataUsage usage, unsigned int blockBinding) {
	this->size = size;
	this->blockBinding = blockBinding;

	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		//Setup the UBO
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, size, data, Renderer::convertToGL(usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, buffer);
	} else {
		//Apply the offset for Vulkan
		this->blockBinding += VULKAN_BINDING_OFFSET;

		//TODO: REMOVE THIS
		//      NEED EITHER STORAGE BUFFER CLASS OR WAY TO SPECIFY THIS
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (Window::getCurrentInstance()->getSettings().videoRaytracing) {
			if (this->blockBinding == 22) {
				usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			}
		}

		//Setup the buffer for Vulkan
		vulkanBuffer = new VulkanBufferObject(size, Vulkan::getDevice(), usageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, true); //Always allow UBO's to be updated
	}
}

UBO::~UBO() {
	if (buffer > 0)
		glDeleteBuffers(1, &buffer);
	else if (vulkanBuffer) {
		delete vulkanBuffer;
	}
}

void UBO::bindGL() {
	//Bind the buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, buffer);
}

void UBO::updateFrame(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	} else {
		//Update the current buffer
		vulkanBuffer->updateFrame(data, offset, size);
	}
}

void UBO::update(void* data, unsigned int offset, unsigned int size) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	} else
		//Update the current buffer
		vulkanBuffer->update(data, offset, size);
}

VkWriteDescriptorSet UBO::getVkWriteDescriptorSet(unsigned int frame, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo* bufferInfo) {

	//TODO: REMOVE THIS
	//      NEED EITHER STORAGE BUFFER CLASS OR WAY TO SPECIFY THIS
	VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	if (Window::getCurrentInstance()->getSettings().videoRaytracing) {
		if (blockBinding == 22) {
			descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		}
	}
	VkWriteDescriptorSet writeDescriptor = utils_vulkan::initWriteDescriptorSet(descriptorType, descriptorSet, blockBinding);
	writeDescriptor.descriptorCount  = 1;
	writeDescriptor.pBufferInfo      = bufferInfo;

	return writeDescriptor;
}
