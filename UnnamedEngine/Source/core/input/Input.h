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

#ifndef CORE_INPUT_INPUT_H_
#define CORE_INPUT_INPUT_H_

#include <vector>
#include <algorithm>

#include "Controller.h"

class Window;

/*****************************************************************************
 * The InputListener class
 *****************************************************************************/

class InputListener {
public:
	InputListener() {}
	virtual ~InputListener() {}

	/* Input methods */
	virtual void onKeyPressed(int key) {}
	virtual void onKeyReleased(int key) {}
	virtual void onChar(int key, char character) {}

	virtual void onMousePressed(int button) {}
	virtual void onMouseReleased(int button) {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) {}
	virtual void onMouseEnter() {}
	virtual void onMouseLeave() {}

	virtual void onScroll(double dx, double dy) {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) {}
	virtual void onControllerButtonPressed(Controller* controller, int button) {}
	virtual void onControllerButtonReleased(Controller* controller, int button) {}
};

/*****************************************************************************
 * The InputManager class manages input for a window
 *****************************************************************************/

class InputManager {
public:
	/* The CursorData structure to keep track of the cursor */
	struct CursorData {
		double lastX = -1;
		double lastY = -1;
	};
private:
	/* All of the input listeners */
	std::vector<InputListener*> listeners;

	/* The window instance this is attached to */
	Window* window;

	/* The cursor */
	CursorData cursorData;

	/* All of the added controllers */
	std::vector<Controller*> controllers;
public:
	InputManager(Window* window) { this->window = window; }
	virtual ~InputManager() {}

	/* Various input methods */
	void addListener(InputListener* listener) { listeners.push_back(listener); }
	void removeListener(InputListener* listener) { listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end()); }

	/* Updates the attached controllers */
	void updateControllers() {
		for (unsigned int i = 0; i < controllers.size(); i++)
			controllers[i]->checkInput();
	}

	/* Used to add a controller */
	void addController(Controller* controller) {
		controllers.push_back(controller);
	}

	/* Returns the CursorData structure */
	CursorData& getCursorData() { return cursorData; }

	/* Various window specific callbacks */
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int codepoint);
	void cursorPosCallback(double xpos, double ypos);
	void cursorEnterCallback(int entered);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);

	/* Methods used to call methods within InputListeners */
	void callOnKeyPressed(int key) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onKeyPressed(key);
	}

	void callOnKeyReleased(int key) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onKeyReleased(key);
	}

	void callOnChar(int key, char character) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onChar(key, character);
	}

	void callOnMousePressed(int button) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onMousePressed(button);
	}

	void callOnMouseReleased(int button) {
		for (unsigned int i = 0; i  < listeners.size(); i++)
			listeners.at(i)->onMouseReleased(button);
	}

	void callOnMouseMoved(double x, double y, double dx, double dy) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onMouseMoved(x, y, dx, dy);
	}

	void callOnMouseDragged(double x, double y, double dx, double dy) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onMouseDragged(x, y, dx, dy);
	}

	void callOnMouseEnter() {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onMouseEnter();
	}

	void callOnMouseLeave() {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onMouseLeave();
	}

	void callOnScroll(double x, double y) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onScroll(x, y);
	}

	void callOnControllerAxis(Controller* controller, int axis, float value) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onControllerAxis(controller, axis, value);
	}

	void callOnControllerButtonPressed(Controller* controller, int button) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onControllerButtonPressed(controller, button);
	}

	void callOnControllerButtonReleased(Controller* controller, int button) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners.at(i)->onControllerButtonReleased(controller, button);
	}
};


#endif /* CORE_INPUT_INPUT_H_ */
