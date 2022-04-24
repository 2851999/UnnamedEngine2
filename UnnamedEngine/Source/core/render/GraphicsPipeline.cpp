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
#include "../Object.h"
#include "../../utils/VulkanUtils.h"

 /*****************************************************************************
  * The GraphicsPipeline class
  *****************************************************************************/

GraphicsPipeline::GraphicsPipeline(GraphicsPipelineLayout* layout, RenderPass* renderPass, uint32_t viewportWidth, uint32_t viewportHeight) : layout(layout), viewportWidth(viewportWidth), viewportHeight(viewportHeight), renderShader(layout->getRenderShader()), colourBlendState(layout->getColourBlendState()), depthState(layout->getDepthState()), cullState(layout->getCullState()) {
	//Check if the viewport width/height was assigned
	if (this->viewportWidth == 0) {
		//Assign default values of the window width/height
		Settings settings = Window::getCurrentInstance()->getSettings();
		this->viewportWidth  = settings.windowWidth;
		this->viewportHeight = settings.windowHeight;
	}
	
	//Check if using Vulkan
	if (BaseEngine::usingVulkan()) {
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, renderShader->getShader()->getVkVertexShaderModule(), "main"); //"main" is the entry point

		//pSpecializationInfo can be used to specify values for shader constants - faster than using if statements
		//default set to nullptr

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, renderShader->getShader()->getVkFragmentShaderModule(), "main");

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (renderShader->getShader()->getVkGeometryShaderModule() != VK_NULL_HANDLE) {
			VkPipelineShaderStageCreateInfo geomShaderStageInfo = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, renderShader->getShader()->getVkGeometryShaderModule(), "main");

			shaderStages = { vertShaderStageInfo, geomShaderStageInfo, fragShaderStageInfo };
		} else
			shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		GraphicsPipeline::VertexInputData vertexInputData = layout->getVertexInputData();

		vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(vertexInputData.bindings.size());
		vertexInputInfo.pVertexBindingDescriptions      = vertexInputData.bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputData.attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions    = vertexInputData.attributes.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology               = convertToVk(vertexInputData.primitiveTopology);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		if (layout->getViewportFlippedVk()) {
			viewport.x      = 0.0f;
			viewport.y      = (float) this->viewportHeight;
			viewport.width  = (float) this->viewportWidth;
			viewport.height = -((float) viewport.y); //Flip so that it resembles OpenGL
		} else {
			viewport.x      = 0.0f;
			viewport.y      = 0.0f;
			viewport.width  = (float) this->viewportWidth;
			viewport.height = (float) this->viewportHeight;
		}
		viewport.minDepth   = 0.0f;
		viewport.maxDepth   = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { this->viewportWidth, this->viewportHeight };

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
		rasterizer.cullMode                = convertToVk(cullState.mode); //VK_CULL_MODE_BACK_BIT
		rasterizer.frontFace               = convertToVk(cullState.frontFace);

		//If viewport is not flipped then, front face must be flipped to match OpenGL (Found with shadow mapping)
		if (!layout->getViewportFlippedVk()) {
			if (rasterizer.frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE)
				rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			else
				rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		rasterizer.depthBiasEnable         = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; //Optional
		rasterizer.depthBiasClamp          = 0.0f; //Optional
		rasterizer.depthBiasSlopeFactor    = 0.0f; //Optional

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable   = VK_FALSE;
		multisampling.rasterizationSamples  = static_cast<VkSampleCountFlagBits>(renderPass->getNumSamples() == 0 ? 1 : renderPass->getNumSamples());
		multisampling.minSampleShading      = 1.0f; //Optional
		multisampling.pSampleMask           = nullptr; //Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
		multisampling.alphaToOneEnable      = VK_FALSE; //Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable       = depthState.depthTestEnable ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable      = depthState.depthWriteEnable ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp        = convertToVk(depthState.depthCompareOp);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds        = 0.0f; //Optional
		depthStencil.maxDepthBounds        = 1.0f; //Optional
		depthStencil.stencilTestEnable     = VK_FALSE;
		depthStencil.front                 = {}; //Optional
		depthStencil.back                  = {}; //Optional

		//Per framebuffer (use same one for all)
		VkPipelineColorBlendAttachmentState colourBlendAttachment = {};
		colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colourBlendAttachment.blendEnable = colourBlendState.blendEnabled ? VK_TRUE : VK_FALSE;

		if (colourBlendState.blendEnabled) {
			colourBlendAttachment.srcColorBlendFactor = convertToVk(colourBlendState.srcRGB);
			colourBlendAttachment.dstColorBlendFactor = convertToVk(colourBlendState.dstRGB);
			colourBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
			colourBlendAttachment.srcAlphaBlendFactor = convertToVk(colourBlendState.srcAlpha);
			colourBlendAttachment.dstAlphaBlendFactor = convertToVk(colourBlendState.srcAlpha);
			colourBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
		}

		//Colour attachments
		std::vector<VkPipelineColorBlendAttachmentState> colourBlendAttachments(renderPass->getNumColourAttachments());
		for (unsigned int i = 0; i < colourBlendAttachments.size(); ++i)
			colourBlendAttachments[i] = colourBlendAttachment;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable     = VK_FALSE;
		colorBlending.logicOp           = VK_LOGIC_OP_COPY; //Optional
		colorBlending.attachmentCount   = static_cast<uint32_t>(colourBlendAttachments.size());
		colorBlending.pAttachments      = colourBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f; //Optional
		colorBlending.blendConstants[1] = 0.0f; //Optional
		colorBlending.blendConstants[2] = 0.0f; //Optional
		colorBlending.blendConstants[3] = 0.0f; //Optional

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages             = shaderStages.data();
		pipelineInfo.pVertexInputState   = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState      = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState   = &multisampling;
		pipelineInfo.pDepthStencilState  = &depthStencil;
		pipelineInfo.pColorBlendState    = &colorBlending;
		pipelineInfo.pDynamicState       = nullptr; //Optional
		pipelineInfo.layout              = layout->getVkInstance();
		pipelineInfo.renderPass          = renderPass->getVkInstance();
		pipelineInfo.subpass             = 0;

		pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; //Optional
		pipelineInfo.basePipelineIndex   = -1; //Optional

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
		//Assign the viewport
		glViewport(0, 0, viewportWidth, viewportHeight);

		//Use the shader
		renderShader->getShader()->use();

		//Assign the depth state
		if (depthState.depthTestEnable) {
			glEnable(GL_DEPTH_TEST);

			glDepthFunc(convertToGL(depthState.depthCompareOp));
		} else
			glDisable(GL_DEPTH_TEST);

		glDepthMask(depthState.depthWriteEnable);

		//Assign the cull state
		if (cullState.mode != CullMode::NONE) {
			glEnable(GL_CULL_FACE);
			glFrontFace(convertToGL(cullState.frontFace));
			glCullFace(convertToGL(cullState.mode));
		} else
			glDisable(GL_CULL_FACE);

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

void GraphicsPipeline::renderAllQueued() {
	if (queuedObjects.size() > 0) {
		//Bind this pipeline
		bind();
		//Go through and render all of the queued objects
		for (GameObject* object : queuedObjects)
			object->queuedRender();
		//Clear the queue
		queuedObjects.clear();
	}
}

GLenum GraphicsPipeline::convertToGL(PrimitiveTopology primitiveTopology) {
	switch (primitiveTopology) {
		case PrimitiveTopology::TRIANGLE_LIST:
			return GL_TRIANGLES;
		case PrimitiveTopology::TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case PrimitiveTopology::TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		case PrimitiveTopology::POINT_LIST:
			return GL_POINTS;
		case PrimitiveTopology::LINE_LIST:
			return GL_LINES;
		case PrimitiveTopology::LINE_STRIP:
			return GL_LINE_STRIP;
		case PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
			return GL_LINES_ADJACENCY;
		case PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
			return GL_LINE_STRIP_ADJACENCY;
		case PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
			return GL_TRIANGLES_ADJACENCY;
		case PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
			return GL_TRIANGLE_STRIP_ADJACENCY;
		case PrimitiveTopology::PATCH_LIST:
			return GL_PATCHES;
		default:
			return GL_TRIANGLES;
	}
}

VkPrimitiveTopology GraphicsPipeline::convertToVk(PrimitiveTopology primitiveTopology) {
	switch (primitiveTopology) {
		case PrimitiveTopology::TRIANGLE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case PrimitiveTopology::TRIANGLE_STRIP:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case PrimitiveTopology::TRIANGLE_FAN:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case PrimitiveTopology::POINT_LIST:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case PrimitiveTopology::LINE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PrimitiveTopology::LINE_STRIP:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case PrimitiveTopology::PATCH_LIST:
			return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
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

GLenum GraphicsPipeline::convertToGL(CullMode mode) {
	switch (mode) {
		case CullMode::NONE:
			return GL_NONE;
		case CullMode::FRONT:
			return GL_FRONT;
		case CullMode::BACK:
			return GL_BACK;
		case CullMode::FRONT_AND_BACK:
			return GL_FRONT_AND_BACK;
		default:
			return GL_NONE;
	}
}

VkCullModeFlagBits GraphicsPipeline::convertToVk(CullMode mode) {
	switch (mode) {
		case CullMode::NONE:
			return VK_CULL_MODE_NONE;
		case CullMode::FRONT:
			return VK_CULL_MODE_FRONT_BIT;
		case CullMode::BACK:
			return VK_CULL_MODE_BACK_BIT;
		case CullMode::FRONT_AND_BACK:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			return VK_CULL_MODE_NONE;
	}
}

GLenum GraphicsPipeline::convertToGL(FrontFace face) {
	switch (face) {
		case FrontFace::COUNTER_CLOCKWISE:
			return GL_CCW;
		case FrontFace::CLOCKWISE:
			return GL_CW;
		default:
			return GL_CCW;
	}
}

VkFrontFace GraphicsPipeline::convertToVk(FrontFace face) {
	switch (face) {
		case FrontFace::COUNTER_CLOCKWISE:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case FrontFace::CLOCKWISE:
			return VK_FRONT_FACE_CLOCKWISE;
		default:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}
}

/*****************************************************************************
 * The GraphicsPipelineLayout class
 *****************************************************************************/

GraphicsPipelineLayout::GraphicsPipelineLayout(RenderShader* renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState, GraphicsPipeline::CullState cullState, bool viewportFlippedVk) :
	renderShader(renderShader), vertexInputData(vertexInputData), colourBlendState(colourBlendState), depthState(depthState), cullState(cullState), viewportFlippedVk(viewportFlippedVk) {

	//Ensure using Vulkan
	if (BaseEngine::usingVulkan()) {
		//Create the pipeline

		std::vector<VkDescriptorSetLayout> layouts;

		for (auto& it : renderShader->getDescriptorSetLayouts())
			layouts.push_back(it.second->getVkLayout());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = utils_vulkan::initPipelineLayoutCreateInfo(static_cast<uint32_t>(layouts.size()), layouts.data());

		if (vkCreatePipelineLayout(Vulkan::getDevice()->getLogical(), &pipelineLayoutInfo, nullptr, &vulkanPipelineLayout) != VK_SUCCESS)
			Logger::log("Failed to create pipeline layout", "RenderPipeline", LogType::Error);
	} else {
		//Assign the primitive topology used for rendering in OpenGL
		primitiveTopologyGL = GraphicsPipeline::convertToGL(vertexInputData.primitiveTopology);
	}
}

GraphicsPipelineLayout::~GraphicsPipelineLayout() {
	//Destroy Vulkan objects
	if (vulkanPipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(Vulkan::getDevice()->getLogical(), vulkanPipelineLayout, nullptr);
}