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

#include "VulkanSwapChain.h"

/*****************************************************************************
 * The VulkanRenderPass class handles a render pass in Vulkan
 *****************************************************************************/

class VulkanRenderPass {
private:
	/* The render pass instance */
	VkRenderPass instance;

	/* The device the render pass is for  */
	VulkanDevice* device;

	/* Framebuffers for rendering to the swap chain */
	std::vector<VkFramebuffer> swapChainFramebuffers;
public:
	/* Constructor */
	VulkanRenderPass(VulkanSwapChain* swapChain);

	/* Destructor */
	virtual ~VulkanRenderPass();

	/* Method to begin this render pass */
	void begin();

	/* Method to end this render pass */
	void end();

	/* Setters and getters */
	VkRenderPass& getInstance() { return instance; }
	std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; }
};

