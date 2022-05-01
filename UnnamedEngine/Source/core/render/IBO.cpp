/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "IBO.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"

#include <type_traits>

 /*****************************************************************************
  * The IBO class
  *****************************************************************************/

void IBO::setup(VkBufferUsageFlags additionalVkUsageFlags) {
	//Check whether using Vulkan or OpenGL
	if (! BaseEngine::usingVulkan()) {
		//Get OpenGL to generate the buffer
		glGenBuffers(1, &bufferGL);
		//Bind the buffer
		bindGL();
		//Pass the data to OpenGL
		if (data.size() > 0)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &data.front(), Renderer::convertToGL(usage));
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, Renderer::convertToGL(usage));
	}

	if (BaseEngine::usingVulkan()) {
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | additionalVkUsageFlags;

		//Create the Vulkan buffer
		vulkanBuffer = new VulkanBufferObject(data.data(), sizeof(unsigned int) * data.size(), Vulkan::getDevice(), usageFlags, usage == DataUsage::STATIC, usage != DataUsage::STATIC); //Assume wont be updated if static
	}
}

void IBO::startRendering() {
	if (! BaseEngine::usingVulkan())
		bindGL();
}

void IBO::stopRendering() {

}

void IBO::updateFrame() {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned int), data.data(), Renderer::convertToGL(usage));
	} else
		vulkanBuffer->updateFrame(data.data(), 0, data.size() * sizeof(unsigned int));
}

void IBO::update() {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned int), data.data(), Renderer::convertToGL(usage));
	} else
		vulkanBuffer->update(data.data(), 0, data.size() * sizeof(unsigned int));
}

void IBO::updateStream(uint32_t size) {
	if (! BaseEngine::usingVulkan()) {
		bindGL();
		//Buffer orphaning
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size, NULL, Renderer::convertToGL(usage));
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, &data.front());
	} else {
		//MAY BE BETTER WAY

		vulkanBuffer->updateFrame(data.data(), 0, size);
	}
}