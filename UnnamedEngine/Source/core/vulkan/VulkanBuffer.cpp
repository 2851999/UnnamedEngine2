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

#include "Vulkan.h"

#include <cstring>

#include "../../utils/Logging.h"
#include "VulkanBuffer.h"

/*****************************************************************************
 * The VulkanBuffer class
 *****************************************************************************/

VulkanBuffer::VulkanBuffer(VkDeviceSize bufferSize, VulkanDevice* device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool useStaging) {
	this->device = device;
	this->useStaging = useStaging;
	this->size = bufferSize;

	//Create the buffer
	Vulkan::createBuffer(bufferSize, usage, properties, instance, bufferMemory);

	//Assign the descriptor info
	bufferInfo.buffer = instance;
	bufferInfo.offset = 0;
	bufferInfo.range  = VK_WHOLE_SIZE;
}

VulkanBuffer::VulkanBuffer(void* data, VkDeviceSize size, VulkanDevice* device, VkBufferUsageFlags usage, bool useStaging) :
		VulkanBuffer(size, device, useStaging ? VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage : usage, useStaging ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), useStaging) {

	//Copy the data to this buffer
	copyData(data, 0, size);
}

void VulkanBuffer::copyData(void* dataToCopy, unsigned int offset, VkDeviceSize size) {
	//Check if supposed to use staging
	if (useStaging) {
		//Create the staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		//Pass the data to the staging buffer
		copyData(dataToCopy, offset, size, stagingBufferMemory);

		//Copy data to the actual buffer being used
		Vulkan::copyBuffer(stagingBuffer, instance, size, Vulkan::getCommandPool(), device->getGraphicsQueue());

		vkDestroyBuffer(device->getLogical(), stagingBuffer, nullptr);
		vkFreeMemory(device->getLogical(), stagingBufferMemory, nullptr);
	} else
		//Pass the data to the buffer
		copyData(dataToCopy, offset, size, bufferMemory);
}

void VulkanBuffer::copyData(void* dataToCopy, unsigned int offset, VkDeviceSize& size, VkDeviceMemory& dest) {
	//Map buffer memory into CPU accessible memory
	void* data;
	vkMapMemory(device->getLogical(), dest, offset, size, 0, &data); //Data is now mapped
	memcpy(data, dataToCopy, (size_t) size);
	vkUnmapMemory(device->getLogical(), dest); //Driver not necessarily copied yet, can either use heap that is host coherent,
	//or use vkFlushMappedMemoryRanges/vkInvalidateMappedMemoryRanges
	//First option ensures mapped memory always matches the contents of allocated memory (may lead to worse performance than explicit flushing - but doesn't matter?)
	//Guaranteed to be complete as of next vkQueueSubmit
}

VulkanBuffer::~VulkanBuffer() {
	vkDestroyBuffer(device->getLogical(), instance, nullptr);
	vkFreeMemory(device->getLogical(), bufferMemory, nullptr);
}


