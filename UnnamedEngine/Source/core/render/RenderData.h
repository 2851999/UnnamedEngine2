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

#ifndef CORE_RENDER_RENDERDATA_H_
#define CORE_RENDER_RENDERDATA_H_

#include "VBO.h"

/*****************************************************************************
 * The RenderData class is used for rendering
 *****************************************************************************/

class RenderData {
private:
	/* The VAO */
	GLuint vao = 0;
	/* The VBO's used for rendering */
	std::vector<VBO<GLfloat>*> vbosFloat;
	std::vector<VBO<GLuint>*>  vbosUInteger;
	VBO<unsigned int>*         vboIndices = NULL;

	/* The render mode */
	GLenum mode;

	/* States the 'count' used for rendering - the number of positions/
	 * indices */
	GLsizei count;

	/* States the number of instances to render, instancing is only used if
	 * this value is greater than zero */
	GLsizei primcount = -1;

	/* The vertex buffer instances and offsets for Vulkan */
	std::vector<VkBuffer> vboVkInstances;
	std::vector<VkDeviceSize> vboVkOffsets;
public:

	/* The constructor */
	RenderData(GLenum mode, GLsizei count) : mode(mode), count(count) {}

	/* The destructor */
	virtual ~RenderData() {}

	/* The method used to setup this data for rendering */
	void setup();

	/* Method used to bind/unbind the VAO/other buffers before/after rendering */
	void bindBuffers();
	void unbindBuffers();

	/* The method used to render this data */
	inline void render() {
		bindBuffers();
		renderWithoutBinding();
		unbindBuffers();
	}

	/* The method to render this data without binding/unbinding the VAO */
	void renderWithoutBinding();

	/* Method to render this data using glDrawElementsBaseVertex */
	void renderBaseVertex(unsigned int count, unsigned int indicesOffset, unsigned int baseVertex);

	/* The setters and getters */
	inline void addVBO(VBO<GLfloat>* vbo) { vbosFloat.push_back(vbo); }
	inline void addVBO(VBO<GLuint>* vbo) { vbosUInteger.push_back(vbo); }
	inline void setIndicesVBO(VBO<unsigned int>* vboIndices) { this->vboIndices = vboIndices; }
	inline void setMode(GLenum mode) { this->mode = mode; }
	inline void setCount(GLsizei count) { this->count = count; }
	inline void setNumInstances(GLsizei primcount) { this->primcount = primcount; }

	inline GLuint getVAO() { return vao; }
};

#endif /* CORE_RENDER_RENDERDATA_H_ */
