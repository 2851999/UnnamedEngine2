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

#ifndef CORE_VULKAN_VULKANGRAPHICSPIPELINE_H_
#define CORE_VULKAN_VULKANGRAPHICSPIPELINE_H_

#include "../Window.h"
#include "VulkanBuffer.h"

#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"

#include "../render/VBO.h"

/*****************************************************************************
 * The VulkanGraphicsPipeline class handles a graphics pipeline in Vulkan
 *****************************************************************************/

class VulkanGraphicsPipeline {
private:
	/* The swap chain for this pipeline */
	VulkanSwapChain* swapChain;

	/* The layout and pipeline instance */
	VkPipelineLayout pipelineLayout;
	VkPipeline       pipeline;

	/* Method to read a file */
	static std::vector<char> readFile(const std::string& fileName);

	/* Method to create a shader module */
	static VkShaderModule createShaderModule(VulkanDevice* device, const std::vector<char>& code);
public:
	/* Constructor */
	VulkanGraphicsPipeline(VulkanSwapChain* swapChain, VBO<float>* vertexBuffer, VulkanRenderPass* renderPass, VkDescriptorSetLayout& descriptorSetLayout);

	/* Destructor */
	virtual ~VulkanGraphicsPipeline();

	/* Getters */
	VkPipeline& getInstance() { return pipeline; }
	VkPipelineLayout& getLayout() { return pipelineLayout; }
};


#endif /* CORE_VULKAN_VULKANGRAPHICSPIPELINE_H_ */
