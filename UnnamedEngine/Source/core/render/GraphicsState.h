/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#ifndef CORE_RENDER_GRAPHICSSTATE_H_
#define CORE_RENDER_GRAPHICSSTATE_H_

/*****************************************************************************
 * The GraphicsState class contains information about what should be used
 * while rendering for both OpenGL and Vulkan e.g. alpha blending
 *****************************************************************************/

class GraphicsState {
public:
	/* Various states that this class handles */
	bool depthWriteEnable = true;

	/* Constructor */
	GraphicsState() {}

	/* Destructor */
	virtual ~GraphicsState() {}

	/* Method to apply the state given the old one (OpenGL) */
	void applyGL(GraphicsState* old);
};


#endif /* CORE_RENDER_GRAPHICSSTATE_H_ */
