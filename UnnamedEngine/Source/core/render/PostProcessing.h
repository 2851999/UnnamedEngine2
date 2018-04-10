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

#ifndef CORE_RENDER_POSTPROCESSING_H_
#define CORE_RENDER_POSTPROCESSING_H_

#include "FBO.h"
#include "Shader.h"

/*****************************************************************************
 * The PostProcessor class can render to a FBO and apply post processing
 * effects when rendering
 *****************************************************************************/

class PostProcessor {
private:
	/* The frame buffer object used for rendering */
	FBO* fbo;

	/* The shader used when rendering the output */
	Shader* shader;
public:
	/* The constructor */
	PostProcessor(std::string path);

	/* The destructor */
	virtual ~PostProcessor() { delete fbo; }

	/* This should be called before rendering to render to the FBO */
	void start();

	/* This should be called after rendering */
	void stop();

	/* This renders the result */
	void render();
};


#endif /* CORE_RENDER_POSTPROCESSING_H_ */
