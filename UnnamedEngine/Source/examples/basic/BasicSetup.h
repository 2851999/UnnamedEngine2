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

#ifndef EXAMPLES_BASIC_BASICSETUP_H_
#define EXAMPLES_BASIC_BASICSETUP_H_

/*****************************************************************************
 * The Basic Setup Example Program
 *
 * Tutorial Number: 1
 * Tutorial Name: Basic Setup
 * Engine Version: V0.1.0 (0::1::2::3)
 * Date Created: 16/10/2016
 * Date Updated: 16/10/2016
 *
 * Description: Demonstrates the setup process to use the engine.
 *****************************************************************************/

/* Include all of the headers needed */
#include "../../core/BaseEngine.h"
#include "../../utils/GLUtils.h"

/* Here is the definition of the main class which is able to receive calls
 * to engine methods by extending the BaseEngine class */
class Tutorial : public BaseEngine {
public:
	/* In order to get calls from the game loop, the following methods can
	 * be overridden as shown below */

	/* This method is called to setup things such as Settings, which are
	 * used as the engine starts */
	void initialise() override;

	/* This method is called once the Window has been created with an
	 * OpenGL context - so all game resources can be loaded once this is
	 * called */
	void created() override;

	/* This method is called in the game loop to update the game */
	void update() override;

	/* This method is called in the game loop to render the game */
	void render() override;

	/* This method is called when the window is about to close, and so
	 * should release all created resources */
	void destroy() override;
};

/* Now to actually write the methods declared above */

void Tutorial::initialise() {
	//At this point, the Window and OpenGL context has not been created,
	//so nothing to do with rendering can be done here, but it can be
	//used to setup the window settings for when it is created

	//Assign some Settings
	getSettings().windowTitle = "Tutorial 1 - Basic Setup"; //The title of the window
	getSettings().videoResolution = VideoResolution::RES_1280x720; //The video resolution
	getSettings().debuggingShowInformation = true; //Shows some engine/performance data

	//The video resolution is used to determine the size of the window
	//whether it is full screen or not, and when the window is actually
	//created, the windowWidth and windowHeight values are assigned to
	//provide easier access to the actual window size
}

void Tutorial::created() {}

void Tutorial::update() {}

void Tutorial::render() {
	//When rendering some OpenGL stuff needs to be performed, but
	//in this case the below can be used so we do not need to worry
	//about it
	GLUtils::setupSimple2DView();
}

void Tutorial::destroy() {}

/*
 * The simplest way to execute the above code is to use the code below:

int main() {
	Tutorial basicSetup;
	basicSetup.create();

	return 0;
}

 */

#endif /* EXAMPLES_BASIC_BASICSETUP_H_ */
