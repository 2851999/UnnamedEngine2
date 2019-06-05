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

#include "../vulkan/Vulkan.h"

/*****************************************************************************
 * The RenderData class
 *****************************************************************************/

void RenderData::setup() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Generate the VAO and bind it
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	} else {
		vboVkInstances.resize(vbosFloat.size());
		vboVkOffsets.resize(vbosFloat.size());
	}

	//Go through each VBO and set it up
	for (unsigned int i = 0; i < vbosFloat.size(); i++) {
		vbosFloat[i]->setup();
		vbosFloat[i]->startRendering();

		if (Window::getCurrentInstance()->getSettings().videoVulkan) {
			vboVkInstances[i] = vbosFloat[i]->getVkBuffer()->getInstance();
			vboVkOffsets[i] = 0; //Is this right offset???
		}
	}

	for (unsigned int i = 0; i < vbosUInteger.size(); i++) {
		vbosUInteger[i]->setup();
		vbosUInteger[i]->startRendering();
	}

	//Now setup the indices VBO if assigned
	if (vboIndices) {
		vboIndices->setup();
		vboIndices->startRendering();
	}

	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(0);
}

void RenderData::bindBuffers() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(vao);
	else {
		vkCmdBindVertexBuffers(Vulkan::getCurrentCommandBuffer(), 0, 1, vboVkInstances.data(), vboVkOffsets.data());
		vkCmdBindIndexBuffer(Vulkan::getCurrentCommandBuffer(), vboIndices->getVkBuffer()->getInstance(), 0, VK_INDEX_TYPE_UINT32); //Using unsigned int which is 32 bit
	}
}
void RenderData::unbindBuffers() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan)
		glBindVertexArray(0);
}

void RenderData::renderWithoutBinding() {
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
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
	if (! Window::getCurrentInstance()->getSettings().videoVulkan) {
		//Check for instancing
		if (primcount == -1) {
			//Check for indices
			if (vboIndices)
				glDrawElementsBaseVertex(mode, count, GL_UNSIGNED_INT, (void*) indicesOffset, baseVertex);
		}
	}
}
