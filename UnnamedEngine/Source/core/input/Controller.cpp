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

#include "Controller.h"

#include "../Window.h"

/*****************************************************************************
 * The Controller class
 *****************************************************************************/

Controller::Controller(int index) {
	this->index = index;
	this->name = std::string(glfwGetJoystickName(index));

	//Get the current states
	currentAxisValues   = glfwGetJoystickAxes(index, &axisCount);
	currentButtonValues = glfwGetJoystickButtons(index, &buttonCount);

	//Assign the current states
	for (int i = 0; i < axisCount; i++)
		axisValues.push_back(currentAxisValues[i]);
	for (int i = 0; i < buttonCount; i++)
		buttonValues.push_back(currentButtonValues[i]);
}

void Controller::checkInput() {
	//Get the current states - seems these methods actually fetch the values
	//from the joystick and then puts it into the dynamic array assigned
	//when the controller was created
	glfwGetJoystickAxes(index, &axisCount);
	glfwGetJoystickButtons(index, &buttonCount);

	//Check for changes and then change any states necessary
	for (int i = 0; i < axisCount; i++) {
		if (axisValues[i] != currentAxisValues[i]) {
			Window::getCurrentInstance()->getInputManager()->callOnControllerAxis(this, i, currentAxisValues[i]);

			axisValues[i] = currentAxisValues[i];
		}
	}

	for (int i = 0; i < buttonCount; i++) {
		if (buttonValues[i] != currentButtonValues[i]) {
			if (currentButtonValues[i] == GLFW_PRESS)
				Window::getCurrentInstance()->getInputManager()->callOnControllerButtonPressed(this, i);
			else if (currentButtonValues[i] == GLFW_RELEASE)
				Window::getCurrentInstance()->getInputManager()->callOnControllerButtonReleased(this, i);

			buttonValues[i] = currentButtonValues[i];
		}
	}
}

bool Controller::getButton(int index) { return buttonValues[index] == GLFW_PRESS; }
