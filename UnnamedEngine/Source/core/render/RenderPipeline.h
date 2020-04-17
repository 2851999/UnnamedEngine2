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

#include "DescriptorSet.h"
#include "RenderShader.h"

class RenderPipelineLayout;

 /*****************************************************************************
  * The RenderPipeline class responsible for handleing Vulkan pipelines and to
  * produce similar behaviour in OpenGL
  *****************************************************************************/

class RenderPipeline {
private:
	/* The layout of this pipeline */
	RenderPipelineLayout* layout;

	/* The shader used with this pipeline */
	RenderShader* renderShader;

	/* The pipeline instance (For Vulkan) */
	VkPipeline vulkanPipeline = VK_NULL_HANDLE;
public:
	/* Structure used to store data required for creating a render pipeline */
	struct VertexInputData {
		std::vector<VkVertexInputBindingDescription>   bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
	};

	/* Constructor */
	RenderPipeline(RenderShader* renderShader, VertexInputData vertexInputData);

	/* Destructor */
	virtual ~RenderPipeline();

	/* Method used to bind this pipeline for rendering */
	void bind();

	/* Getters */
	inline RenderPipelineLayout* getLayout() { return layout; }
	inline VkPipeline& getVkInstance() { return vulkanPipeline; }
};

/*****************************************************************************
 * The RenderPipelineLayout class responsible for handleing the layout of a
 * RenderPipeline
 *****************************************************************************/

class RenderPipelineLayout {
private:
	/* The pipeline layout instance (For Vulkan) */
	VkPipelineLayout vulkanPipelineLayout = VK_NULL_HANDLE;
public:
	/* Constructor */
	RenderPipelineLayout();

	/* Destructor */
	virtual ~RenderPipelineLayout();

	/* Method used to setup this layout */
	void setup(RenderShader* renderShader);

	/* Getters */
	inline VkPipelineLayout& getVkInstance() { return vulkanPipelineLayout; }
};