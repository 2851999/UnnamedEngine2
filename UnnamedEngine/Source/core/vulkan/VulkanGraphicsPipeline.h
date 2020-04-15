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

#pragma once

#include "../Window.h"
#include "VulkanBuffer.h"

#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"

#include "../render/VBO.h"

/*****************************************************************************
 * The VulkanGraphicsPipeline class handles a graphics pipeline in Vulkan
 *****************************************************************************/

class RenderData;

class VulkanGraphicsPipeline {
private:
	/* The swap chain for this pipeline */
	VulkanSwapChain* swapChain;

	/* The layout and pipeline instance */
	VkPipelineLayout pipelineLayout;
	VkPipeline       pipeline;
public:
	/* Constructor */
	VulkanGraphicsPipeline(VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, RenderData* renderData, RenderShader* renderShader);

	/* Destructor */
	virtual ~VulkanGraphicsPipeline();

    /* Method used to bind this pipline for use */
    void bind();

	/* Getters */
	VkPipeline& getInstance() { return pipeline; }
	VkPipelineLayout& getLayout() { return pipelineLayout; }
};


