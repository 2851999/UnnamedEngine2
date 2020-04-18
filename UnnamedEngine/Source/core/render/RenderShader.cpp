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

#include "RenderShader.h"

#include "RenderPipeline.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The RenderShader class
 *****************************************************************************/

RenderShader::RenderShader(unsigned int id, Shader* shader) : id(id), shader(shader) {
	//Create the pipeline layout
	pipelineLayout = new RenderPipelineLayout();
}

RenderShader::~RenderShader() {
	delete pipelineLayout;
}

void RenderShader::setup() {
	//Setup the pipeline layout for this render shader
	pipelineLayout->setup(this);
}
