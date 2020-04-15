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

#pragma once

#include "FBO.h"
#include "Shader.h"

/*****************************************************************************
 * The PostProcessor class can render to a FBO and apply post processing
 * effects when rendering
 *****************************************************************************/

class PostProcessor {
private:
	/* The number of colour attachments in the FBO */
	unsigned int numColourAttachments;

	/* The frame buffer object used for rendering */
	FBO* fbo = NULL;

	/* The shader used when rendering the output */
	Shader* shader = NULL;
public:
	/* The constructors */
	PostProcessor(bool multisample, unsigned int numColourAttachments = 1);
	PostProcessor(const std::string& path, bool multisample, unsigned int numColourAttachments = 1);

	/* The destructor */
	virtual ~PostProcessor() { delete fbo; }

	/* This should be called before rendering to render to the FBO */
	void start();

	/* This should be called after rendering */
	void stop();

	/* This renders the result */
	void render();

	/* Copies the result to the default framebuffer using glBlitFramebuffer */
	void copyToScreen(GLbitfield mask);

	/* Copies the result to a specified framebuffer using glBlitFramebuffer */
	void copyToFramebuffer(FBO* drawFBO, GLbitfield mask);

	/* Getters */
	inline FBO* getFBO() { return fbo; }
	inline Shader* getShader() { return shader; }
};

