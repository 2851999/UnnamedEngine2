/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
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

#include "RenderData.h"

#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

#include "Renderer.h"

/*****************************************************************************
 * The RenderData class
 *****************************************************************************/

RenderData::~RenderData() {
	for (unsigned int i = 0; i < textureSets.size(); ++i)
		delete textureSets[i];
	if (descriptorPool != VK_NULL_HANDLE) {
		delete graphicsVkPipeline;
		vkDestroyDescriptorSetLayout(Vulkan::getDevice()->getLogical(), descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(Vulkan::getDevice()->getLogical(), descriptorPool, nullptr);
	}
}

void RenderData::setup(RenderShader* renderShader) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Generate the VAO and bind it
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	} else {
		vboVkInstances.resize(vbosFloat.size());
		vboVkOffsets.resize(vbosFloat.size());
	}

	//Go through each VBO and set it up
	for (unsigned int i = 0; i < vbosFloat.size(); i++) {
		vbosFloat[i]->setup();
		vbosFloat[i]->startRendering();

		if (Window::getCurrentInstance()->getSettings().videoVulkan) {
			vboVkInstances[i] = vbosFloat[i]->getVkBuffer()->getInstance();
			vboVkOffsets[i] = 0; //Is this right offset???
		}
	}

	for (unsigned int i = 0; i < vbosUInteger.size(); i++) {
		vbosUInteger[i]->setup();
		vbosUInteger[i]->startRendering();
	}

	//Now setup the indices VBO if assigned
	if (vboIndices) {
		vboIndices->setup();
		vboIndices->startRendering();
	}

	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(0);
	else {
		//Add the required textures from the texture set's (if there is one)
		if (textureSets.size() > 0) {
			//Just use the first one as the example (should all have same textures here anyway)
			for (TextureSet::TextureInfo& info : textureSets[0]->getTextureInfos())
				textureBindings.push_back(info.binding);
		}

		numSwapChainImages = Vulkan::getSwapChain()->getImageCount();
//		unsigned int numUBODescriptors   = numSwapChainImages * ubos.size();
//		unsigned int numImageDescriptors = numSwapChainImages * textureSets.size();
		unsigned int numDescriptorSets   = numSwapChainImages * (textureSets.size() == 0 ? 1 : textureSets.size());

		//---------------------------CREATE DESCRIPTOR POOL---------------------------
		//Assign the creation info
		std::vector<VkDescriptorPoolSize> poolSizes = {};
		for (unsigned int i = 0; i < ubos.size(); ++i) {
			VkDescriptorPoolSize poolSize;
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
			poolSizes.push_back(poolSize);
		}
		for (unsigned int i = 0; i < textureBindings.size(); ++i) {
			VkDescriptorPoolSize poolSize;
			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount = static_cast<uint32_t>(numSwapChainImages);
			poolSizes.push_back(poolSize);
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes    = poolSizes.data();
		poolInfo.maxSets       = static_cast<uint32_t>(numDescriptorSets);
		poolInfo.flags         = 0;

		//Attempt to create the pool
		if (vkCreateDescriptorPool(Vulkan::getDevice()->getLogical(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			Logger::log("Failed to create descriptor pool", "RenderData", LogType::Error);

		//---------------------------CREATE DESCRIPTOR SET LAYOUT---------------------------
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		//Go through the UBO's
		for (auto& ubo : ubos) {
			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding            = ubo.second->getBinding();
			uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount    = 1;
			uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_ALL_GRAPHICS; //VK_SHADER_STAGE_ALL_GRAPHICS
			uboLayoutBinding.pImmutableSamplers = nullptr; //Optional

			//Add the binding
			bindings.push_back(uboLayoutBinding);
		}

		//Go through the textures
		for (unsigned int textureBinding : textureBindings) {
			VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
			samplerLayoutBinding.binding            = textureBinding;
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
			Logger::log("Failed to create descriptor set layout", "RenderData", LogType::Error);

		//---------------------------CREATE DESCRIPTOR SETS---------------------------
		std::vector<VkDescriptorSetLayout> layouts(numDescriptorSets, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool     = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(numDescriptorSets);
		allocInfo.pSetLayouts        = layouts.data();

		descriptorSets.resize(numDescriptorSets);
		if (vkAllocateDescriptorSets(Vulkan::getDevice()->getLogical(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
			Logger::log("Failed to allocate descriptor sets", "RenderData", LogType::Error);

		//Setup the pipeline
		graphicsVkPipeline = new VulkanGraphicsPipeline(Vulkan::getSwapChain(), vbosFloat[0], Vulkan::getRenderPass(), this, renderShader);

		//Assign the descriptor write info
		setupVulkan(renderShader);
	}
}

void RenderData::setupVulkan(RenderShader* renderShader) {
	//Setup the descriptor set write's
	if (Window::getCurrentInstance()->getSettings().videoVulkan) {
		numSwapChainImages = Vulkan::getSwapChain()->getImageCount();

		//Allows writing of each UBO and texture
		if (textureSets.size() == 0) {
			for (unsigned int i = 0; i < numSwapChainImages; ++i) {
				std::vector<VkWriteDescriptorSet> descriptorWrites = {};

				for (auto& ubo : ubos)
					descriptorWrites.push_back(ubo.second->getVkWriteDescriptorSet(i, descriptorSets[i], ubo.second->getVkBuffer(i)->getBufferInfo()));

				vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		} else {
			for (unsigned int x = 0; x < numSwapChainImages; ++x) {
				for (unsigned int y = 0; y < textureSets.size(); ++y) {
					unsigned int i = (y * numSwapChainImages) + x;
					std::vector<VkWriteDescriptorSet> descriptorWrites = {};
					for (auto& ubo : ubos)
						descriptorWrites.push_back(ubo.second->getVkWriteDescriptorSet(x, descriptorSets[i], ubo.second->getVkBuffer(x)->getBufferInfo()));

					for (TextureSet::TextureInfo& textureInfo : textureSets[y]->getTextureInfos()) {
						//Only assign if have an actual texture (allows textures to be assigned later)
						//if (textureInfo.texture != NULL) {
							VkWriteDescriptorSet textureWrite;
							textureWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							textureWrite.dstSet           = descriptorSets[i];
							textureWrite.dstBinding       = textureInfo.binding;
							textureWrite.dstArrayElement  = 0;
							textureWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							textureWrite.descriptorCount  = 1;
							textureWrite.pBufferInfo      = nullptr;
							textureWrite.pImageInfo       = textureInfo.texture != NULL ? textureInfo.texture->getVkImageInfo() : Renderer::getBlankTexture()->getVkImageInfo();
							textureWrite.pTexelBufferView = nullptr;
							textureWrite.pNext            = nullptr;
							descriptorWrites.push_back(textureWrite);
						//}
					}
					vkUpdateDescriptorSets(Vulkan::getDevice()->getLogical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
				}
			}
		}
	}
}

void RenderData::add(Texture* texture, unsigned int binding) {
	textureBindings.push_back(binding);
	for (unsigned int i = 0; i < textureSets.size(); ++i)
		textureSets[i]->add(binding, texture);
}

void RenderData::bindBuffers() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(vao);
	else {
		vkCmdBindVertexBuffers(Vulkan::getCurrentCommandBuffer(), 0, 1, vboVkInstances.data(), vboVkOffsets.data());
		vkCmdBindIndexBuffer(Vulkan::getCurrentCommandBuffer(), vboIndices->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit
	}
}
void RenderData::unbindBuffers() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(0);
}

void RenderData::renderWithoutBinding() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Check for instancing
		if (primcount > 0) {
			//Check for indices
			if (vboIndices)
				glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, (void*) NULL, primcount);
			else
				glDrawArraysInstanced(mode, 0, count, primcount);
		} else if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) NULL);
			else
				glDrawArrays(mode, 0, count);
		}
	} else {
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				vkCmdDrawIndexed(Vulkan::getCurrentCommandBuffer(), count, 1, 0, 0, 0);
			else
				vkCmdDraw(Vulkan::getCurrentCommandBuffer(), count, 1, 0, 0);
		}
	}
}

void RenderData::renderBaseVertex(unsigned int count, unsigned int indicesOffset, unsigned int baseVertex) {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Check for instancing
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				glDrawElementsBaseVertex(mode, count, GL_UNSIGNED_INT, (void*) (indicesOffset * sizeof(unsigned int)), baseVertex); //Assume indices stored as unsigned integers
		}
	} else {
		//Check for instancing
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				vkCmdDrawIndexed(Vulkan::getCurrentCommandBuffer(), count, 1, indicesOffset, baseVertex, 0);
		}
	}
}

UBO* RenderData::getUBO(unsigned int id) {
	if (ubos.count(id) > 0)
		return ubos[id];
	else {
//		Logger::log("Could not find UBO with id " + utils_string::str(id) + ". Was it added?", "RenderData", LogType::Debug);
		return NULL;
	}
}

const VkDescriptorSet* RenderData::getVkDescriptorSet(unsigned int materialIndex) {
	//Return the descriptor set for the current frame and requested material index
	//Note: Have one texture set per material, organised sequentially for each frame
	return &descriptorSets[(materialIndex * Vulkan::getSwapChain()->getImageCount()) + Vulkan::getCurrentFrame()];

}
