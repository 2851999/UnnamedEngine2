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

#include "VulkanGraphicsPipeline.h"

#include "VulkanRenderShader.h"

#include "../../utils/Logging.h"

#include <fstream>

/*****************************************************************************
 * The VulkanGraphicsPipeline class
 *****************************************************************************/

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanSwapChain* swapChain, VBO<float>* vertexBuffer, VulkanRenderPass* renderPass, VulkanRenderShader* renderShader) {
	this->swapChain = swapChain;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = renderShader->getShader()->getVkVertexShaderModule();
	vertShaderStageInfo.pName  = "main"; //Entry point

	//pSpecializationInfo can be used to specify values for shader constants - faster than using if statements
	//default set to nullptr

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = renderShader->getShader()->getVkFragmentShaderModule();
	fragShaderStageInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription    = vertexBuffer->getVkBindingDescription();
	auto attributeDescriptions = vertexBuffer->getVkAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

//	vertexInputInfo.vertexBindingDescriptionCount = 0;
//	vertexInputInfo.pVertexBindingDescriptions = nullptr;
//	vertexInputInfo.vertexAttributeDescriptionCount = 0;
//	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x        = 0.0f;
	viewport.y        = (float) swapChain->getExtent().height;
	viewport.width    = (float) swapChain->getExtent().width;
	viewport.height   = -((float) swapChain->getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset   = { 0, 0 };
	scissor.extent   = swapChain->getExtent();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports    = &viewport;
	viewportState.scissorCount  = 1;
	viewportState.pScissors     = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable        = VK_FALSE; //If true fragments clamped rather than discarded, requires GPU feature
	rasterizer.rasterizerDiscardEnable = VK_FALSE; //If true discards everything, wouldn't render to frame buffer
	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL; //Anything else requires GPU feature
	rasterizer.lineWidth               = 1.0f;
	rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable         = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; //Optional
	rasterizer.depthBiasClamp          = 0.0f; //Optional
	rasterizer.depthBiasSlopeFactor    = 0.0f; //Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading      = 1.0f; //Optional
	multisampling.pSampleMask           = nullptr; //Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
	multisampling.alphaToOneEnable      = VK_FALSE; //Optional

	//Per framebuffer (only have one here)
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable         = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD; //Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD; //Optional

//	colorBlendAttachment.blendEnable = VK_TRUE;
//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable     = VK_FALSE;
	colorBlending.logicOp           = VK_LOGIC_OP_COPY; //Optional
	colorBlending.attachmentCount   = 1;
	colorBlending.pAttachments      = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; //Optional
	colorBlending.blendConstants[1] = 0.0f; //Optional
	colorBlending.blendConstants[2] = 0.0f; //Optional
	colorBlending.blendConstants[3] = 0.0f; //Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = 1; //Optional
	pipelineLayoutInfo.pSetLayouts            = &renderShader->getDescriptorSetLayout(); //Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; //Optional
	pipelineLayoutInfo.pPushConstantRanges    = nullptr; //Optional

	if (vkCreatePipelineLayout(swapChain->getDevice()->getLogical(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		Logger::log("Failed to create pipeline layout", "VulkanGraphicsPipeline", LogType::Error);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount          = 2;
	pipelineInfo.pStages             = shaderStages;
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;
	pipelineInfo.pDepthStencilState  = nullptr; //Optional
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDynamicState       = nullptr; //Optional
	pipelineInfo.layout              = pipelineLayout;
	pipelineInfo.renderPass          = renderPass->getInstance();
	pipelineInfo.subpass             = 0;

	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; //Optional
	pipelineInfo.basePipelineIndex   = -1; //Optional

	if (vkCreateGraphicsPipelines(swapChain->getDevice()->getLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		Logger::log("Failed to create graphics pipeline", "VulkanGraphicsPipeline", LogType::Error);
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
	//Destroy the pipeline and its layout
	vkDestroyPipeline(swapChain->getDevice()->getLogical(), pipeline, nullptr);
	vkDestroyPipelineLayout(swapChain->getDevice()->getLogical(), pipelineLayout, nullptr);
}
