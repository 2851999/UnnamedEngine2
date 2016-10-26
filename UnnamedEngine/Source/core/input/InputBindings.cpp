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

#include <algorithm>

#include "../ml/ML.h"
#include "../Window.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The InputBinding class
 *****************************************************************************/

InputBinding::InputBinding(InputBindings* bindings) : bindings(bindings) {
	Window::getCurrentInstance()->getInputManager()->addListener(this);
}

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
	if (waitingForInput) {
		//Assign the key
		assignKey(key);
		//Notify the listeners
		bindings->callOnButtonAssigned(this);
		//Stop waiting for input
		waitingForInput = false;
	} else {
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
	if (waitingForInput) {
		//Assign the button
		assignMouseButton(button);
		//Notify the listeners
		bindings->callOnButtonAssigned(this);
		//Stop waiting for input
		waitingForInput = false;
	} else {
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
	if (waitingForInput) {
		//Assign the button
		assignControllerButton(controller, button);
		//Notify the listeners
		bindings->callOnButtonAssigned(this);
		//Stop waiting for input
		waitingForInput = false;
	} else {
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
	if (waitingForInput) {
		//Check what we were waiting for and assign the correct key
		if (waitingForPos) {
			keyboardKeyPositive = key;
			waitingForPos = false;
		} else if (waitingForNeg) {
			keyboardKeyNegative = key;
			waitingForNeg = false;
		}
		//Notify the listeners
		bindings->callOnAxisAssigned(this);
		//Stop waiting for input
		waitingForInput = waitingForNeg || waitingForPos;
	} else {
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
		//Assign the controller
		this->controller = controller;
		controllerAxis = axis;
		//Notify the listeners
		bindings->callOnAxisAssigned(this);
		//Stop waiting for input
		waitingForPos = false;
		waitingForNeg = false;
		waitingForInput = false;
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

InputBindingButton* InputBindings::getButtonBinding(std::string name) {
	//Ensure that binding exists
	if (buttonBindings.count(name) > 0)
		//Return the binding
		return buttonBindings.at(name);
	else {
		//Log an error
		Logger::log("No button binding with the name '" + name + "'", "InputBindings", Logger::Error);
		//Return
		return NULL;
	}
}

InputBindingAxis* InputBindings::getAxisBinding(std::string name) {
	//Ensure that binding exists
	if (axisBindings.count(name) > 0)
		//Return the binding
		return axisBindings.at(name);
	else {
		//Log an error
		Logger::log("No axis binding with the name '" + name + "'", "InputBindings", Logger::Error);
		//Return
		return NULL;
	}
}

void InputBindings::save(std::string path) {
	//Create the root of the document
	MLElement root("ml");

	//Go though each button binding
	for (auto& elem : buttonBindings) {
		//Get the current button binding
		InputBindingButton* button = elem.second;
		//Create the button binding
		MLElement buttonBinding("button");

		//Add the required attributes
		buttonBinding.add(MLAttribute("name", elem.first));
		buttonBinding.add(MLAttribute("keyboardKey", StrUtils::str(button->getKeyboardKey())));
		buttonBinding.add(MLAttribute("mouseButton", StrUtils::str(button->getMouseButton())));
		buttonBinding.add(MLAttribute("controllerButton", StrUtils::str(button->getControllerButton())));

		//Check whether a controller element is needed
		if (button->hasController()) {
			//Create the controller element
			MLElement controller("controller");

			//Add the required attributes
			controller.add(MLAttribute("index", StrUtils::str(button->getController()->getIndex())));
			controller.add(MLAttribute("name", button->getController()->getName()));

			//Add the controller element to the button binding
			buttonBinding.add(controller);
		}
		//Add the button binding to the root element
		root.add(buttonBinding);
	}

	//Go though each axis binding
	for (auto& elem : axisBindings) {
		//Get the current button binding
		InputBindingAxis* axis = elem.second;
		//Create the button binding
		MLElement axisBinding("axis");

		//Add the required attributes
		axisBinding.add(MLAttribute("name", elem.first));
		axisBinding.add(MLAttribute("keyboardKeyPos", StrUtils::str(axis->getKeyboardKeyPos())));
		axisBinding.add(MLAttribute("keyboardKeyNeg", StrUtils::str(axis->getKeyboardKeyNeg())));
		axisBinding.add(MLAttribute("controllerAxis", StrUtils::str(axis->getControllerAxis())));

		//Check whether a controller element is needed
		if (axis->hasController()) {
			//Create the controller element
			MLElement controller("controller");

			//Add the required attributes
			controller.add(MLAttribute("index", StrUtils::str(axis->getController()->getIndex())));
			controller.add(MLAttribute("name", axis->getController()->getName()));

			//Add the controller element to the axis binding
			axisBinding.add(controller);
		}
		//Add the axis binding to the root element
		root.add(axisBinding);
	}

	//The document to store the data in
	MLDocument document(root);
	//Save the document
	document.save(path);
}

void InputBindings::load(std::string path, InputManager* inputManager) {
	//Load the document
	MLDocument document;
	document.load(path);

	//Get the root element
	MLElement& root = document.getRoot();
	//The currently loaded controllers
	std::map<int, Controller*> controllers;
	//Go through all of the child elements within the root
	for (MLElement& child : root.getChildren()) {
		//The current controller
		Controller* currentController = NULL;

		//Go through each element within the child
		for (MLElement& elem : child.getChildren()) {
			//Check the name
			if (elem.getName() == "controller") {
				int index = -1;
				std::string name;

				for (MLAttribute& attrib : elem.getAttributes()) {
					if (attrib.getName() == "index")
						index = attrib.getDataAsInt();
					else if (attrib.getName() == "name")
						name = attrib.getData();
				}
				//Add the controller if required
				if (controllers.count(index) < 1) {
					int index = ControllerUtils::findController(index, name);
					if (index >= 0) {
						currentController = new Controller(index);
						controllers.insert(std::pair<int, Controller*>(index, currentController));
						//Add the controller to the input manager if it was provided
						if (inputManager)
							inputManager->addController(currentController);
					}
				} else
					currentController = controllers.at(index);
			}
		}

		//Check the child's name
		if (child.getName() == "button") {
			//Create the button binding
			InputBindingButton* buttonBinding = new InputBindingButton(this);

			//Go through each attribute and assign the appropriate values
			for (MLAttribute& attrib : child.getAttributes()) {
				if (attrib.getName() == "name")
					buttonBindings.insert(std::pair<std::string, InputBindingButton*>(attrib.getData(), buttonBinding));
				else if (attrib.getName() == "keyboardKey")
					buttonBinding->assignKey(attrib.getDataAsInt());
				else if (attrib.getName() == "mouseButton")
					buttonBinding->assignMouseButton(attrib.getDataAsInt());
				else if (attrib.getName() == "controllerButton" && currentController)
					buttonBinding->assignControllerButton(currentController, attrib.getDataAsInt());
			}
		} else if (child.getName() == "axis") {
			//Create the axis binding
			InputBindingAxis* axisBinding = new InputBindingAxis(this);

			//Go through each attribute and assign the appropriate values
			for (MLAttribute& attrib : child.getAttributes()) {
				if (attrib.getName() == "name")
					axisBindings.insert(std::pair<std::string, InputBindingAxis*>(attrib.getData(), axisBinding));
				else if (attrib.getName() == "keyboardKeyPos")
					axisBinding->assignKeyPos(attrib.getDataAsInt());
				else if (attrib.getName() == "keyboardKeyNeg")
					axisBinding->assignKeyNeg(attrib.getDataAsInt());
				else if (attrib.getName() == "controllerAxis" && currentController)
					axisBinding->assignControllerAxis(currentController, attrib.getDataAsInt());
			}
		}
	}
}

void InputBindings::addListener(InputBindingsListener* listener) {
	listeners.push_back(listener);
}

void InputBindings::removeListener(InputBindingsListener* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

