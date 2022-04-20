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

#pragma once

#include <vulkan/vulkan.h>

/*****************************************************************************
 * Various Vulkan utilities
 *****************************************************************************/

namespace utils_vulkan {
	inline VkVertexInputBindingDescription initVertexInputBindings(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
		VkVertexInputBindingDescription description;
		description.binding   = binding;
		description.stride    = stride;
		description.inputRate = inputRate;
		return description;
	}

	inline VkVertexInputAttributeDescription initVertexAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
		VkVertexInputAttributeDescription description;
		description.location = location;
		description.binding  = binding;
		description.format   = format;
		description.offset   = offset;
		return description;
	}

	inline VkPipelineShaderStageCreateInfo initPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module, const char* pName) {
		VkPipelineShaderStageCreateInfo createInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		createInfo.stage  = stage;
		createInfo.module = module;
		createInfo.pName  = pName;
		return createInfo;
	}

	/* Returns a partially completed VkWriteDescriptorSet structure (missing pBufferInfo/pImageInfo/pNext and descriptorCount) */
	inline VkWriteDescriptorSet initWriteDescriptorSet(VkDescriptorType descriptorType, VkDescriptorSet dstSet, uint32_t dstBinding) {
		VkWriteDescriptorSet writeDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		writeDescriptorSet.descriptorType = descriptorType;
		writeDescriptorSet.dstSet         = dstSet;
		writeDescriptorSet.dstBinding     = dstBinding;
		return writeDescriptorSet;
	}
}