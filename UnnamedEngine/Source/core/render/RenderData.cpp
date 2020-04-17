/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
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

#include "RenderData.h"

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

#include "Renderer.h"

/*****************************************************************************
 * The RenderData class
 *****************************************************************************/

RenderData::~RenderData() {
	delete descriptorSetModel;
}

void RenderData::setup(RenderShader* renderShader) {
	//Create the descriptor set
	descriptorSetModel = new DescriptorSet(renderShader->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_NUMBER_PER_MODEL));
	//Setup the descriptor set
	descriptorSetModel->setup();

	if (! BaseEngine::usingVulkan()) {
		//Generate the VAO and bind it
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	} else {
		vboVkInstances.resize(vbosFloat.size() + vbosUInteger.size());
		vboVkOffsets.resize(vbosFloat.size() + vbosUInteger.size());
	}

	//Go through each VBO and set it up
	for (unsigned int i = 0; i < vbosFloat.size(); ++i) {
		vbosFloat[i]->setup(i);
		vbosFloat[i]->startRendering();

		if (BaseEngine::usingVulkan()) {
			vboVkInstances[i] = vbosFloat[i]->getVkBuffer()->getInstance();
			vboVkOffsets[i] = 0;

			bindingVkDescriptions.push_back(vbosFloat[i]->getVkBindingDescription());
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions = vbosFloat[i]->getVkAttributeDescriptions();
			attributeVkDescriptions.insert(attributeVkDescriptions.end(), attributeDescriptions.begin(), attributeDescriptions.end());
		}
	}

	for (unsigned int i = 0; i < vbosUInteger.size(); ++i) {
		vbosUInteger[i]->setup(vbosFloat.size() + i);
		vbosUInteger[i]->startRendering();

		if (BaseEngine::usingVulkan()) {
			vboVkInstances[vbosFloat.size() + i] = vbosUInteger[i]->getVkBuffer()->getInstance();
			vboVkOffsets[vbosFloat.size() + i] = 0;

			bindingVkDescriptions.push_back(vbosUInteger[i]->getVkBindingDescription());
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions = vbosUInteger[i]->getVkAttributeDescriptions();
			attributeVkDescriptions.insert(attributeVkDescriptions.end(), attributeDescriptions.begin(), attributeDescriptions.end());
		}
	}

	//Now setup the indices VBO if assigned
	if (vboIndices) {
		vboIndices->setup(0);
		vboIndices->startRendering();
	}

	if (! BaseEngine::usingVulkan())
		glBindVertexArray(0);
}

void RenderData::bindBuffers() {
	if (! BaseEngine::usingVulkan())
		glBindVertexArray(vao);
	else {
		vkCmdBindVertexBuffers(Vulkan::getCurrentCommandBuffer(), 0, vboVkInstances.size(), vboVkInstances.data(), vboVkOffsets.data());
		vkCmdBindIndexBuffer(Vulkan::getCurrentCommandBuffer(), vboIndices->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit
	}
}
void RenderData::unbindBuffers() {
	if (! BaseEngine::usingVulkan())
		glBindVertexArray(0);
}

void RenderData::renderWithoutBinding() {
	if (! BaseEngine::usingVulkan()) {
		//Check for instancing
		if (primcount > 0) {
			//Check for indices
			if (vboIndices)
				glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, (void*) NULL, primcount);
			else
				glDrawArraysInstanced(mode, 0, count, primcount);
		} else if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) NULL);
			else
				glDrawArrays(mode, 0, count);
		}
	} else {
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				vkCmdDrawIndexed(Vulkan::getCurrentCommandBuffer(), count, 1, 0, 0, 0);
			else
				vkCmdDraw(Vulkan::getCurrentCommandBuffer(), count, 1, 0, 0);
		}
	}
}

void RenderData::renderBaseVertex(unsigned int count, unsigned int indicesOffset, unsigned int baseVertex) {
	if (! BaseEngine::usingVulkan()) {
		//Check for instancing
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				glDrawElementsBaseVertex(mode, count, GL_UNSIGNED_INT, (void*) (indicesOffset * sizeof(unsigned int)), baseVertex); //Assume indices stored as unsigned integers
		}
	} else {
		//Check for instancing
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				vkCmdDrawIndexed(Vulkan::getCurrentCommandBuffer(), count, 1, indicesOffset, baseVertex, 0);
		}
	}
}
