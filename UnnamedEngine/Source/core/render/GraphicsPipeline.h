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

class GraphicsPipelineLayout;

 /*****************************************************************************
  * The GraphicsPipeline class responsible for handleing Vulkan pipelines and to
  * produce similar behaviour in OpenGL
  *****************************************************************************/

class GraphicsPipeline {
public:
	/* Various generalised blend states */
	enum class BlendFactor {
		ZERO, ONE, SRC_ALPHA, ONE_MINUS_SRC_ALPHA
	};

	/* Varuous generalised compare operations */
	enum class CompareOperation {
		LESS, EQUAL, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL
	};

	/* Structure containing information about the depth state to be used */
	struct DepthState {
		bool depthTestEnable  = true;
		bool depthWriteEnable = true;

		CompareOperation depthCompareOp = CompareOperation::LESS_OR_EQUAL;
	};

	/* Structure containing information about the colour blend state to be used */
	struct ColourBlendState {
		bool blendEnabled = false;

		BlendFactor srcRGB = BlendFactor::ONE;
		BlendFactor dstRGB = BlendFactor::ZERO;
		BlendFactor srcAlpha = BlendFactor::ONE;
		BlendFactor dstAlpha = BlendFactor::ZERO;
	};
private:
	/* The layout of this pipeline */
	GraphicsPipelineLayout* layout;

	/* The shader used with this pipeline */
	RenderShader* renderShader;

	/* The pipeline instance (For Vulkan) */
	VkPipeline vulkanPipeline = VK_NULL_HANDLE;

	/* The colour blend state to use with this pipeline */
	ColourBlendState colourBlendState;

	/* The depth state to use with this pipeline */
	DepthState depthState;
public:
	/* Structure used to store data required for creating a render pipeline */
	struct VertexInputData {
		std::vector<VkVertexInputBindingDescription>   bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
	};

	/* Constructor */
	GraphicsPipeline(GraphicsPipelineLayout* layout);

	/* Destructor */
	virtual ~GraphicsPipeline();

	/* Method used to bind this pipeline for rendering */
	void bind();

	/* Methods used to convert the a generalised states to the one required by
	   OpenGL/Vulkan */
	GLenum convertToGL(BlendFactor factor);
	VkBlendFactor convertToVk(BlendFactor factor);
	GLenum convertToGL(CompareOperation op);
	VkCompareOp convertToVk(CompareOperation op);

	/* Getters */
	inline GraphicsPipelineLayout* getLayout() { return layout; }
	inline VkPipeline& getVkInstance() { return vulkanPipeline; }
};

/*****************************************************************************
 * The GraphicsPipelineLayout class responsible for handleing the layout of a
 * GraphicsPipeline
 *****************************************************************************/

class GraphicsPipelineLayout {
private:
	/* The pipeline layout instance (For Vulkan) */
	VkPipelineLayout vulkanPipelineLayout = VK_NULL_HANDLE;

	/* The shader used with this pipeline */
	RenderShader* renderShader;

	/* The vertex input data for the pipeline */
	GraphicsPipeline::VertexInputData vertexInputData;

	/* The colour and depth states for the pipeline */
	GraphicsPipeline::ColourBlendState colourBlendState;
	GraphicsPipeline::DepthState       depthState;
public:
	/* Constructor */
	GraphicsPipelineLayout(RenderShader* renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState);

	/* Destructor */
	virtual ~GraphicsPipelineLayout();

	/* Getters */
	inline VkPipelineLayout& getVkInstance() { return vulkanPipelineLayout; }
	inline RenderShader* getRenderShader() { return renderShader; }
	inline GraphicsPipeline::VertexInputData& getVertexInputData() { return vertexInputData; }
	inline GraphicsPipeline::ColourBlendState& getColourBlendState() { return colourBlendState; }
	inline GraphicsPipeline::DepthState& getDepthState() { return depthState; }
};