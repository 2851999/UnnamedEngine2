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

#ifndef CORE_RENDER_VBO_H_
#define CORE_RENDER_VBO_H_

#include <GL/glew.h>

#include "Shader.h"

/*****************************************************************************
 * The VBO class is used to manage a vertex buffer object
 *****************************************************************************/

template <typename T>
class VBO {
public:
	/* A structure containing information for supplying VBO data to an
	 * attribute within a shader */
	struct Attribute {
		/* Various pieces of data for OpenGL */
		GLint  location; //Location of this attribute (within a shader program)
		GLint  size;     //This is not literal size, but the number of
					     //components
		GLuint offset;   //Offset of the first component of the vertex attribute
		GLuint stride;   //Byte offset between consecutive vertex attributes

		GLuint divisor;   //Number of instances that will pass between updates of this attribute
	};
private:
	/* Various pieces of data for OpenGL */
	GLuint                buffer;
	GLenum                target;
	GLsizeiptr            size;
	std::vector<T>& data;
	GLenum                usage;

	bool                  instanced;

	/* The attributes this VBO supplies */
	std::vector<Attribute> attributes;
public:
	/* The constructors */
	VBO(GLenum target, GLsizeiptr size, std::vector<T>& data, GLenum usage, bool instanced = false) :
		buffer(0), target(target), size(size), data(data), usage(usage), instanced(instanced) {}

	/* The destructor */
	virtual ~VBO() {
		if (buffer > 0)
			glDeleteBuffers(1, &buffer);
	}

	/* Various OpenGL methods */
	inline void bind() { glBindBuffer(target, buffer); }

	/* Used to add an attribute */
	void addAttribute(GLint location, GLint size, GLuint divisor = 0);

	/* Used to create and setup this VBO */
	void setup();

	/* Used to setup an attribute */
	void setup(Attribute& attribute);

	/* Methods used to setup this VBO just before/after rendering */
	void startRendering();
	void stopRendering();

	/* Methods used to update the VBO's data */
	void update();
	void updateStream(GLsizeiptr size);

	/* Setters and getters */
	inline std::vector<T>& getData() { return data; }
};

template class VBO<GLfloat>;
template class VBO<unsigned int>;

#endif /* CORE_RENDER_VBO_H_ */
