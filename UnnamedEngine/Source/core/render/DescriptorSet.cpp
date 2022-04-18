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

#include "DescriptorSet.h"

#include "Texture.h"
#include "UBO.h"
#include "Renderer.h"
#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

 /*****************************************************************************
  * The DescriptorSet class
  *****************************************************************************/

DescriptorSet::DescriptorSet(DescriptorSetLayout* layout) : layout(layout) {
	//Obtain the UBO and texture info required from the layout
	std::vector<DescriptorSetLayout::UBOInfo> ubosInfo = layout->getUBOs();
	textureBindings = layout->getTextureBindings();

	raytracing = layout->getRaytracing();

	//Add the required UBOs and textures
	for (DescriptorSetLayout::UBOInfo& uboInfo : ubosInfo) {
		UBO* ubo = new UBO(NULL, uboInfo.size, uboInfo.usage, uboInfo.binding);

		ubos.push_back(ubo);
	}

	for (unsigned int j = 0; j < textureBindings.size(); ++j) {
		textureBindings[j].textures.resize(textureBindings[j].numTextures);
		for (unsigned int i = 0; i < textureBindings[j].numTextures; ++i) {
			TextureInfo info;
			//In Vulkan require same binding with multiple textures, otherwise treat each as a separate binding as in OpenGL
			info.binding           = BaseEngine::usingVulkan() ? textureBindings[j].binding : (textureBindings[j].binding + i);
			info.texture           = NULL;
			info.locationInBinding = i;
			info.bindingInfoIndex  = j;

			textures.push_back(info);
		}
	}
}

DescriptorSet::~DescriptorSet() {
	//Destory Vulkan data
	if (vulkanDescriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(Vulkan::getDevice()->getLogical(), vulkanDescriptorPool, nullptr);
	if (m_isInUpdateQueue)
		Vulkan::removeFromDescriptorSetQueue(this);
	for (UBO* ubo : ubos)
		delete ubo;
	ubos.clear();
}

void DescriptorSet::setupVk() {
	//Obtain the number of needed descriptor sets required
	unsigned int numSwapChainImages = Vulkan::getSwapChain()->getImageCount();
	unsigned int numDescriptorSets = numSwapChainImages;

	//-------------------------------- CREATE DESCRIPTOR POOL --------------------------------

	//Determine the number of descriptors that will need to be allocated in the pool
	std::vector<VkDescriptorPoolSize> poolSizes = {};

	for (unsigned int i = 0; i < ubos.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
		poolSizes.push_back(poolSize);
	}

	for (unsigned int i = 0; i < textures.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
		poolSizes.push_back(poolSize);
	}

	if (raytracing) {
		numDescriptorSets = 1;

		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		poolSize.descriptorCount = 1;
		poolSizes.push_back(poolSize);

		VkDescriptorPoolSize poolSize2;
		poolSize2.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize2.descriptorCount = 1;
		poolSizes.push_back(poolSize2);
	}

	//Create the descriptor pool (Should minimise number created later)
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = static_cast<uint32_t>(numDescriptorSets);
	poolInfo.flags         = 0;

	//Attempt to create the pool
	if (vkCreateDescriptorPool(Vulkan::getDevice()->getLogical(), &poolInfo, nullptr, &vulkanDescriptorPool) != VK_SUCCESS)
		Logger::log("Failed to create descriptor pool", "DescriptorSet", LogType::Error);

	//-------------------------------- CREATE DESCRIPTOR SETS --------------------------------
	std::vector<VkDescriptorSetLayout> layouts(numDescriptorSets, layout->getVkLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = vulkanDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(numDescriptorSets);
	allocInfo.pSetLayouts        = layouts.data();

	vulkanDescriptorSets.resize(numDescriptorSets);

	//Attempt to create the descriptor sets
	if (vkAllocateDescriptorSets(Vulkan::getDevice()->getLogical(), &allocInfo, vulkanDescriptorSets.data()) != VK_SUCCESS)
		Logger::log("Failed to allocate Vulkan descriptor sets", "DescriptorSet", LogType::Error);

	//Update everything for Vulkan
	updateAllVk();
}

void DescriptorSet::setup() {
	if (BaseEngine::usingVulkan())
		setupVk();
}

void DescriptorSet::updateAllVk() {
	//Go through and update the image infos
	for (unsigned int i = 0; i < textures.size(); ++i)
		textureBindings[textures[i].bindingInfoIndex].textures[textures[i].locationInBinding] = (textures[i].texture == NULL) ? ((textureBindings[textures[i].bindingInfoIndex].type == TextureType::TEXTURE_2D) ? Renderer::getBlankTexture()->getVkImageInfo() : Renderer::getBlankCubemap()->getVkImageInfo()) : textures[i].texture->getVkImageInfo();

	//Go through each descriptor set (One per swap chain image)
	for (unsigned int i = 0; i < vulkanDescriptorSets.size(); ++i) {
		//Contains parameters for the write operations of the descriptor set
		std::vector<VkWriteDescriptorSet> descriptorWrites = {};

		//UBOs
		for (UBO* ubo : ubos)
			descriptorWrites.push_back(ubo->getVkWriteDescriptorSet(i, vulkanDescriptorSets[i], ubo->getVkBuffer(i)->getBufferInfo()));

		//Textures
		for (TextureBindingInfo& textureBindingInfo : textureBindings) {
			VkWriteDescriptorSet textureWrite;
			textureWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			textureWrite.dstSet           = vulkanDescriptorSets[i];
			textureWrite.dstBinding       = textureBindingInfo.binding;
			textureWrite.dstArrayElement  = 0;
			textureWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			textureWrite.descriptorCount  = textureBindingInfo.numTextures;
			textureWrite.pBufferInfo      = nullptr;
			textureWrite.pImageInfo       = textureBindingInfo.textures.data();
			textureWrite.pTexelBufferView = nullptr;
			textureWrite.pNext            = nullptr;

			descriptorWrites.push_back(textureWrite);
		}

		//Update the descriptor sets
		vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		//std::cout << textureBindings.size() << std::endl;
		//std::cout << reinterpret_cast<uint64_t&>(vulkanDescriptorSets[i]) << std::endl;
	}
}

void DescriptorSet::updateVk(unsigned int frame) {
	//Go through and update the image infos
	for (unsigned int i = 0; i < textures.size(); ++i)
		textureBindings[textures[i].bindingInfoIndex].textures[textures[i].locationInBinding] = (textures[i].texture == NULL) ? (textureBindings[textures[i].bindingInfoIndex].type == TextureType::TEXTURE_2D ? Renderer::getBlankTexture()->getVkImageInfo() : Renderer::getBlankCubemap()->getVkImageInfo()) : textures[i].texture->getVkImageInfo();

	//Contains parameters for the write operations of the descriptor set
	std::vector<VkWriteDescriptorSet> descriptorWrites = {};

	//UBOs
	for (UBO* ubo : ubos)
		descriptorWrites.push_back(ubo->getVkWriteDescriptorSet(frame, vulkanDescriptorSets[frame], ubo->getVkBuffer(frame)->getBufferInfo()));

	//Textures
	for (TextureBindingInfo& textureBindingInfo : textureBindings) {
		VkWriteDescriptorSet textureWrite;
		textureWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureWrite.dstSet           = vulkanDescriptorSets[frame];
		textureWrite.dstBinding       = textureBindingInfo.binding;
		textureWrite.dstArrayElement  = 0;
		textureWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureWrite.descriptorCount  = textureBindingInfo.numTextures;
		textureWrite.pBufferInfo      = nullptr;
		textureWrite.pImageInfo       = textureBindingInfo.textures.data();
		textureWrite.pTexelBufferView = nullptr;
		textureWrite.pNext            = nullptr;

		descriptorWrites.push_back(textureWrite);
	}

	//Update the descriptor sets
	vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DescriptorSet::updateVkRaytracing(VkAccelerationStructureKHR* raytracingTLAS, VkImageView raytracingOutputImage) {
	//Contains parameters for the write operations of the descriptor set
	std::vector<VkWriteDescriptorSet> descriptorWrites = {};

	//Information about the TLAS binding
	VkWriteDescriptorSetAccelerationStructureKHR descriptorAccelerationStructureInfo{};
	descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
	descriptorAccelerationStructureInfo.pAccelerationStructures = raytracingTLAS;

	//Write descriptor set description, needs to use pNext to link the above
	VkWriteDescriptorSet asWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	asWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	asWrite.dstSet          = vulkanDescriptorSets[0];
	asWrite.dstBinding      = 0; //Should match layout
	asWrite.descriptorCount = 1;
	asWrite.pNext           = &descriptorAccelerationStructureInfo;

	descriptorWrites.push_back(asWrite);

	//Information about the storage image
	VkDescriptorImageInfo storageImageDescriptor{};
	storageImageDescriptor.imageView = raytracingOutputImage;
	storageImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	//Write descriptor set for the storage image
	VkWriteDescriptorSet textureWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	textureWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	textureWrite.dstSet           = vulkanDescriptorSets[0];
	textureWrite.dstBinding       = 1; //Should match layout
	textureWrite.dstArrayElement  = 0;
	textureWrite.descriptorCount  = 1;
	textureWrite.pBufferInfo      = nullptr;
	textureWrite.pImageInfo       = &storageImageDescriptor;
	textureWrite.pTexelBufferView = nullptr;
	textureWrite.pNext            = nullptr;

	descriptorWrites.push_back(textureWrite);

	//Update the descriptor sets
	vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DescriptorSet::update() {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		m_isInUpdateQueue = true;
		//Update this descriptor set
		Vulkan::updateDescriptorSet(this);
	}
}

void DescriptorSet::bind() {
	//Check if using Vulkan or not
	if (BaseEngine::usingVulkan())
		//Bind the descriptor set for Vulkan
		vkCmdBindDescriptorSets(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::getCurrentGraphicsPipeline()->getLayout()->getVkInstance(), layout->getSetNumber(), 1, &vulkanDescriptorSets[Vulkan::getCurrentFrame()], 0, nullptr);
	else {
		//Need to bind the appropriate UBOs and textures for OpenGL

		//UBOs
		for (UBO* ubo : ubos)
			//Bind the UBO for use
			ubo->bindGL();

		//Textures
		for (TextureInfo& info : textures) {
			if (info.texture != NULL) {
				glActiveTexture(GL_TEXTURE0 + info.binding);
				info.texture->bind();
			}
		}
	}
}

void DescriptorSet::bind(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout) {
	//Check if using Vulkan or not
	if (BaseEngine::usingVulkan())
		//Bind the descriptor set for Vulkan
		vkCmdBindDescriptorSets(Vulkan::getCurrentCommandBuffer(), bindPoint, pipelineLayout, layout->getSetNumber(), 1, raytracing ? &vulkanDescriptorSets[0] : &vulkanDescriptorSets[Vulkan::getCurrentFrame()], 0, nullptr);
}

void DescriptorSet::unbind() {
	//Ensure not using Vulkan
	if (! BaseEngine::usingVulkan()) {
		//Unbind the textures
		for (TextureInfo& info : textures) {
			if (info.texture != NULL) {
				glActiveTexture(GL_TEXTURE0 + info.binding);
				info.texture->unbind();
			}
		}
	}
}

/*****************************************************************************
 * The DescriptorSetLayout class
 *****************************************************************************/

DescriptorSetLayout::~DescriptorSetLayout() {
	//Destroy the layout
	if (vulkanDescriptorSetLayout != VK_NULL_HANDLE)
		vkDestroyDescriptorSetLayout(Vulkan::getDevice()->getLogical(), vulkanDescriptorSetLayout, nullptr);
}

void DescriptorSetLayout::setupVk() {
	//Bindings within the descriptor set
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	//Go through the required UBOs
	for (unsigned int i = 0; i < ubos.size(); ++i) {
		//Setup the binding and add it
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding            = ubos[i].binding + UBO::VULKAN_BINDING_OFFSET; //Apply offset for Vulkan (Caused access violation without)
		uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount    = 1;
		if (Window::getCurrentInstance()->getSettings().videoRaytracing)
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		else
			uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_ALL_GRAPHICS; //VK_SHADER_STAGE_ALL_GRAPHICS
		uboLayoutBinding.pImmutableSamplers = nullptr; //Optional

		bindings.push_back(uboLayoutBinding);
	}

	//Go through the required textures
	for (unsigned int i = 0; i < textureBindings.size(); ++i) {
		//Setup the binding and add it
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding            = textureBindings[i].binding;
		samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount    = textureBindings[i].numTextures;
		samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_ALL_GRAPHICS; //Should only use whats necessary, but for terrain need access in vertex shader as well
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		bindings.push_back(samplerLayoutBinding);
	}

	if (raytracing) {
		//Setup the accleration structure binding
		VkDescriptorSetLayoutBinding asBinding = {};
		asBinding.binding            = 0;
		asBinding.descriptorType     = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		asBinding.descriptorCount    = 1;
		asBinding.stageFlags         = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		asBinding.pImmutableSamplers = nullptr; //Optional

		bindings.push_back(asBinding);

		//Setup the output image binding
		VkDescriptorSetLayoutBinding outputLayoutBinding = {};
		outputLayoutBinding.binding            = 1;
		outputLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		outputLayoutBinding.descriptorCount    = 1;
		outputLayoutBinding.stageFlags         = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		outputLayoutBinding.pImmutableSamplers = nullptr;

		bindings.push_back(outputLayoutBinding);
	}

	//Create info for the descriptor set layout
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings    = bindings.data();
	layoutInfo.pNext        = nullptr;
	layoutInfo.flags        = 0;

	//Attempt to create the descriptor set layout
	if (vkCreateDescriptorSetLayout(Vulkan::getDevice()->getLogical(), &layoutInfo, nullptr, &vulkanDescriptorSetLayout) != VK_SUCCESS)
		Logger::log("Failed to create Vulkan descriptor set layout", "DescriptorSetLayout", LogType::Error);
}

void DescriptorSetLayout::setup() {
	if (BaseEngine::usingVulkan())
		setupVk();
}