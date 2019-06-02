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
#include "../Window.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The FramebufferStore class
 *****************************************************************************/

void FramebufferStore::setup(GLuint fboTarget, bool multisample) {
	//Check for a render buffer object
	if (getParameters().getTarget() == GL_RENDERBUFFER) {
		//Setup the render buffer
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(getParameters().getTarget(), rbo);
		if (multisample)
			glRenderbufferStorageMultisample(getParameters().getTarget(), Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight());
		else
			glRenderbufferStorage(getParameters().getTarget(), internalFormat, getWidth(), getHeight());
		glBindRenderbuffer(getParameters().getTarget(), 0);

		//Attach to framebuffer
		glFramebufferRenderbuffer(fboTarget, attachment, getParameters().getTarget(), rbo);
	} else {
		//Setup the texture
		create();
		bind();
		if (multisample)
			glTexImage2DMultisample(getParameters().getTarget(), Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
		else
			glTexImage2D(getParameters().getTarget(), 0, internalFormat, getWidth(), getHeight(), 0, format, type, NULL);

		applyParameters(false);

		//Attach to framebuffer
		glFramebufferTexture2D(fboTarget, attachment, getParameters().getTarget(), getHandle(), 0);
	}
}

/*****************************************************************************
 * The FramebufferStoreCubemap class
 *****************************************************************************/

void FramebufferStoreCubemap::setup(GLuint fboTarget, bool multisample) {
	//Assume want to use framebuffer - target should be GL_TEXTURE_CUBE_MAP for applyParameters to work

	//Setup the texture
	create();
	bind();

	for (unsigned int i = 0; i < 6; ++i) {
		if (multisample)
			glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Window::getCurrentInstance()->getSettings().videoSamples, internalFormat, getWidth(), getHeight(), true);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, getWidth(), getHeight(), 0, format, type, NULL);
	}

	applyParameters(false, false);

	//Attach to framebuffer
	glFramebufferTexture(fboTarget, attachment, getHandle(), 0);
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
	for (unsigned int i = 0; i < stores.size(); i++) {
		//Setup the current texture
		stores[i]->setup(target, multisample);

		//Assume that if it is not a depth attachment then it is a colour attachment
		if (stores[i]->getAttachment() != GL_DEPTH_ATTACHMENT)
			colourAttachments.push_back(stores[i]->getAttachment());
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
	int status = glCheckFramebufferStatus(target);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		Logger::log("Framebuffer is not complete, current status: " + utils_string::str(status), "FramebufferObject", LogType::Error);

	//Bind the default FBO
	glBindFramebuffer(target, 0);
}

void FBO::copyTo(unsigned int fboHandle, GLenum sourceMode, GLenum destMode, int sourceWidth, int sourceHeight, int destX, int destY, int destWidth, int destHeight, GLbitfield mask) {
	//Copy the data from this FBO to the other one
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glReadBuffer(sourceMode);
	glDrawBuffer(destMode);
	glBlitFramebuffer(0, 0, sourceWidth, sourceHeight, destX, destY, destX + destWidth, destY + destHeight, mask, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::copyTo(FBO* fbo, unsigned int sourceStoreIndex, unsigned int destStoreIndex) {
	//Get the source store
	FramebufferStore* source = getFramebufferStore(sourceStoreIndex);
	//Get the destination store
	FramebufferStore* dest = fbo->getFramebufferStore(destStoreIndex);

	//Get the mask (assume if it isn't copying depth then it is copying colour)
	GLenum mask;
	if (source->getAttachment() == GL_DEPTH_ATTACHMENT)
		mask = GL_DEPTH_BUFFER_BIT;
	else
		mask = GL_COLOR_BUFFER_BIT;

	//Copy the data
	copyTo(fbo->getHandle(), source->getAttachment(), dest->getAttachment(), source->getWidth(), source->getHeight(), 0, 0, dest->getWidth(), dest->getHeight(), mask);
}

void FBO::copyToScreen(unsigned int sourceStoreIndex, int x, int y, int width, int height) {
	//Get the source store
	FramebufferStore* source = getFramebufferStore(sourceStoreIndex);

	//Copy the data
	copyTo(0, source->getAttachment(), GL_BACK, source->getWidth(), source->getHeight(), x, y, width, height, GL_COLOR_BUFFER_BIT);
}
