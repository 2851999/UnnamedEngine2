/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#ifndef CORE_RENDER_GEOMETRYBUFFER_H_
#define CORE_RENDER_GEOMETRYBUFFER_H_

#include "FBO.h"

/*****************************************************************************
 * The GeometryBuffer class contains everything needed to handle a geometry
 * buffer for deferred rendering
 *****************************************************************************/

class GeometryBuffer : public FBO {
private:
	/* Method used to generate a framebuffer texture */
	FramebufferTexture* createBuffer(GLenum target, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
public:
	/* The constructor */
	GeometryBuffer(bool pbr);

	/* The destructor */
	virtual ~GeometryBuffer() {}
};


#endif /* CORE_RENDER_GEOMETRYBUFFER_H_ */
