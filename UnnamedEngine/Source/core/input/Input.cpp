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

#include <algorithm>

#include "../Window.h"
#include "Input.h"

/*****************************************************************************
 * The Mouse class
 *****************************************************************************/

bool Mouse::isPressed(int button) {
	return Window::getCurrentInstance()->isMouseDown(button);
}

Vector2d Mouse::getPosition() {
	return Window::getCurrentInstance()->getCursorPosition();
}

/*****************************************************************************
 * The Keyboard class
 *****************************************************************************/

bool Keyboard::isPressed(int key) {
	return Window::getCurrentInstance()->isKeyPressed(key);
}

/*****************************************************************************
 * The InputManager class
 *****************************************************************************/

void InputManager::addListener(InputListener* listener) {
	listeners.push_back(listener);
}

void InputManager::removeListener(InputListener* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void InputManager::updateControllers() {
	for (unsigned int i = 0; i < controllers.size(); i++)
		controllers[i]->checkInput();
}

/* Used to add a controller */
void InputManager::addController(Controller* controller) {
	controllers.push_back(controller);
}

void InputManager::keyCallback(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && window->getSettings().inputRepeatKeyboardEvents))
		InputManager::callOnKeyPressed(key);
	else if (action == GLFW_RELEASE)
		InputManager::callOnKeyReleased(key);
}

void InputManager::charCallback(unsigned int codepoint) {
	InputManager::callOnChar(codepoint, (char) codepoint);
}

void InputManager::cursorPosCallback(double xpos, double ypos) {
	double dx = xpos - cursorData.lastX;
	double dy = ypos - cursorData.lastY;
	if (cursorData.lastX == -1) {
		dx = 0;
		dy = 0;
	}
	cursorData.lastX = xpos;
	cursorData.lastY = ypos;

	InputManager::callOnMouseMoved(xpos, ypos, dx, dy);

	if (window->isMouseDown(GLFW_MOUSE_BUTTON_LEFT))
		InputManager::callOnMouseDragged(xpos, ypos, dx, dy);
}

void InputManager::cursorEnterCallback(int entered) {
	if (entered)
		InputManager::callOnMouseEnter();
	else
		InputManager::callOnMouseLeave();
}

void InputManager::mouseButtonCallback(int button, int action, int mods) {
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && window->getSettings().inputRepeatMouseEvents))
		InputManager::callOnMousePressed(button);
	else if (action == GLFW_RELEASE)
		InputManager::callOnMouseReleased(button);
}

void InputManager::scrollCallback(double xoffset, double yoffset) {
	InputManager::callOnScroll(xoffset, yoffset);
}

void InputManager::callOnKeyPressed(int key) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onKeyPressed(key);
}

void InputManager::callOnKeyReleased(int key) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onKeyReleased(key);
}

void InputManager::callOnChar(int key, char character) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onChar(key, character);
}

void InputManager::callOnMousePressed(int button) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onMousePressed(button);
}

void InputManager::callOnMouseReleased(int button) {
	for (unsigned int i = 0; i  < listeners.size(); i++)
		listeners.at(i)->onMouseReleased(button);
}

void InputManager::callOnMouseMoved(double x, double y, double dx, double dy) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onMouseMoved(x, y, dx, dy);
}

void InputManager::callOnMouseDragged(double x, double y, double dx, double dy) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onMouseDragged(x, y, dx, dy);
}

void InputManager::callOnMouseEnter() {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onMouseEnter();
}

void InputManager::callOnMouseLeave() {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onMouseLeave();
}

void InputManager::callOnScroll(double x, double y) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onScroll(x, y);
}

void InputManager::callOnControllerAxis(Controller* controller, int axis, float value) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onControllerAxis(controller, axis, value);
}

void InputManager::callOnControllerButtonPressed(Controller* controller, int button) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onControllerButtonPressed(controller, button);
}

void InputManager::callOnControllerButtonReleased(Controller* controller, int button) {
	for (unsigned int i = 0; i < listeners.size(); i++)
		listeners.at(i)->onControllerButtonReleased(controller, button);
}

