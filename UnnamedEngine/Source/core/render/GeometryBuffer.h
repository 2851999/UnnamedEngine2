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

class GeometryBuffer {
private:
	/* States whether PBR is being used */
	bool pbr;

	/* The framebuffer objects */
	FBO* multisampleFBO = NULL;
	FBO* defaultFBO = NULL;

	/* Method used to create an FBO for the geometry buffer */
	FBO* createFBO(bool pbr, bool multisample);

	/* Method used to generate a framebuffer store */
	FramebufferStore* createBuffer(GLenum target, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
public:
	/* The constructor */
	GeometryBuffer(bool pbr, bool multisample);

	/* The destructor */
	virtual ~GeometryBuffer() {}

	/* Method called to start rendering to this framebuffer */
	void bind();

	/* Method called to stop rendering to this framebuffer */
	void unbind();

	/* Outputs the depth information to the default framebuffer to allow forward rendering */
	void outputDepthInfo();

	/* Returns pointers to the framebuffer stores (textures to be used in the lighting pass) */
	inline FramebufferStore* getPositionBuffer() { return defaultFBO->getFramebufferStore(0); }
	inline FramebufferStore* getNormalBuffer() { return defaultFBO->getFramebufferStore(1); }
	inline FramebufferStore* getAlbedoBuffer() { return defaultFBO->getFramebufferStore(2); }
	inline FramebufferStore* getMetalnessAOBuffer() { return defaultFBO->getFramebufferStore(3); } //Will only work if PBR is enabled
	inline FramebufferStore* getDepthBuffer() {
		if (pbr)
			return defaultFBO->getFramebufferStore(4);
		else
			return defaultFBO->getFramebufferStore(3);
	}

	/* Returns the default FBO */
	inline FBO* getFBO() { return defaultFBO; }
};


#endif /* CORE_RENDER_GEOMETRYBUFFER_H_ */
