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
		//Ensure the value is other a certain threshold - prevents random noise from
		//causing an assignment
		if (utils_maths::abs(value) >= 0.2f) {
			//Assign the controller
			this->controller = controller;
			controllerAxis = axis;
			//Notify the listeners
			bindings->callOnAxisAssigned(this);
			//Stop waiting for input
			waitingForPos = false;
			waitingForNeg = false;
			waitingForInput = false;
		}
	} else {
		if (this->controller == controller && controllerAxis == axis) {
			//Check whether the value should be assigned
			if (utils_maths::abs(value) > controllerAxisDeadZone) {
				if (controllerAxisInverted)
					this->value = -value;
				else
					this->value = value;
				bindings->callOnAxis(this);
			} else
				this->value = 0;
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
	auto buttonBindingIt = buttonBindings.find(name);
	if (buttonBindingIt != buttonBindings.end())
		//Return the binding
		return buttonBindingIt->second;
	else {
		//Log an error
		Logger::log("No button binding with the name '" + name + "'", "InputBindings", LogType::Error);
		//Return
		return NULL;
	}
}

InputBindingAxis* InputBindings::getAxisBinding(std::string name) {
	//Ensure that binding exists
	auto axisBindingIt = axisBindings.find(name);
	if (axisBindingIt != axisBindings.end())
		//Return the binding
		return axisBindingIt->second;
	else {
		//Log an error
		Logger::log("No axis binding with the name '" + name + "'", "InputBindings", LogType::Error);
		//Return
		return NULL;
	}
}

void InputBindings::save(MLDocument& document) {
	//Create the input bindings element
	MLElement inputBindingsElement("input_bindings");

	//Go though each button binding
	for (auto& elem : buttonBindings) {
		//Get the current button binding
		InputBindingButton* button = elem.second;
		//Create the button binding
		MLElement buttonBinding("button");

		//Add the required attributes
		buttonBinding.add(MLAttribute("name", elem.first));
		buttonBinding.add(MLAttribute("keyboardKey", utils_string::str(button->getKeyboardKey())));
		buttonBinding.add(MLAttribute("mouseButton", utils_string::str(button->getMouseButton())));
		buttonBinding.add(MLAttribute("controllerButton", utils_string::str(button->getControllerButton())));

		//Check whether a controller element is needed
		if (button->hasController()) {
			//Create the controller element
			MLElement controller("controller");

			//Add the required attributes
			controller.add(MLAttribute("index", utils_string::str(button->getController()->getIndex())));
			controller.add(MLAttribute("name", button->getController()->getName()));

			//Add the controller element to the button binding
			buttonBinding.add(controller);
		}
		//Add the button binding to the root element
		inputBindingsElement.add(buttonBinding);
	}

	//Go though each axis binding
	for (auto& elem : axisBindings) {
		//Get the current button binding
		InputBindingAxis* axis = elem.second;
		//Create the button binding
		MLElement axisBinding("axis");

		//Add the required attributes
		axisBinding.add(MLAttribute("name", elem.first));
		axisBinding.add(MLAttribute("keyboardKeyPos", utils_string::str(axis->getKeyboardKeyPos())));
		axisBinding.add(MLAttribute("keyboardKeyNeg", utils_string::str(axis->getKeyboardKeyNeg())));
		axisBinding.add(MLAttribute("controllerAxis", utils_string::str(axis->getControllerAxis())));
		axisBinding.add(MLAttribute("controllerAxisInverted", utils_string::str(axis->getControllerAxisInverted())));
		axisBinding.add(MLAttribute("controllerAxisDeadZone", utils_string::str(axis->getControllerAxisDeadZone())));

		//Check whether a controller element is needed
		if (axis->hasController()) {
			//Create the controller element
			MLElement controller("controller");

			//Add the required attributes
			controller.add(MLAttribute("index", utils_string::str(axis->getController()->getIndex())));
			controller.add(MLAttribute("name", axis->getController()->getName()));

			//Add the controller element to the axis binding
			axisBinding.add(controller);
		}
		//Add the axis binding to the root element
		inputBindingsElement.add(axisBinding);
	}

	//Add the input bindings element, or replace the current one if one already exists
	int index = document.getRoot().findChild("input_bindings");
	if (index >= 0)
		document.getRoot().setChild(index, inputBindingsElement);
	else
		document.getRoot().add(inputBindingsElement);
}

void InputBindings::save(std::string path) {
	//Create the document
	MLDocument document(MLElement("ml"));
	//Write the settings to the document
	save(document);
	//Save the document
	document.save(path);
}

void InputBindings::load(MLDocument& document, InputManager* inputManager) {
	//Attempt to get the index of the settings element in the document
	int index = document.getRoot().findChild("input_bindings");
	//Check whether the settings element was found
	if (index >= 0) {
		//The currently loaded controllers
		std::unordered_map<int, Controller*> controllers;
		//Go through all of the child elements within the root
		for (MLElement& child : document.getRoot().getChild(index).getChildren()) {
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
					//Get the controller index
					index = utils_controller::findController(index, name);
					//Add the controller if found
					if (index > -1 ) {
						if (controllers.count(index) < 1) {
							currentController = new Controller(index);
							controllers.insert(std::pair<int, Controller*>(index, currentController));
							//Add the controller to the input manager if it was provided
							if (inputManager)
								inputManager->addController(currentController);
						} else
							currentController = controllers.at(index);
					}
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
					else if (attrib.getName() == "controllerButton")
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
					else if (attrib.getName() == "controllerAxis")
						axisBinding->assignControllerAxis(currentController, attrib.getDataAsInt());
					else if (attrib.getName() == "controllerAxisInverted")
						axisBinding->setControllerAxisInverted(attrib.getDataAsBool());
					else if (attrib.getName() == "controllerAxisDeadZone")
						axisBinding->setControllerAxisDeadZone(attrib.getDataAsFloat());
				}
			}
		}
	} else
		//Log an error
		Logger::log("InputBindings not found in the document", "InputBindings", LogType::Error);
}

void InputBindings::load(std::string path, InputManager* inputManager) {
	//Load the document
	MLDocument document;
	document.load(path);
	//Load the input bindings
	load(document, inputManager);
}

void InputBindings::addListener(InputBindingsListener* listener) {
	listeners.push_back(listener);
}

void InputBindings::removeListener(InputBindingsListener* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

