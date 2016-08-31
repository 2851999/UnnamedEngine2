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

#include "VBO.h"

/*****************************************************************************
 * The VBO class
 *****************************************************************************/
template <typename T>
void VBO<T>::addAttribute(GLint location, GLint size, GLuint divisor) {
	Attribute attrib;
	attrib.location = location;
	attrib.size     = size;
	attrib.offset   = 0;
	attrib.stride   = 0;
	attrib.divisor  = divisor;
	attributes.push_back(attrib);
}

template <typename T>
void VBO<T>::setup() {
	//Get OpenGL to generate the buffer
	glGenBuffers(1, &buffer);
	//Bind the buffer
	bind();
	//Pass the data to OpenGL
	if (data.size() > 0)
		glBufferData(target, size, &data.front(), usage);
	else
		glBufferData(target, size, NULL, usage);
	//Check to see whether the Attributes is > 1, as if this is the case
	//this VBO is interleaved
	if (attributes.size() > 1) {
		//In this case the offset and stride of the attributes need to be
		//calculated

		//The stride to use
		GLint stride = 0;

		//Go through all of their attributes
		for (unsigned int i = 0; i < attributes.size(); i++)
			stride += attributes[i].size;
		stride *= sizeof(data[0]);

		//Now to setup and assign the attributes values

		//The current offset is incremented so it is defined outside the
		//for loop
		GLint currentOffset = 0;

		//Go through all of the attributes
		for (unsigned int i = 0; i < attributes.size(); i++) {
			//Assign the offset and stride
			attributes[i].offset = currentOffset;
			attributes[i].stride = stride;

			//Setup the current attribute
			setup(attributes[i]);

			//Increment the current offset
			currentOffset += attributes[i].size * sizeof(data[0]);
		}
	} else {
		//Go through all of the attributes
		for (unsigned int i = 0; i < attributes.size(); i++)
			//Setup the current attribute
			setup(attributes[i]);
	}
}

template <typename T>
void VBO<T>::setup(Attribute& attribute) {
	//Ensure the location is valid
	if (attribute.location != -1) {
		//Enable that vertex attribute array and setup its pointer
		glEnableVertexAttribArray(attribute.location);
		glVertexAttribPointer(attribute.location, attribute.size, GL_FLOAT, GL_FALSE, attribute.stride, (void*) attribute.offset);
	}
}

template <typename T>
void VBO<T>::startRendering() {
	bind();

	//Enable the vertex attribute arrays
	for (unsigned int i = 0; i < attributes.size(); i++) {
		glEnableVertexAttribArray(attributes[i].location);
		//Check for instancing
		if (instanced)
			glVertexAttribDivisor(attributes[i].location, attributes[i].divisor);
	}
}

template <typename T>
void VBO<T>::stopRendering() {
	//Disable the vertex attribute arrays
	for (unsigned int i = 0; i < attributes.size(); i++)
		glDisableVertexAttribArray(attributes[i].location);
}

template <typename T>
void VBO<T>::update() {
	bind();
	glBufferData(target, data.size() * sizeof(data[0]), &data.front(), usage);
}

template <typename T>
void VBO<T>::updateStream(GLsizeiptr size) {
	bind();
	//Buffer orphaning
	glBufferData(target, this->size, NULL, usage);
	glBufferSubData(target, 0, size, &data.front());
}
