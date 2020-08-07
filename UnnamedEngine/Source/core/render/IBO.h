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

#include <GL/glew.h>

#include "RenderShader.h"
#include "../vulkan/VulkanBuffer.h"
#include "DataUsage.h"


 /*****************************************************************************
  * The IBO class is used to manage an index buffer object
  *****************************************************************************/

class IBO {
private:
	/* Buffer handle for OpenGL */
	GLuint bufferGL = 0;

	/* The Vulkan buffer for this IBO */
	VulkanBufferObject* vulkanBuffer = NULL;

	/* Data for this IBO */
	uint32_t                   size;
	std::vector<unsigned int>& data;
	DataUsage                  usage;
public:
	/* The constructors */
	IBO(uint32_t size, std::vector<unsigned int>& data, DataUsage usage) :
		size(size), data(data), usage(usage) {
	}

	/* The destructor */
	virtual ~IBO() {
		if (bufferGL > 0)
			glDeleteBuffers(1, &bufferGL);
		delete vulkanBuffer;
	}

	/* Various OpenGL methods */
	inline void bindGL() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferGL); }

	/* Used to create and setup this IBO */
	void setup();

	/* Methods used to setup this VBO just before/after rendering */
	void startRendering();
	void stopRendering();

	/* Methods used to update the VBO's data */
	void update();
	void updateFrame();
	void updateStream(uint32_t size);

	/* Setters and getters */
	inline VulkanBuffer* getVkCurrentBuffer() { return vulkanBuffer->getCurrentBuffer(); }
};

