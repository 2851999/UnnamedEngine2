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

/*****************************************************************************
 * The PostProcessor class
 *****************************************************************************/

PostProcessor::PostProcessor(std::string path) {
	fbo = new FBO(GL_FRAMEBUFFER);
	fbo->attach(new FramebufferTexture(
			GL_TEXTURE_2D,
			GL_RGBA16F,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			GL_RGBA,
			GL_FLOAT,
			GL_COLOR_ATTACHMENT0
	));

	fbo->attach(new FramebufferTexture(
			GL_RENDERBUFFER,
			GL_DEPTH_COMPONENT32,
			Window::getCurrentInstance()->getSettings().windowWidth,
			Window::getCurrentInstance()->getSettings().windowHeight,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			GL_DEPTH_ATTACHMENT
	));

	fbo->setup();

	shader = Shader::createShader(Shader::loadShaderSource(path + ".vs"), Shader::loadShaderSource(path + ".fs"));
	shader->addAttribute("Position", "position");
	shader->addAttribute("TextureCoordinate", "textureCoord");
	shader->addUniform("Texture", "tex");
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
	Renderer::render(fbo->getFramebufferTexture(0), shader);
}
