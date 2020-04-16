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

#include "Vulkan.h"
#include "../../utils/Logging.h"
#include "../render/RenderData.h"
#include "../render/Renderer.h"

#include <fstream>

/*****************************************************************************
 * The GraphicsPipeline class
 *****************************************************************************/

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, RenderData* renderData, RenderShader* renderShader) {
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

	auto bindingDescription    = renderData->getVkBindingDescriptions();
	auto attributeDescriptions = renderData->getVkAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(bindingDescription.size());
	vertexInputInfo.pVertexBindingDescriptions      = bindingDescription.data();
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
	rasterizer.cullMode                = VK_CULL_MODE_NONE; //VK_CULL_MODE_BACK_BIT
	rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable         = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; //Optional
	rasterizer.depthBiasClamp          = 0.0f; //Optional
	rasterizer.depthBiasSlopeFactor    = 0.0f; //Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = static_cast<VkSampleCountFlagBits>(swapChain->getNumSamples() == 0 ? 1 : swapChain->getNumSamples());
	multisampling.minSampleShading      = 1.0f; //Optional
	multisampling.pSampleMask           = nullptr; //Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
	multisampling.alphaToOneEnable      = VK_FALSE; //Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable       = VK_TRUE;
	depthStencil.depthWriteEnable      = renderShader->getGraphicsState()->depthWriteEnable ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds        = 0.0f; //Optional
	depthStencil.maxDepthBounds        = 1.0f; //Optional
	depthStencil.stencilTestEnable     = VK_FALSE;
	depthStencil.front                 = {}; //Optional
	depthStencil.back                  = {}; //Optional

	//Per framebuffer (only have one here)
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable         = VK_FALSE;
//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
//	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD; //Optional
//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
//	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD; //Optional

	if (renderShader->getGraphicsState()->alphaBlending) {
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		//Required to blend lighting
		//colorBlendAttachment.blendEnable = VK_TRUE;
		//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	} else
		colorBlendAttachment.blendEnable = VK_FALSE;

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

	std::vector<VkDescriptorSetLayout> layouts;
	layouts.push_back(renderData->getVkDescriptorSetLayout());
	layouts.push_back(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_MATERIAL)->getVkLayout());
	layouts.push_back(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_MODEL)->getVkLayout());
	layouts.push_back(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_LIGHT_BATCH)->getVkLayout());

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(layouts.size()); //Optional
	pipelineLayoutInfo.pSetLayouts            = layouts.data(); //Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; //Optional
	pipelineLayoutInfo.pPushConstantRanges    = nullptr; //Optional

	if (vkCreatePipelineLayout(swapChain->getDevice()->getLogical(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		Logger::log("Failed to create pipeline layout", "GraphicsPipeline", LogType::Error);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount          = 2;
	pipelineInfo.pStages             = shaderStages;
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;
	pipelineInfo.pDepthStencilState  = &depthStencil;
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDynamicState       = nullptr; //Optional
	pipelineInfo.layout              = pipelineLayout;
	pipelineInfo.renderPass          = renderPass->getInstance();
	pipelineInfo.subpass             = 0;

	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; //Optional
	pipelineInfo.basePipelineIndex   = -1; //Optional

	if (vkCreateGraphicsPipelines(swapChain->getDevice()->getLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		Logger::log("Failed to create graphics pipeline", "GraphicsPipeline", LogType::Error);
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
	//Destroy the pipeline and its layout
	vkDestroyPipeline(swapChain->getDevice()->getLogical(), pipeline, nullptr);
	vkDestroyPipelineLayout(swapChain->getDevice()->getLogical(), pipelineLayout, nullptr);
}

void VulkanGraphicsPipeline::bind() {
	Vulkan::bindGraphicsPipeline(this);
}
