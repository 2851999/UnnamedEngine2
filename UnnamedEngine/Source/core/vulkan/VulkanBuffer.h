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

#ifndef CORE_VULKAN_VULKANBUFFER_H_
#define CORE_VULKAN_VULKANBUFFER_H_

#include "VulkanDevice.h"

/*****************************************************************************
 * The VulkanBuffer class manages a Vulkan buffer (For vertex data)
 *****************************************************************************/

template <typename T>
class VulkanBuffer {
private:
	/* Device this buffer is for */
	VulkanDevice* device;

	/* The buffer instance */
	VkBuffer instance;

	/* The memory used by this buffer */
	VkDeviceMemory bufferMemory;

	/* Locates a particular kind of graphics memory for a buffer */
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	/* Creates a buffer */
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	/* Copies a buffer from one place to another */
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool, VkQueue& graphicsQueue);
public:
	/* Constructor */
	VulkanBuffer(T* vertexData, unsigned int dataCount, VulkanDevice* device, VkBufferUsageFlags usage);

	/* Destructor */
	virtual ~VulkanBuffer();

	/* Setters and getters */
	VkBuffer& getInstance() { return instance; }
	uint32_t getNumVertices() { return static_cast<uint32_t>(3); }
};

template class VulkanBuffer<float>;
template class VulkanBuffer<unsigned int>;


#endif /* CORE_VULKAN_VULKANBUFFER_H_ */
