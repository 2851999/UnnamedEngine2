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

#ifndef UTILS_GLUTILS_H_
#define UTILS_GLUTILS_H_

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*****************************************************************************
 * Various OpenGL utilities
 *****************************************************************************/

namespace utils_gl {
	/* Various setup methods for rendering */
	inline void clearColourBuffer()  { glClear(GL_COLOR_BUFFER_BIT); }
	inline void clearDepthBuffer()   { glClear(GL_DEPTH_BUFFER_BIT); }
	inline void clearColourAndDepthBuffer() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
	inline void setupAlphaBlending() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	inline void setupAlphaBlendingMSAA() {
		glEnable(GL_MULTISAMPLE_ARB);
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
	}
	inline void enableWireframe()    { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	inline void disableWireframe()   { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
	inline void enableDepthTest()    { glEnable(GL_DEPTH_TEST); }
	inline void disableDepthTest()   { glDisable(GL_DEPTH_TEST); }

	inline void setupSimple2DView(bool msaa = false) {
		clearColourBuffer();
		if (msaa)
			setupAlphaBlendingMSAA();
		else
			setupAlphaBlending();
	}

	inline void setupSimple3DView(bool msaa = false) {
		clearColourAndDepthBuffer();
		enableDepthTest();
		if (msaa)
			setupAlphaBlendingMSAA();
		else
			setupAlphaBlending();
	}
}



#endif /* UTILS_GLUTILS_H_ */
