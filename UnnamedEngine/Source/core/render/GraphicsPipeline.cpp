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

#include "GraphicsPipeline.h"

#include "RenderPass.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"
#include "../BaseEngine.h"
#include "../../utils/VulkanUtils.h"

 /*****************************************************************************
  * The GraphicsPipeline class
  *****************************************************************************/

GraphicsPipeline::GraphicsPipeline(GraphicsPipelineLayout* layout, RenderPass* renderPass) : layout(layout), renderShader(layout->getRenderShader()), colourBlendState(layout->getColourBlendState()), depthState(layout->getDepthState()) {
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = renderShader->getShader()->getVkVertexShaderModule();
		vertShaderStageInfo.pName = "main"; //Entry point

		//pSpecializationInfo can be used to specify values for shader constants - faster than using if statements
		//default set to nullptr

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = renderShader->getShader()->getVkFragmentShaderModule();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		GraphicsPipeline::VertexInputData vertexInputData = layout->getVertexInputData();

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputData.bindings.size());
		vertexInputInfo.pVertexBindingDescriptions = vertexInputData.bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputData.attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputData.attributes.data();

		//	vertexInputInfo.vertexBindingDescriptionCount = 0;
		//	vertexInputInfo.pVertexBindingDescriptions = nullptr;
		//	vertexInputInfo.vertexAttributeDescriptionCount = 0;
		//	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		if (renderShader->getID() != Renderer::SHADER_FRAMEBUFFER) {
			viewport.x = 0.0f;
			viewport.y = (float) Window::getCurrentInstance()->getSettings().windowHeight;
			viewport.width = (float) Window::getCurrentInstance()->getSettings().windowWidth;
			viewport.height = -((float) viewport.y); //Flip so that it resembles OpenGL
		} else {
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float) Window::getCurrentInstance()->getSettings().windowWidth;
			viewport.height = (float) Window::getCurrentInstance()->getSettings().windowHeight;
		}
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = Vulkan::getSwapChain()->getExtent();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE; //If true fragments clamped rather than discarded, requires GPU feature
		rasterizer.rasterizerDiscardEnable = VK_FALSE; //If true discards everything, wouldn't render to frame buffer
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //Anything else requires GPU feature
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE; //VK_CULL_MODE_BACK_BIT
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; //Optional
		rasterizer.depthBiasClamp = 0.0f; //Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; //Optional

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(renderPass->getNumSamples() == 0 ? 1 : renderPass->getNumSamples());
		multisampling.minSampleShading = 1.0f; //Optional
		multisampling.pSampleMask = nullptr; //Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
		multisampling.alphaToOneEnable = VK_FALSE; //Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthState.depthTestEnable ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = depthState.depthWriteEnable ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = convertToVk(depthState.depthCompareOp);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; //Optional
		depthStencil.maxDepthBounds = 1.0f; //Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; //Optional
		depthStencil.back = {}; //Optional

		//Per framebuffer (only have one here)
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = colourBlendState.blendEnabled ? VK_TRUE : VK_FALSE;

		if (colourBlendState.blendEnabled) {
			colorBlendAttachment.srcColorBlendFactor = convertToVk(colourBlendState.srcRGB);
			colorBlendAttachment.dstColorBlendFactor = convertToVk(colourBlendState.dstRGB);
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = convertToVk(colourBlendState.srcAlpha);
			colorBlendAttachment.dstAlphaBlendFactor = convertToVk(colourBlendState.srcAlpha);
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; //Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; //Optional
		colorBlending.blendConstants[1] = 0.0f; //Optional
		colorBlending.blendConstants[2] = 0.0f; //Optional
		colorBlending.blendConstants[3] = 0.0f; //Optional

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; //Optional
		pipelineInfo.layout = layout->getVkInstance();
		pipelineInfo.renderPass = renderPass->getVkInstance();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //Optional
		pipelineInfo.basePipelineIndex = -1; //Optional

		if (vkCreateGraphicsPipelines(Vulkan::getDevice()->getLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vulkanPipeline) != VK_SUCCESS)
			Logger::log("Failed to create graphics pipeline", "GraphicsPipeline", LogType::Error);
	}
}

GraphicsPipeline::~GraphicsPipeline() {
	//Destroy Vulkan objects
	if (vulkanPipeline != VK_NULL_HANDLE)
		vkDestroyPipeline(Vulkan::getDevice()->getLogical(), vulkanPipeline, nullptr);
}

void GraphicsPipeline::bind() {
	if (BaseEngine::usingVulkan()) {
		//Bind the pipeline
		vkCmdBindPipeline(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline);
	} else {
		//Use the shader
		renderShader->getShader()->use();

		//Assign the depth state
		if (depthState.depthTestEnable) {
			glEnable(GL_DEPTH_TEST);

			glDepthMask(depthState.depthWriteEnable);
			glDepthFunc(convertToGL(depthState.depthCompareOp));
		} else
			glDisable(GL_DEPTH_TEST);

		//Assign the blend state
		if (colourBlendState.blendEnabled) {
			glEnable(GL_BLEND);

			glBlendFuncSeparate(convertToGL(colourBlendState.srcRGB), convertToGL(colourBlendState.dstRGB), convertToGL(colourBlendState.srcAlpha), convertToGL(colourBlendState.dstAlpha));
		} else
			glDisable(GL_BLEND);
	}

	//Notify Renderer
	Renderer::setCurrentGraphicsPipeline(this);
}

GLenum GraphicsPipeline::convertToGL(BlendFactor factor) {
	switch (factor) {
		case BlendFactor::ZERO:
			return GL_ZERO;
		case BlendFactor::ONE:
			return GL_ONE;
		case BlendFactor::SRC_ALPHA:
			return GL_SRC_ALPHA;
		case BlendFactor::ONE_MINUS_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		default:
			return GL_ZERO;
	}
}

VkBlendFactor GraphicsPipeline::convertToVk(BlendFactor factor) {
	switch (factor) {
		case BlendFactor::ZERO:
			return VK_BLEND_FACTOR_ZERO;
		case BlendFactor::ONE:
			return VK_BLEND_FACTOR_ONE;
		case BlendFactor::SRC_ALPHA:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case BlendFactor::ONE_MINUS_SRC_ALPHA:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		default:
			return VK_BLEND_FACTOR_ZERO;
	}
}

GLenum GraphicsPipeline::convertToGL(CompareOperation op) {
	switch (op) {
		case CompareOperation::LESS:
			return GL_LESS;
		case CompareOperation::EQUAL:
			return GL_EQUAL;
		case CompareOperation::LESS_OR_EQUAL:
			return GL_LEQUAL;
		case CompareOperation::GREATER:
			return GL_GREATER;
		case CompareOperation::GREATER_OR_EQUAL:
			return GL_GEQUAL;
		default:
			return GL_LESS;
	}
}

VkCompareOp GraphicsPipeline::convertToVk(CompareOperation op) {
	switch (op) {
		case CompareOperation::LESS:
			return VK_COMPARE_OP_LESS;
		case CompareOperation::EQUAL:
			return VK_COMPARE_OP_EQUAL;
		case CompareOperation::LESS_OR_EQUAL:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case CompareOperation::GREATER:
			return VK_COMPARE_OP_GREATER;
		case CompareOperation::GREATER_OR_EQUAL:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		default:
			return VK_COMPARE_OP_LESS;
	}
}

/*****************************************************************************
 * The GraphicsPipelineLayout class
 *****************************************************************************/

GraphicsPipelineLayout::GraphicsPipelineLayout(RenderShader* renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState) : renderShader(renderShader), vertexInputData(vertexInputData), colourBlendState(colourBlendState), depthState(depthState) {
	//Ensure using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Create the pipeline

		std::vector<VkDescriptorSetLayout> layouts;

		for (auto& it : renderShader->getDescriptorSetLayouts())
			layouts.push_back(it.second->getVkLayout());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size()); //Optional
		pipelineLayoutInfo.pSetLayouts = layouts.data(); //Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; //Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; //Optional

		if (vkCreatePipelineLayout(Vulkan::getDevice()->getLogical(), &pipelineLayoutInfo, nullptr, &vulkanPipelineLayout) != VK_SUCCESS)
			Logger::log("Failed to create pipeline layout", "RenderPipeline", LogType::Error);
	}
}

GraphicsPipelineLayout::~GraphicsPipelineLayout() {
	//Destroy Vulkan objects
	if (vulkanPipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(Vulkan::getDevice()->getLogical(), vulkanPipelineLayout, nullptr);
}
