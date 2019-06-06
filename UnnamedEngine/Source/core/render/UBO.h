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

#ifndef CORE_RENDER_UBO_H_
#define CORE_RENDER_UBO_H_

#include <GL/glew.h>

#include "Shader.h"

#include "../vulkan/Vulkan.h"

/*****************************************************************************
 * The UBO class is used to manage a uniform buffer object
 *****************************************************************************/

class UBO {
private:
	/* The buffer instance for OpenGL */
	GLuint buffer;

	/* The buffers for Vulkan (one required per swap chain image) */
	std::vector<VulkanBuffer*> vulkanBuffers;
public:
	/* Constructor */
	UBO(void* data, unsigned int size, GLenum usage, GLuint blockBinding);

	/* Destructor */
	virtual ~UBO();

	/* Method to update the contents of this buffer */
	void update(void* data, GLintptr offset, GLsizeiptr size);
};


#endif /* CORE_RENDER_UBO_H_ */
