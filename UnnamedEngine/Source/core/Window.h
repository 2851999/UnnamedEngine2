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

#ifndef CORE_WINDOW_H_
#define CORE_WINDOW_H_

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Settings.h"
#include "input/Input.h"

/*****************************************************************************
 * The Window class
 *****************************************************************************/

class Window {
private:
	/* The GLFWwindow instance */
	GLFWwindow*   instance = NULL;
	/* The InputManger to handle the input from this window instance */
	InputManager* inputManager = NULL;
	/* The settings used when creating this window instance */
	Settings      settings;

	/* Various internal callbacks */
	static void windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void windowCharCallback(GLFWwindow* window, unsigned int codepoint);
	static void windowCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void windowCursorEnterCallback(GLFWwindow* window, int entered);
	static void windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	/* Method used to get the instance of the Window class that corresponds to a GLFWwindow instance */
	static Window* getWindowInstance(GLFWwindow* window);

	/* The current instance being used */
	static Window* currentInstance;
public:

	/* Constructor and destructor */
	Window();
	virtual ~Window() { destroy(); }

	/* Attempts to create the window and returns a boolean value which is true if it succeeded */
	bool create();
	/* Updates the window */
	void update();
	/* Destroys this window instance */
	void destroy();

	/* Assigns various values */
	void setResizable(bool resizable);
	void setDecorated(bool decorated);
	void setRefreshRate(int refreshRate);
	void setFloating(bool floating);
	void setSamples(int samples);
	void setVSync(bool vSync);
	void setPosition(int x, int y);

	/* The method used to enable/disable/toggle the cursor */
	void enableCursor();
	void disableCursor();
	void toggleCursor();

	/* Moves the window into the centre of the primary monitor */
	void centre();
	/* Makes this window instance the current one to receive input */
	void makeCurrent();
	/* Returns whether the user has tried to close the window */
	bool shouldClose();

	/* State driven input methods */
	bool isKeyPressed(int key);
	bool isMouseDown(int button);

	/* Getters */
	inline GLFWwindow*    getInstance() { return instance; }
	inline Settings&      getSettings() { return settings; }
	inline InputManager*  getInputManager() { return inputManager; }

	static inline Window* getCurrentInstance() { return currentInstance; }
};


#endif /* CORE_WINDOW_H_ */
