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

#include "Window.h"
#include "input/Input.h"
#include "../utils/Logging.h"
#include "../utils/GLUtils.h"

/*****************************************************************************
 * The Window class
 *****************************************************************************/

Window* Window::currentInstance;

Window::Window() {
	inputManager = new InputManager(this);
}

bool Window::create() {
	//Attempt to initialise GLFW
	if (! glfwInit()) {
		Logger::log("Failed to initialise GLFW", "Window", LogType::Error);
		return false;
	}

	//Setup the window
	glfwDefaultWindowHints();

	//Assign the values needed before the creation of the window
	setResizable(settings.windowResizable);
	setDecorated(settings.windowDecorated);
	setSamples(settings.videoSamples);
	setRefreshRate(settings.videoRefreshRate);
	setFloating(settings.windowFloating);

	Vector2i targetRes = settings.videoResolution;
	GLFWmonitor* monitor = NULL;

	//If the window should be fullscreen, the monitor instance needs to be assigned
	if (settings.windowFullscreen) {
		monitor = glfwGetPrimaryMonitor();

		//Check for borderless fullscreen and apply it as required
		if (settings.windowBorderless) {
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		}
	}

	instance = glfwCreateWindow(targetRes.getX(), targetRes.getY(), settings.windowTitle.c_str(), monitor, NULL);

	//Stop everything if the window creation was unsuccessful
	if (! instance) {
		Logger::log("Failed to create the window", "Window", LogType::Error);
		glfwTerminate();
		return false;
	}

	makeCurrent();

	//Set the VSync setting (Has to be assigned after the creation of the windwo
	setVSync(settings.videoVSync);

	//Get the actual size of the renderable part of the window (in case the desired size was not
	//supported by the monitor)
	int width, height;
	glfwGetFramebufferSize(instance, &width, &height);
	settings.windowWidth = width;
	settings.windowHeight = height;
	settings.windowAspectRatio = (float) width / (float) height;

	if (! settings.windowFullscreen)
		centre();

	//Setup the input
	glfwSetKeyCallback(instance, windowKeyCallback);
	glfwSetCharCallback(instance, windowCharCallback);
	glfwSetCursorPosCallback(instance, windowCursorPosCallback);
	glfwSetCursorEnterCallback(instance, windowCursorEnterCallback);
	glfwSetMouseButtonCallback(instance, windowMouseButtonCallback);
	glfwSetScrollCallback(instance, windowScrollCallback);

	return true;
}

void Window::update() {
	glfwSwapBuffers(instance);
	glfwPollEvents();

	//Update the controllers
	inputManager->updateControllers();
}

void Window::destroy() {
	delete inputManager;

	glfwDestroyWindow(instance);
	glfwTerminate();
}

void Window::setResizable(bool resizable)    { glfwWindowHint(GLFW_RESIZABLE, resizable); }
void Window::setDecorated(bool decorated)    { glfwWindowHint(GLFW_DECORATED, decorated); }
void Window::setRefreshRate(int refreshRate) { glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);              }
void Window::setFloating(bool floating)      { glfwWindowHint(GLFW_FLOATING, floating);   }
void Window::setSamples(int samples)         { glfwWindowHint(GLFW_SAMPLES, samples);                       }
void Window::setVSync(bool vSync)            { glfwSwapInterval(vSync);                                     }
void Window::setPosition(int x, int y)       { glfwSetWindowPos(instance, x, y);                            }

void Window::enableCursor()  { glfwSetInputMode(instance, GLFW_CURSOR, GLFW_CURSOR_NORMAL);   }
void Window::disableCursor() { glfwSetInputMode(instance, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void Window::toggleCursor() {
	if (glfwGetInputMode(instance, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		enableCursor();
	else
		disableCursor();
}

void Window::setCursorPosition(double x, double y) {
	glfwSetCursorPos(instance, x, y);
}
void Window::centre() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	setPosition(mode->width / 2 - settings.windowWidth / 2, mode->height / 2 - settings.windowHeight / 2);
}
void Window::makeCurrent() { glfwMakeContextCurrent(instance); currentInstance = this;   }
bool Window::shouldClose() { return glfwWindowShouldClose(instance); }

bool Window::isKeyPressed(int key)   {
	int state = glfwGetKey(instance, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Window::isMouseDown(int button) {
	int state = glfwGetMouseButton(instance, button);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

Vector2d Window::getCursorPosition() {
	//The x and y cursor positions
	double x, y;
	//Poll the current cursor position
	glfwGetCursorPos(instance, &x, &y);
	//Return the position
	return Vector2d(x, y);
}

Window* Window::getCurrentInstance() {
	if (currentInstance == NULL)
		Logger::log("No window is current", "Window", LogType::Warning);
	return currentInstance;
}

/* Various callbacks from GLFW */
void Window::windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	currentInstance->getInputManager()->keyCallback(key, scancode, action, mods);
}

void Window::windowCharCallback(GLFWwindow* window, unsigned int codepoint) {
	currentInstance->getInputManager()->charCallback(codepoint);
}

void Window::windowCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	currentInstance->getInputManager()->cursorPosCallback(xpos, ypos);
}

void Window::windowCursorEnterCallback(GLFWwindow* window, int entered) {
	currentInstance->getInputManager()->cursorEnterCallback(entered);
}

void Window::windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	currentInstance->getInputManager()->mouseButtonCallback(button, action, mods);
}

void Window::windowScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	currentInstance->getInputManager()->scrollCallback(xoffset, yoffset);
}

