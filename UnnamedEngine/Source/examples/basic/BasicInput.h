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

#ifndef EXAMPLES_BASIC_BASICINPUT_H_
#define EXAMPLES_BASIC_BASICINPUT_H_

/*****************************************************************************
 * The Basic Input Example Program
 *
 * Tutorial Number: 2
 * Tutorial Name: Input
 * Engine Version: V0.2.9 (1::0::6::4)
 * Date Created: 17/10/2016
 * Date Updated: 13/04/2018
 *
 * Description: Demonstrates how to receive user input.
 *****************************************************************************/

/* Include all of the headers needed */
#include "../../core/BaseEngine.h"
#include "../../utils/GLUtils.h"

/* There are two kinds of input methods available:
 * 	- Event driven
 * 	- State driven
 *
 * Event driven involves overriding methods from InputListener and then
 * using these methods to process the input. The second method (State driven)
 * uses methods in the Mouse and Keyboard classes to check
 * specific states.
 *
 * For event driven, you should also inherit from InputListener, but
 * BaseEngine does this for us, so it isn't needed here
 */
class Tutorial : public BaseEngine {
public:
	/* Various methods seen in the last tutorial */
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;

	/* Input methods from InputListener - used for event-driven input */
	virtual void onKeyPressed(int key) override {}
	virtual void onKeyReleased(int key) override {}
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override {}
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) override {}
	virtual void onControllerButtonPressed(Controller* controller, int index) override {}
	virtual void onControllerButtonReleased(Controller* controller, int index) override {}
};

/* Now to actually write the methods declared above */

void Tutorial::initialise() {
	//Assign some Settings
	getSettings().windowTitle = "Tutorial 2 - Basic Input";
	getSettings().videoResolution = VideoResolution::RES_1280x720;
	getSettings().debugShowInformation = true;
}

void Tutorial::created() {
	//The below code is to add an input listener to the input manager
	//attached to the window, this only needs to be called if you are
	//inheriting InputListener directly
	//getWindow()->getInputManager()->addListener(this);

	//For controllers, things can be a bit more complicated but here is an example
	//of finding and creating a controller instance, and setting it up for event-driven
	//input
	/*
	Controller* controller = new Controller(ControllerUtils::getControllerIndexByName("INSERT_NAME_HERE"));
	getWindow()->getInputManager()->addController(controller);
	//For state-driven, you can simply use various other methods e.g.
	controller->getAxis(0); //Returns the value of axis 0
	controller->getButton(0); //Returns the current state of the button 0
	*/
	//Controller input is made simpler using InputBindings, which can also allow
	//saving and loading of input settings to a file, but this is not shown
	//here
}

void Tutorial::update() {
	//Here is an example of state-driven input
	if (Keyboard::isPressed(GLFW_KEY_ESCAPE))
		//Stop the game engine
		requestClose();
}

void Tutorial::render() {
	//Seen from the last tutorial
	utils_gl::setupSimple2DView();
}

#endif /* EXAMPLES_BASIC_BASICINPUT_H_ */
