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

#include "VulkanRenderShader.h"

#include "Vulkan.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The VulkanRenderShader class
 *****************************************************************************/

VulkanRenderShader::VulkanRenderShader(Shader* shader) {
	this->shader = shader;
}

VulkanRenderShader::~VulkanRenderShader() {
	vkDestroyDescriptorSetLayout(Vulkan::getDevice()->getLogical(), descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(Vulkan::getDevice()->getLogical(), descriptorPool, nullptr);
}

void VulkanRenderShader::setup() {
	unsigned int swapChainImageCount = Vulkan::getSwapChain()->getImageCount();

	//---------------------------CREATE DESCRIPTOR POOL---------------------------
	//Assign the creation info
	std::vector<VkDescriptorPoolSize> poolSizes = {};
	for (unsigned int i = 0; i < ubos.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(swapChainImageCount);
		poolSizes.push_back(poolSize);
	}
	for (unsigned int i = 0; i < textures.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = static_cast<uint32_t>(swapChainImageCount); //Have one for each swap chain image
		poolSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = static_cast<uint32_t>(swapChainImageCount);
	poolInfo.flags         = 0;

	//Attempt to create the pool
	if (vkCreateDescriptorPool(Vulkan::getDevice()->getLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		Logger::log("Failed to create descriptor pool", "VulkanRenderShader", LogType::Error);

	//---------------------------CREATE DESCRIPTOR SET LAYOUT---------------------------
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	//Go through the UBO's
	for (UBO* ubo : ubos) {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding            = ubo->getBinding();
		uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount    = 1;
		uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT; //VK_SHADER_STAGE_ALL_GRAPHICS
		uboLayoutBinding.pImmutableSamplers = nullptr; //Optional

		//Add the binding
		bindings.push_back(uboLayoutBinding);
	}

	//Go through the textures
	for (ShaderTextureInfo textureInfo : textures) {
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding            = textureInfo.binding;
		samplerLayoutBinding.descriptorCount    = 1;
		samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings.push_back(samplerLayoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings    = bindings.data();

	//Create the descriptor set layout
	if (vkCreateDescriptorSetLayout(Vulkan::getDevice()->getLogical(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		Logger::log("Failed to create descriptor set layout", "VulkanRenderShader", LogType::Error);

	//---------------------------CREATE DESCRIPTOR SETS---------------------------
	std::vector<VkDescriptorSetLayout> layouts(swapChainImageCount, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImageCount);
	allocInfo.pSetLayouts        = layouts.data();

	descriptorSets.resize(swapChainImageCount);
	if (vkAllocateDescriptorSets(Vulkan::getDevice()->getLogical(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		Logger::log("Failed to allocate descriptor sets", "VulkanRenderShader", LogType::Error);

	//Allows writing of each UBO and texture
	for (unsigned int i = 0; i < swapChainImageCount; ++i) {

	    std::vector<VkWriteDescriptorSet> descriptorWrites = {};
	    for (UBO* ubo : ubos) {
			VkDescriptorBufferInfo bufferInfo = ubo->getVkBuffer(i)->getBufferInfo();

	    	descriptorWrites.push_back(ubo->getVkWriteDescriptorSet(i, descriptorSets[i], &bufferInfo));
	    }
	    for (ShaderTextureInfo textureInfo : textures) {
		    VkDescriptorImageInfo imageInfo = textureInfo.texture->getVkImageInfo();

	    	VkWriteDescriptorSet textureWrite;
	    	textureWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			textureWrite.dstSet           = descriptorSets[i];
			textureWrite.dstBinding       = textureInfo.binding;
			textureWrite.dstArrayElement  = 0;
			textureWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			textureWrite.descriptorCount  = 1;
			textureWrite.pBufferInfo      = nullptr;
			textureWrite.pImageInfo       = &imageInfo;
			textureWrite.pTexelBufferView = nullptr;
			textureWrite.pNext            = nullptr;
			descriptorWrites.push_back(textureWrite);
	    }

		vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
