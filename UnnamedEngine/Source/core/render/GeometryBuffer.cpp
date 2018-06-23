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

#include "GeometryBuffer.h"

#include "../Window.h"

/*****************************************************************************
 * The GeometryBuffer class
 *****************************************************************************/

GeometryBuffer::GeometryBuffer(bool pbr) : pbr(pbr) {
	//Create the  default FBO
	defaultFBO = createFBO(pbr);
}

FBO* GeometryBuffer::createFBO(bool pbr) {
	//Create the FBO instance
	FBO* fbo = new FBO(GL_FRAMEBUFFER, false);

	//Attach the buffers to the FBO

	//Use textures for binding to use in the shaders
	GLenum target = GL_TEXTURE_2D;

	//Position buffer
	fbo->attach(createBuffer(target, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0));

	//Normal buffer
	fbo->attach(createBuffer(target, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT1));

	//Albedo buffer
	fbo->attach(createBuffer(target, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2));

	//Extra buffer for metalness and ao components for PBR
	if (pbr)
		fbo->attach(createBuffer(target, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT3));

	//Depth buffer
	fbo->attach(createBuffer(target, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, GL_DEPTH_ATTACHMENT));

	//Setup this buffer
	fbo->setup();

	//Return the FBO
	return fbo;
}

FramebufferStore* GeometryBuffer::createBuffer(GLenum target, GLint internalFormat, GLenum format, GLenum type, GLenum attachment) {
	return new FramebufferStore(
			target,
			internalFormat,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			format,
			type,
			attachment,
			GL_LINEAR,
			GL_CLAMP_TO_EDGE,
			true
	);
}

void GeometryBuffer::bind() {
		defaultFBO->bind();
}

void GeometryBuffer::unbind() {
	defaultFBO->unbind();
}

void GeometryBuffer::outputDepthInfo() {
	//Copy depth data to the default framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultFBO->getHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	unsigned int windowWidth = Window::getCurrentInstance()->getSettings().windowWidth;
	unsigned int windowHeight = Window::getCurrentInstance()->getSettings().windowHeight;
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
