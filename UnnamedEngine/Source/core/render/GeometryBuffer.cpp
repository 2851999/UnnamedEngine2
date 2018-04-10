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

GeometryBuffer::GeometryBuffer(bool pbr) : FBO(GL_FRAMEBUFFER) {
	//Attach the buffers

	//Position buffer
	attach(createBuffer(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0));

	//Normal buffer
	attach(createBuffer(GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT1));

	//Albedo buffer
	attach(createBuffer(GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2));

	//Extra buffer for metalness and ao components for PBR
	if (pbr)
		attach(createBuffer(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT3));

	//Depth buffer
	attach(createBuffer(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT));

	//Setup this buffer
	setup();
}

FramebufferTexture* GeometryBuffer::createBuffer(GLenum target, GLint internalFormat, GLenum format, GLenum type, GLenum attachment) {
	return new FramebufferTexture(
			target,
			internalFormat,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			format,
			type,
			attachment,
			GL_NEAREST,
			GL_CLAMP_TO_EDGE,
			false
	);
}
