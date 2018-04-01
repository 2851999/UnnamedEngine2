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

#include "FBO.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The FramebufferTexture class
 *****************************************************************************/

void FramebufferTexture::setup(GLuint fboTarget) {
	//Check for a render buffer object
	if (getParameters().getTarget() == GL_RENDERBUFFER) {
		//Setup the render buffer
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(getParameters().getTarget(), rbo);
		glRenderbufferStorage(getParameters().getTarget(), internalFormat, getWidth(), getHeight());
		glBindRenderbuffer(getParameters().getTarget(), 0);

		//Attach to framebuffer
		glFramebufferRenderbuffer(fboTarget, attachment, getParameters().getTarget(), rbo);
	} else {
		//Setup the texture
		create();
		bind();
		glTexImage2D(getParameters().getTarget(), 0, internalFormat, getWidth(), getHeight(), 0, format, type, NULL);
		applyParameters(true);

		//Attach to framebuffer
		glFramebufferTexture2D(fboTarget, attachment, getParameters().getTarget(), getHandle(), 0);
	}
}

/*****************************************************************************
 * The FBO class
 *****************************************************************************/

void FBO::setup() {
	//Generate and bind the FBO
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(target, framebuffer);

	//List of all of the colour attachments being used
	std::vector<unsigned int> colourAttachments;

	//Go though each attached FramebufferTexture
	for (unsigned int i = 0; i < textures.size(); i++) {
		//Setup the current texture
		textures[i]->setup(target);

		//Assume that if it is not a depth attachment then it is a colour attachment
		if (textures[i]->getAttachment() != GL_DEPTH_ATTACHMENT)
			colourAttachments.push_back(textures[i]->getAttachment());
	}

	if (colourAttachments.size() == 0) {
		//No colour attachment
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	} else {
		unsigned int attachments[colourAttachments.size()];
		for (unsigned int i = 0; i < colourAttachments.size(); i++)
			attachments[i] = colourAttachments[i];
		glDrawBuffers(colourAttachments.size(), attachments);
	}

	//Check to see whether the setup was successful
	if (glCheckFramebufferStatus(target) != GL_FRAMEBUFFER_COMPLETE)
		Logger::log("Framebuffer is not complete", "FramebufferObject", LogType::Error);

	//Bind the default FBO
	glBindFramebuffer(target, 0);
}
