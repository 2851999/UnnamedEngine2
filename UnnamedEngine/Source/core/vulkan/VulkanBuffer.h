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

class VulkanBuffer {
private:
	/* States whether this buffer needs to use a staging buffer (i.e. memory used is inaccessable to CPU */
	bool useStaging;

	/* Device this buffer is for */
	VulkanDevice* device;

	/* The buffer instance */
	VkBuffer instance;

	/* The memory used by this buffer */
	VkDeviceMemory bufferMemory;

	/* The descriptor info about this buffer */
	VkDescriptorBufferInfo bufferInfo;

	/* The size of this buffer */
	VkDeviceSize size;

	/* Method to copy memory to a buffer (that is accessible to CPU) */
	void copyData(void* data, unsigned int offset, VkDeviceSize& size, VkDeviceMemory& dest);
public:
	/* Constructor */
	VulkanBuffer(VkDeviceSize bufferSize, VulkanDevice* device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool useStaging = true);
	VulkanBuffer(void* data, VkDeviceSize size, VulkanDevice* device, VkBufferUsageFlags usage, bool useStaging = true);

	/* Destructor */
	virtual ~VulkanBuffer();

	/* Method used to copy data to this buffer */
	void copyData(void* data, unsigned int offset, VkDeviceSize size);

	/* Setters and getters */
	VkBuffer& getInstance() { return instance; }
	const VkDescriptorBufferInfo* getBufferInfo() { return &bufferInfo; } //Require const here otherwise seems to give an occasional error with offset = 0x1b
	VkDeviceSize& getSize() { return size; }
};


#endif /* CORE_VULKAN_VULKANBUFFER_H_ */
