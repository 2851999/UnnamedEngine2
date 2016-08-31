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

#include "../utils/Logging.h"
#include "input/Input.h"
#include "Window.h"

/*****************************************************************************
 * The Window class
 *****************************************************************************/

using namespace Logger;

Window* Window::currentInstance;

Window::Window() {
	inputManager = new InputManager(this);
}

bool Window::create() {
	//Attempt to initialise GLFW
	if (! glfwInit()) {
		log("Failed to initialise GLFW", "Window", LogType::Error);
		return false;
	}

	//Setup the window
	glfwDefaultWindowHints();

	setResizable(settings.windowResizable);
	setDecorated(settings.windowDecorated);
	setSamples(settings.videoSamples);
	setVSync(settings.videoVSync);
	setRefreshRate(settings.videoRefreshRate);
	setFloating(settings.windowFloating);

	Vector2i targetRes = settings.videoResolution;
	GLFWmonitor* monitor = NULL;

	if (settings.windowFullscreen) {
		monitor = glfwGetPrimaryMonitor();

		if (settings.windowBorderless) {
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		}
	}

	instance = glfwCreateWindow(targetRes.getX(), targetRes.getY(), settings.windowTitle.c_str(), monitor, NULL);

	if (! instance) {
		log("Failed to create the window", "Window", LogType::Error);
		glfwTerminate();
		return false;
	}

	makeCurrent();

	int width, height;
	glfwGetFramebufferSize(instance, &width, &height);
	settings.windowWidth = width;
	settings.windowHeight = height;

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

void Window::setResizable(bool resizable)    { glfwWindowHint(GLFW_RESIZABLE, GLUtils::glValue(resizable)); }
void Window::setDecorated(bool decorated)    { glfwWindowHint(GLFW_DECORATED, GLUtils::glValue(decorated)); }
void Window::setRefreshRate(int refreshRate) { glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);              }
void Window::setFloating(bool floating)      { glfwWindowHint(GLFW_FLOATING, GLUtils::glValue(floating));   }
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

void Window::centre() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	setPosition(mode->width / 2 - settings.windowWidth / 2, mode->height / 2 - settings.windowHeight / 2);
}
void Window::makeCurrent() { glfwMakeContextCurrent(instance); currentInstance = this;   }
bool Window::shouldClose() { return GLUtils::boolValue(glfwWindowShouldClose(instance)); }

bool Window::isKeyPressed(int key)   {
	int state = glfwGetKey(instance, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool Window::isMouseDown(int button) {
	int state = glfwGetMouseButton(instance, button);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

/* Various callbacks */
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

