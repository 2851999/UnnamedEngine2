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
#include "SSBO.h"
#include "Renderer.h"
#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"
#include "../../utils/VulkanUtils.h"

 /*****************************************************************************
  * The DescriptorSet class
  *****************************************************************************/

DescriptorSet::DescriptorSet(DescriptorSetLayout* layout, bool raytracing) : layout(layout), raytracing(raytracing) {
	//Obtain the UBO and texture info required from the layout
	std::vector<DescriptorSetLayout::BufferInfo> shaderBuffersInfo = layout->getShaderBuffers();
	textureBindings = layout->getTextureBindings();
	std::vector<DescriptorSetLayout::ASInfo> asBindingsVector = layout->getAccelerationStructureBindings();

	//Add the required UBOs, SSBOs and textures
	for (DescriptorSetLayout::BufferInfo& bufferInfo : shaderBuffersInfo) {
		ShaderBuffer* shaderBuffer = NULL;

		if (bufferInfo.type == ShaderBuffer::Type::UBO)
			shaderBuffer = new UBO(NULL, bufferInfo.size, bufferInfo.usage, bufferInfo.binding);
		else if (bufferInfo.type == ShaderBuffer::Type::SSBO)
			shaderBuffer = new SSBO(NULL, bufferInfo.size, bufferInfo.usage, bufferInfo.binding);

		shaderBuffers.push_back(shaderBuffer);
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

	asBindings.resize(asBindingsVector.size());
	for (unsigned int i = 0; i < asBindings.size(); ++i) {
		asBindings[i].binding = asBindingsVector[i].binding;
		asBindings[i].accelerationStructure = VK_NULL_HANDLE;
	}
}

DescriptorSet::~DescriptorSet() {
	//Destory Vulkan data
	if (vulkanDescriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(Vulkan::getDevice()->getLogical(), vulkanDescriptorPool, nullptr);
	if (m_isInUpdateQueue)
		Vulkan::removeFromDescriptorSetQueue(this);
	for (ShaderBuffer* shaderBuffer : shaderBuffers)
		delete shaderBuffer;
	shaderBuffers.clear();
}

void DescriptorSet::setupVk() {
	//Obtain the number of needed descriptor sets required
	unsigned int numSwapChainImages = Vulkan::getSwapChain()->getImageCount();
	unsigned int numDescriptorSets = numSwapChainImages;

	//-------------------------------- CREATE DESCRIPTOR POOL --------------------------------

	//Determine the number of descriptors that will need to be allocated in the pool
	std::vector<VkDescriptorPoolSize> poolSizes = {};

	for (unsigned int i = 0; i < shaderBuffers.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type            = shaderBuffers[i]->getVkDescriptorType();
		poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
		poolSizes.push_back(poolSize);
	}

	for (unsigned int i = 0; i < textures.size(); ++i) {
		if (textureBindings[i].type == TextureType::STORAGE_IMAGE) {
			VkDescriptorPoolSize poolSize;
			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
			poolSizes.push_back(poolSize);
		} else {
			VkDescriptorPoolSize poolSize;
			poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			poolSize.descriptorCount = static_cast<uint32_t>(1);
			poolSizes.push_back(poolSize);
		}
	}

	for (unsigned int i = 0; i < asBindings.size(); ++i) {
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize.descriptorCount = static_cast<uint32_t>(1);
		poolSizes.push_back(poolSize);
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

		//ShaderBuffer's
		for (ShaderBuffer* shaderBuffer : shaderBuffers)
			descriptorWrites.push_back(shaderBuffer->getVkWriteDescriptorSet(i, vulkanDescriptorSets[i], shaderBuffer->getVkBuffer(i)->getBufferInfo()));

		//Textures
		for (TextureBindingInfo& textureBindingInfo : textureBindings) {
			if (textureBindingInfo.type == TextureType::STORAGE_IMAGE) {
				//Information about the storage image
				VkDescriptorImageInfo storageImageDescriptor{};
				storageImageDescriptor.imageView = textureBindingInfo.textures[0].imageView;
				storageImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

				//Write descriptor set for the storage image
				VkWriteDescriptorSet textureWrite = utils_vulkan::initWriteDescriptorSet(
					VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
					vulkanDescriptorSets[0],
					1 //dstBinding should match layout
				);
				textureWrite.descriptorCount = 1;
				textureWrite.pImageInfo = &storageImageDescriptor;

				descriptorWrites.push_back(textureWrite);
			} else {
				VkWriteDescriptorSet textureWrite = utils_vulkan::initWriteDescriptorSet(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, vulkanDescriptorSets[i], textureBindingInfo.binding);
				textureWrite.descriptorCount = textureBindingInfo.numTextures;
				textureWrite.pImageInfo = textureBindingInfo.textures.data();

				descriptorWrites.push_back(textureWrite);
			}
		}

		//Acceleration structures
		for (AccelerationStructureInfo& asInfo : asBindings) {
			//Information about the TLAS binding
			VkWriteDescriptorSetAccelerationStructureKHR descriptorAccelerationStructureInfo{};
			descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
			descriptorAccelerationStructureInfo.pAccelerationStructures = asInfo.accelerationStructure;

			//Write descriptor set description, needs to use pNext to link the above
			VkWriteDescriptorSet asWrite = utils_vulkan::initWriteDescriptorSet(
				VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
				vulkanDescriptorSets[0],
				asInfo.binding //dstBinding should match layout
			);
			asWrite.descriptorCount = 1;
			asWrite.pNext = &descriptorAccelerationStructureInfo;

			descriptorWrites.push_back(asWrite);
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

	//ShaderBuffer's
	for (ShaderBuffer* shaderBuffer : shaderBuffers)
		descriptorWrites.push_back(shaderBuffer->getVkWriteDescriptorSet(frame, vulkanDescriptorSets[frame], shaderBuffer->getVkBuffer(frame)->getBufferInfo()));

	//Textures
	for (TextureBindingInfo& textureBindingInfo : textureBindings) {
		if (textureBindingInfo.type == TextureType::STORAGE_IMAGE) {
			//Information about the storage image
			VkDescriptorImageInfo storageImageDescriptor{};
			storageImageDescriptor.imageView = textureBindingInfo.textures[0].imageView;;
			storageImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			//Write descriptor set for the storage image
			VkWriteDescriptorSet textureWrite = utils_vulkan::initWriteDescriptorSet(
				VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				vulkanDescriptorSets[0],
				1 //dstBinding should match layout
			);
			textureWrite.descriptorCount = 1;
			textureWrite.pImageInfo      = &storageImageDescriptor;

			descriptorWrites.push_back(textureWrite);
		} else {
			VkDescriptorType descriptorType = textureBindingInfo.type == TextureType::STORAGE_IMAGE ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			VkWriteDescriptorSet textureWrite = utils_vulkan::initWriteDescriptorSet(descriptorType, vulkanDescriptorSets[frame], textureBindingInfo.binding);
			textureWrite.descriptorCount = textureBindingInfo.numTextures;
			textureWrite.pImageInfo = textureBindingInfo.textures.data();

			descriptorWrites.push_back(textureWrite);
		}
	}

	//Acceleration structures
	for (AccelerationStructureInfo& asInfo : asBindings) {
		//Information about the TLAS binding
		VkWriteDescriptorSetAccelerationStructureKHR descriptorAccelerationStructureInfo{};
		descriptorAccelerationStructureInfo.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
		descriptorAccelerationStructureInfo.pAccelerationStructures    = asInfo.accelerationStructure;

		//Write descriptor set description, needs to use pNext to link the above
		VkWriteDescriptorSet asWrite = utils_vulkan::initWriteDescriptorSet(
			VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
			vulkanDescriptorSets[0],
			asInfo.binding //dstBinding should match layout
		);
		asWrite.descriptorCount = 1;
		asWrite.pNext           = &descriptorAccelerationStructureInfo;

		descriptorWrites.push_back(asWrite);
	}

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
		//Need to bind the appropriate UBOs, SSBOs and textures for OpenGL

		//ShaderBuffer's
		for (ShaderBuffer* shaderBuffer : shaderBuffers)
			//Bind the UBO for use
			shaderBuffer->bindGL();

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

	//Go through the required ShaderBuffer's
	for (unsigned int i = 0; i < shaderBuffers.size(); ++i) {
		//Setup the binding and add it
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = shaderBuffers[i].binding + ShaderBuffer::VULKAN_BINDING_OFFSET; //Apply offset for Vulkan (Caused access violation without)
		layoutBinding.descriptorType = ShaderBuffer::convertToVkDescriptorType(shaderBuffers[i].type);
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = shaderBuffers[i].shaderStageFlags;
		layoutBinding.pImmutableSamplers = nullptr; //Optional

		bindings.push_back(layoutBinding);
	}

	//Go through the required textures
	for (unsigned int i = 0; i < textureBindings.size(); ++i) {
		VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		VkPipelineStageFlags stageFlags = textureBindings[i].shaderStageFlags;

		if (textureBindings[i].type == DescriptorSet::TextureType::STORAGE_IMAGE)
			descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		//Setup the binding and add it
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding            = textureBindings[i].binding;
		samplerLayoutBinding.descriptorType     = descriptorType;
		samplerLayoutBinding.descriptorCount    = textureBindings[i].numTextures;
		samplerLayoutBinding.stageFlags         = stageFlags; //Should only use whats necessary, but for terrain need access in vertex shader as well
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		bindings.push_back(samplerLayoutBinding);
	}

	for (unsigned int i = 0; i < asBindings.size(); ++i) {
		VkDescriptorSetLayoutBinding asBinding = {};
		asBinding.binding            = asBindings[i].binding;
		asBinding.descriptorType     = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		asBinding.descriptorCount    = 1;
		asBinding.stageFlags         = asBindings[i].shaderStageFlags;
		asBinding.pImmutableSamplers = nullptr;

		bindings.push_back(asBinding);
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