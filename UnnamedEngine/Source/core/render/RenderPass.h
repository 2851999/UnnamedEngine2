/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#pragma once

 /*****************************************************************************
  * The RenderPass class handles a specific render pass used when rendering
  *****************************************************************************/

class RenderPass {
private:
	/* The Vulkan instance */
	VkRenderPass vulkanInstance = VK_NULL_HANDLE;

	/* Framebuffer object for rendering this render pass to,
	   if NULL assumes default framebuffer should be used */
	FBO* fbo = NULL;

	/* The number of samples this render pass uses */
	unsigned int numSamples = 0;
public:
	/* Constructor with the FBO to use for rendering */
	RenderPass(FBO* fbo = NULL);

	/* Destructor */
	virtual ~RenderPass();

	/* Method to begin this render pass */
	void begin();

	/* Method to end this render pass */
	void end();

	/* Getters */
	inline VkRenderPass& getVkInstance() { return vulkanInstance; }
	inline FBO* getFBO() { return fbo; }
	inline unsigned int getNumSamples() { return numSamples; }
};
