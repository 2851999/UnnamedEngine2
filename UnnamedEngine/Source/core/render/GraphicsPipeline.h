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

class RenderPass;
class GraphicsPipelineLayout;
class GameObject;

/*****************************************************************************
 * The GraphicsPipeline class responsible for handleing Vulkan pipelines and to
 * produce similar behaviour in OpenGL
 *****************************************************************************/

class GraphicsPipeline {
public:
	/* Various generalised primitive topologies */
	enum class PrimitiveTopology {
		TRIANGLE_LIST, TRIANGLE_STRIP, TRIANGLE_FAN, POINT_LIST, LINE_LIST, LINE_STRIP, LINE_LIST_WITH_ADJACENCY, LINE_STRIP_WITH_ADJACENCY, TRIANGLE_LIST_WITH_ADJACENCY, TRIANGLE_STRIP_WITH_ADJACENCY, PATCH_LIST
	};

	/* Various generalised blend states */
	enum class BlendFactor {
		ZERO, ONE, SRC_ALPHA, ONE_MINUS_SRC_ALPHA
	};

	/* Varuous generalised compare operations */
	enum class CompareOperation {
		LESS, EQUAL, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL
	};

	/* Various generalised culling modes */
	enum class CullMode {
		NONE, FRONT, BACK, FRONT_AND_BACK
	};

	/* Various generalised faces for culling modes */
	enum class FrontFace {
		COUNTER_CLOCKWISE, CLOCKWISE
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

		BlendFactor srcRGB   = BlendFactor::ONE;
		BlendFactor dstRGB   = BlendFactor::ZERO;
		BlendFactor srcAlpha = BlendFactor::ONE;
		BlendFactor dstAlpha = BlendFactor::ZERO;
	};

	/* Structure containing information about the culling state to be used */
	struct CullState {
		CullMode  mode      = CullMode::NONE;
		FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
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

	/* The cull state to use with this pipeline */
	CullState cullState;

	/* List of game objects queued to be renered with this pipeline */
	std::vector<GameObject*> queuedObjects;

	/* Width and height of the viewport used */
	uint32_t viewportWidth;
	uint32_t viewportHeight;
public:
	/* Structure used to store data required for creating a render pipeline */
	struct VertexInputData {
		GraphicsPipeline::PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		std::vector<VkVertexInputBindingDescription>   bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
	};

	/* Constructor */
	GraphicsPipeline(GraphicsPipelineLayout* layout, RenderPass* renderPass, uint32_t viewportWidth = 0, uint32_t viewportHeight = 0);

	/* Destructor */
	virtual ~GraphicsPipeline();

	/* Method used to bind this pipeline for rendering */
	void bind();

	/* Method used to render all queued objects with this pipeline (Clears afterwards) */
	void renderAllQueued();

	/* Method used to queue an object for rendering with this pipeline */
	inline void queueRender(GameObject* object) { queuedObjects.push_back(object); }

	/* Methods used to convert the a generalised states to the one required by
	   OpenGL/Vulkan */
	static GLenum convertToGL(PrimitiveTopology primativeTopology);
	static VkPrimitiveTopology convertToVk(PrimitiveTopology primativeTopology);
	static GLenum convertToGL(BlendFactor factor);
	static VkBlendFactor convertToVk(BlendFactor factor);
	static GLenum convertToGL(CompareOperation op);
	static VkCompareOp convertToVk(CompareOperation op);
	static GLenum convertToGL(CullMode mode);
	static VkCullModeFlagBits convertToVk(CullMode mode);
	static GLenum convertToGL(FrontFace face);
	static VkFrontFace convertToVk(FrontFace face);

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

	/* The colour, depth and cull states for the pipeline */
	GraphicsPipeline::ColourBlendState colourBlendState;
	GraphicsPipeline::DepthState       depthState;
	GraphicsPipeline::CullState        cullState;

	/* States whether the viewport should be flipped (Vulkan is flipped compared to OpenGL) */
	bool viewportFlippedVk;
public:
	/* Constructor */
	GraphicsPipelineLayout(RenderShader* renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState, GraphicsPipeline::CullState cullState, bool viewportFlippedVk);

	/* Destructor */
	virtual ~GraphicsPipelineLayout();

	/* Getters */
	inline VkPipelineLayout& getVkInstance() { return vulkanPipelineLayout; }
	inline RenderShader* getRenderShader() { return renderShader; }
	inline GraphicsPipeline::VertexInputData& getVertexInputData() { return vertexInputData; }
	inline GraphicsPipeline::ColourBlendState& getColourBlendState() { return colourBlendState; }
	inline GraphicsPipeline::DepthState& getDepthState() { return depthState; }
	inline GraphicsPipeline::CullState& getCullState() { return cullState; }
	inline bool getViewportFlippedVk() { return viewportFlippedVk; }
};