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

#ifndef CORE_INPUT_INPUTBINDINGS_H_
#define CORE_INPUT_INPUTBINDINGS_H_

#include <map>

#include "Input.h"

#include "../Window.h"

/*****************************************************************************
 * The InputBinding class
 *****************************************************************************/

class InputBindings;

class InputBinding : public InputListener {
protected:
	/* The input bindings instance this is attached to */
	InputBindings* bindings;
	/* Whether this binding is waiting for input to assign its values */
	bool waitingForInput = false;
public:
	/* The constructor */
	InputBinding(InputBindings* bindings) : bindings(bindings) { Window::getCurrentInstance()->getInputManager()->addListener(this); }

	/* The destructor */
	virtual ~InputBinding() {}

	/* Getters and setters */
	inline void waitForInput() { waitingForInput = true; }
	inline void cancelWaitForInput() { waitingForInput = false; }
	inline bool isWaitingForInput() { return waitingForInput; }

	/* Input methods */
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
	virtual void onControllerButtonPressed(Controller* controller, int button) override {}
	virtual void onControllerButtonReleased(Controller* controller, int button) override {}
};

/*****************************************************************************
 * The InputBindingButton class
 *****************************************************************************/

class InputBindingButton : public InputBinding {
private:
	/* A keyboard key */
	int keyboardKey = -1;

	/* A mouse button */
	int mouseButton = -1;

	/* A controller */
	Controller* controller = NULL;

	/* A controller button */
	int controllerButton = -1;

	/* States whether this button is currently pressed */
	bool pressed = false;
public:
	/* The constructor */
	InputBindingButton(InputBindings* bindings) : InputBinding(bindings) {}

	/* The destructor */
	virtual ~InputBindingButton() {}

	/* Various methods to manually assign the trigger for this button */
	inline void assignKey(int key) { keyboardKey = key; }
	inline void assignMouseButton(int button) { mouseButton = button; }
	inline void assignControllerButton(Controller* controller, int button) { this->controller = controller; controllerButton = button; }

	/* Getters */
	bool isPressed() { return pressed; }

	/* Input methods */
	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override;
	virtual void onMouseReleased(int button) override;
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) override {}
	virtual void onControllerButtonPressed(Controller* controller, int button) override;
	virtual void onControllerButtonReleased(Controller* controller, int button) override;
};

/*****************************************************************************
 * The InputBindingAxis class
 *****************************************************************************/

class InputBindingAxis : public InputBinding {
private:
	/* Keyboard keys */
	int keyboardKeyPositive = -1;
	int keyboardKeyNegative = -1;

	/* Controller axis */
	Controller* controller = NULL;
	int controllerAxis = -1;

	/* The current axis value */
	float value = 0;
public:
	/* The constructor */
	InputBindingAxis(InputBindings* bindings) : InputBinding(bindings) {}

	/* The destructor */
	virtual ~InputBindingAxis() {}

	/* Various methods to manually assign the triggers for this axis */
	inline void assignKeys(int keyPositive, int keyNegative) { keyboardKeyPositive = keyPositive; keyboardKeyNegative = keyNegative; }
	inline void assignControllerAxis(Controller* controller, int axis) { this->controller = controller; controllerAxis = axis; }

	/* Getters */
	float getValue() { return value; }

	/* Input methods */
	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override {}
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) override;
	virtual void onControllerButtonPressed(Controller* controller, int button) override {}
	virtual void onControllerButtonReleased(Controller* controller, int button) override {}
};

/*****************************************************************************
 * The InputBindingsListener class
 *****************************************************************************/

class InputBindingsListener {
public:
	/* The constructor */
	InputBindingsListener() {}

	/* The destructor */
	virtual ~InputBindingsListener() {}

	/* Called when a button is pressed/released */
	virtual void onButtonPressed(InputBindingButton* button) {}
	virtual void onButtonReleased(InputBindingButton* button) {}

	/* Called when an axis's value changes */
	virtual void onAxis(InputBindingAxis* axis) {}
};

/*****************************************************************************
 * The InputBindings class
 *****************************************************************************/

class InputBindings {
private:
	/* The button bindings */
	std::map<std::string, InputBindingButton*> buttonBindings;

	/* The axis bindings */
	std::map<std::string, InputBindingAxis*> axisBindings;

	/* The bindings listeners */
	std::vector<InputBindingsListener*> listeners;
public:
	/* The constructor */
	InputBindings() {}

	/* The destructor */
	virtual ~InputBindings();

	/* Method used to create and return a button binding */
	InputBindingButton* createButtonBinding(std::string name);

	/* Method used to create and return an axis binding */
	InputBindingAxis* createAxisBinding(std::string name);

	/* Method used to add a listener */
	inline void addListener(InputBindingsListener* listener) { listeners.push_back(listener); }

	/* Method used to remove a listener */
	inline void removeListener(InputBindingsListener* listener) { listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end()); }

	/* Methods used to call listener events */
	inline void callOnButtonPressed(InputBindingButton* button) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners[i]->onButtonPressed(button);
	}

	inline void callOnButtonReleased(InputBindingButton* button) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners[i]->onButtonReleased(button);
	}

	inline void callOnAxis(InputBindingAxis* axis) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners[i]->onAxis(axis);
	}
};

#endif /* CORE_INPUT_INPUTBINDINGS_H_ */
