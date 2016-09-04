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

#include "InputBindings.h"

/*****************************************************************************
 * The InputBindingButton class
 *****************************************************************************/

void InputBindingButton::onKeyPressed(int key) {
	//Check whether this binding button should be triggered
	if (keyboardKey == key) {
		pressed = true;
		bindings->callOnButtonPressed(this);
	}
}

void InputBindingButton::onKeyReleased(int key) {
	//Check whether waiting for input
	if (waitingForInput)
		//Assign the key
		assignKey(key);
	else {
		//Check whether this binding button should be triggered
		if (keyboardKey == key) {
			pressed = false;
			bindings->callOnButtonReleased(this);
		}
	}
}

void InputBindingButton::onMousePressed(int button) {
	//Check whether this binding button should be triggered
	if (mouseButton == button) {
		pressed = true;
		bindings->callOnButtonPressed(this);
	}
}

void InputBindingButton::onMouseReleased(int button) {
	//Check whether waiting for input
	if (waitingForInput)
		//Assign the button
		assignMouseButton(button);
	else {
		//Check whether this binding button should be triggered
		if (mouseButton == button) {
			pressed = false;
			bindings->callOnButtonReleased(this);
		}
	}
}

void InputBindingButton::onControllerButtonPressed(Controller* controller, int button) {
	//Check whether this binding button should be triggered
	if (this->controller == controller && controllerButton == button) {
		pressed = true;
		bindings->callOnButtonPressed(this);
	}
}

void InputBindingButton::onControllerButtonReleased(Controller* controller, int button) {
	//Check whether waiting for input
	if (waitingForInput)
		//Assign the button
		assignControllerButton(controller, button);
	else {
		//Check whether this binding button should be triggered
		if (this->controller == controller && controllerButton == button) {
			pressed = false;
			bindings->callOnButtonReleased(this);
		}
	}
}

/*****************************************************************************
 * The InputBindingAxis class
 *****************************************************************************/

void InputBindingAxis::onKeyPressed(int key) {
	if (! waitingForInput) {
		if (key == keyboardKeyPositive) {
			value = 1.0f;
			keyboardKeyPosDown = true;
			bindings->callOnAxis(this);
		} else if (key == keyboardKeyNegative) {
			value = -1.0f;
			keyboardKeyNegDown = true;
			bindings->callOnAxis(this);
		}
	}
}

void InputBindingAxis::onKeyReleased(int key) {
	if (! waitingForInput) {
		if (key == keyboardKeyPositive) {
			//Check whether other direction's key down
			if (keyboardKeyNegDown)
				value = -1.0f;
			else
				value = 0.0f;
			keyboardKeyPosDown = false;
			bindings->callOnAxis(this);
		} else if (key == keyboardKeyNegative) {
			//Check whether other direction's key down
			if (keyboardKeyPosDown)
				value = 1.0f;
			else
				value = 0.0f;
			keyboardKeyNegDown = false;
			bindings->callOnAxis(this);
		}
	}
}

void InputBindingAxis::onControllerAxis(Controller* controller, int axis, float value) {
	if (waitingForInput) {
		this->controller = controller;
		controllerAxis = axis;
	} else {
		if (this->controller == controller && controllerAxis == axis) {
			this->value = -value;
			bindings->callOnAxis(this);
		}
	}
}

/*****************************************************************************
 * The InputBindings class
 *****************************************************************************/

InputBindings::~InputBindings() {
	for (auto& iterator : buttonBindings)
		delete iterator.second;
	buttonBindings.clear();
	listeners.clear();
}

InputBindingButton* InputBindings::createButtonBinding(std::string name) {
	//Create the binding button
	InputBindingButton* button = new InputBindingButton(this);
	//Add the button to the buttons map
	buttonBindings.insert(std::pair<std::string, InputBindingButton*>(name, button));
	//Return the button binding
	return button;
}

InputBindingAxis* InputBindings::createAxisBinding(std::string name) {
	//Create the binding axis
	InputBindingAxis* axis = new InputBindingAxis(this);
	//Add the button to the axis map
	axisBindings.insert(std::pair<std::string, InputBindingAxis*>(name, axis));
	//Return the button binding
	return axis;
}
