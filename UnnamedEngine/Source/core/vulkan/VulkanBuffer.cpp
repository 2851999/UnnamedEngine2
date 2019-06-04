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

template <typename T>
VulkanBuffer<T>::VulkanBuffer(T* vertexData, unsigned int dataCount, VulkanDevice* device, VkBufferUsageFlags usage) {
	this->device = device;
	//Create the vertex buffer

	VkDeviceSize bufferSize = sizeof(T) * dataCount;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map buffer memory into CPU accessible memory
	void* data;
	vkMapMemory(device->getLogical(), stagingBufferMemory, 0, bufferSize, 0, &data); //Data is now mapped
	memcpy(data, vertexData, (size_t) bufferSize);
	vkUnmapMemory(device->getLogical(), stagingBufferMemory); //Driver not necessarily copied yet, can either use heap that is host coherent,
	//or use vkFlushMappedMemoryRanges/vkInvalidateMappedMemoryRanges
	//First option ensures mapped memory always matches the contents of allocated memory (may lead to worse performance than explicit flushing - but doesn't matter?)
	//Guaranteed to be complete as of next vkQueueSubmit

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instance, bufferMemory);

	copyBuffer(stagingBuffer, instance, bufferSize, Vulkan::getCommandPool(), device->getGraphicsQueue());

	vkDestroyBuffer(device->getLogical(), stagingBuffer, nullptr);
	vkFreeMemory(device->getLogical(), stagingBufferMemory, nullptr);
}

template <typename T>
VulkanBuffer<T>::~VulkanBuffer() {
	vkDestroyBuffer(device->getLogical(), instance, nullptr);
	vkFreeMemory(device->getLogical(), bufferMemory, nullptr);
}

template <typename T>
void VulkanBuffer<T>::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //Only used by graphics queue

	if (vkCreateBuffer(device->getLogical(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		Logger::log("Failed to create vertex buffer", "VulkanBuffer", LogType::Error);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device->getLogical(), buffer, &memoryRequirements);

	//Allocate memory to this buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getLogical(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		Logger::log("Failed to allocate vertex buffer memory", "VulkanBuffer", LogType::Error);

	//Associate the memory with the buffer
	vkBindBufferMemory(device->getLogical(), buffer, bufferMemory, 0);
}

template <typename T>
void VulkanBuffer<T>::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool, VkQueue& graphicsQueue) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool        = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device->getLogical(), &allocInfo, &commandBuffer);

	//Start recording to the command buffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT not necessary here since only using once and want to wait for copy to finish

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size      = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	//Stop recording to the buffer
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device->getLogical(), commandPool, 1, &commandBuffer);
}

template <typename T>
uint32_t VulkanBuffer<T>::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(device->getPhysical(), &memoryProperties);

	//Should also check what heap memory comes from since this can also affect performance - not done here

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) //Check if corresponding bit for current memory type is 1
				&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	Logger::log("Failed to find suitable memory type", "VulkanBuffer", LogType::Error);
	return 0;
}


