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

/*****************************************************************************
 * The UBO class
 *****************************************************************************/

UBO::UBO(void* data, unsigned int size, GLenum usage, GLuint blockBinding) {
	//Check whether using Vulkan or OpenGL
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Setup the UBO
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, buffer);
	} else {
		//Setup the buffer for Vulkan
		vulkanBuffers.resize(Vulkan::getSwapChain()->getImageCount());

		for (unsigned int i = 0; i < Vulkan::getSwapChain()->getImageCount(); ++i)
			vulkanBuffers[i] = new VulkanBuffer(size, Vulkan::getDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	}
}

UBO::~UBO() {
	if (buffer > 0)
		glDeleteBuffers(1, &buffer);
	else if (vulkanBuffers.size() > 0) {
		for (VulkanBuffer* buffer : vulkanBuffers)
			delete buffer;
		vulkanBuffers.clear();
	}
}

void UBO::update(void* data, GLintptr offset, GLsizeiptr size) {
	//Check whether using Vulkan or OpenGL
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	} else {
		//Update the current buffer
		vulkanBuffers[Vulkan::getCurrentFrame()]->copyData(data, offset, size);
	}
}
