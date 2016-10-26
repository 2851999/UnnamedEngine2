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

#include "Controller.h"
#include "../Vector.h"

class Window;

/*****************************************************************************
 * The Mouse class
 *****************************************************************************/

class Mouse {
public:
	/* Returns whether a certain mouse button is pressed on the currently
	 * active window */
	static bool isPressed(int button);
	/* Returns the mouse/cursor position on the currently active window */
	static Vector2d getPosition();
};

/*****************************************************************************
 * The Keyboard class
 *****************************************************************************/

class Keyboard {
public:
	/* Returns whether a certain key is pressed on the currently active
	 * window */
	static bool isPressed(int key);
};

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
	/* The constructor */
	InputManager(Window* window) { this->window = window; }

	/* The destructor */
	virtual ~InputManager() {}

	/* Various input methods */
	void addListener(InputListener* listener);
	void removeListener(InputListener* listener);

	/* Updates the attached controllers */
	void updateControllers();

	/* Method used to add a controller */
	void addController(Controller* controller);

	/* Method used to remove a controller */
	void removeController(Controller* controller);

	/* Method used to release all of the controllers */
	void releaseControllers();

	/* Returns the CursorData structure */
	inline CursorData& getCursorData() { return cursorData; }

	/* Various window specific callbacks */
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int codepoint);
	void cursorPosCallback(double xpos, double ypos);
	void cursorEnterCallback(int entered);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);

	/* Methods used to call methods within InputListeners */
	void callOnKeyPressed(int key);
	void callOnKeyReleased(int key);
	void callOnChar(int key, char character);
	void callOnMousePressed(int button);
	void callOnMouseReleased(int button);
	void callOnMouseMoved(double x, double y, double dx, double dy);
	void callOnMouseDragged(double x, double y, double dx, double dy);
	void callOnMouseEnter();
	void callOnMouseLeave();
	void callOnScroll(double x, double y);
	void callOnControllerAxis(Controller* controller, int axis, float value);
	void callOnControllerButtonPressed(Controller* controller, int button);
	void callOnControllerButtonReleased(Controller* controller, int button);
};


#endif /* CORE_INPUT_INPUT_H_ */
