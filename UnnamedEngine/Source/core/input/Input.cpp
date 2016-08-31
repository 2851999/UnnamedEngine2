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

#include "../Window.h"
#include "Input.h"

/*****************************************************************************
 * The InputManager class
 *****************************************************************************/

void InputManager::keyCallback(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && window->getSettings().inputRepeatKeyboardEvents))
		callOnKeyPressed(key);
	else if (action == GLFW_RELEASE)
		callOnKeyReleased(key);
}

void InputManager::charCallback(unsigned int codepoint) {
	callOnChar(codepoint, (char) codepoint);
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

	callOnMouseMoved(xpos, ypos, dx, dy);

	if (window->isMouseDown(GLFW_MOUSE_BUTTON_LEFT))
		callOnMouseDragged(xpos, ypos, dx, dy);
}

void InputManager::cursorEnterCallback(int entered) {
	if (entered)
		callOnMouseEnter();
	else
		callOnMouseLeave();
}

void InputManager::mouseButtonCallback(int button, int action, int mods) {
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && window->getSettings().inputRepeatMouseEvents))
		callOnMousePressed(button);
	else if (action == GLFW_RELEASE)
		callOnMouseReleased(button);
}

void InputManager::scrollCallback(double xoffset, double yoffset) {
	callOnScroll(xoffset, yoffset);
}

