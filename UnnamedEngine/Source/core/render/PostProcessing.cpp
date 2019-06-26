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

#include "PostProcessing.h"
#include "Renderer.h"
#include "../Window.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The PostProcessor class
 *****************************************************************************/

PostProcessor::PostProcessor(bool multisample) {
	fbo = new FBO(GL_FRAMEBUFFER, multisample);
	fbo->attach(new FramebufferStore(
			multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
			GL_RGBA16F,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			GL_RGBA,
			GL_FLOAT,
			GL_COLOR_ATTACHMENT0,
			GL_NEAREST,
			GL_CLAMP_TO_EDGE,
			true
	));

	fbo->attach(new FramebufferStore(
			GL_RENDERBUFFER,
			GL_DEPTH_COMPONENT32,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			GL_DEPTH_COMPONENT,
			GL_UNSIGNED_INT,
			GL_DEPTH_ATTACHMENT,
			GL_NEAREST,
			GL_CLAMP_TO_EDGE,
			true
	));

	fbo->setup();
}

PostProcessor::PostProcessor(std::string path, bool multisampling) : PostProcessor(multisampling) {
	shader = Shader::createShader(Shader::loadShaderSource(path + ".vs"), Shader::loadShaderSource(path + ".fs"));
}

void PostProcessor::start() {
	fbo->bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void PostProcessor::stop() {
	fbo->unbind();
}

void PostProcessor::render() {
	Renderer::render(fbo->getFramebufferStore(0), shader);
}

void PostProcessor::copyToScreen(GLbitfield mask) {
	unsigned int width = Window::getCurrentInstance()->getSettings().windowWidth;
	unsigned int height = Window::getCurrentInstance()->getSettings().windowHeight;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getHandle());
	glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::copyToFramebuffer(FBO* drawFBO, GLbitfield mask) {
	unsigned int width = Window::getCurrentInstance()->getSettings().windowWidth;
	unsigned int height = Window::getCurrentInstance()->getSettings().windowHeight;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO->getHandle());
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getHandle());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
