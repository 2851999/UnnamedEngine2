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

/*****************************************************************************
 * The RenderData class
 *****************************************************************************/

void RenderData::setup() {
	//Generate the VAO and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Go through each VBO and set it up
	for (unsigned int i = 0; i < vbos.size(); i++) {
		vbos[i]->setup();
		vbos[i]->startRendering();
	}

	//Now setup the indices VBO if assigned
	if (vboIndices) {
		vboIndices->setup();
		vboIndices->startRendering();
	}

	glBindVertexArray(0);
}

void RenderData::render() {
	//Use the VAO
	glBindVertexArray(vao);

	//Check for instancing
	if (primcount > 0) {
		//Check for indices
		if (vboIndices)
			glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, (void*) NULL, primcount);
		else
			glDrawArraysInstanced(mode, 0, count, primcount);
	} else {
		//Check for indices
		if (vboIndices)
			glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) NULL);
		else
			glDrawArrays(mode, 0, count);
	}

	glBindVertexArray(0);
}
