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

#include "KeyboardShortcuts.h"

#include <algorithm>

#include "../Window.h"

/*****************************************************************************
 * The KeyboardShortcut class
 *****************************************************************************/

KeyboardShortcut::KeyboardShortcut(std::string name, std::vector<int> keys) {
	//Assign the name
	this->name = name;
	//Assign the completed value
	this->completed = false;
	//Go though each key provided and add them to the keys list
	for (unsigned int a = 0; a < keys.size(); a++)
		this->keys.insert(std::pair<int, bool>(keys[a], false));
}

KeyboardShortcut::~KeyboardShortcut() {

}

void KeyboardShortcut::onKeyPressed(int code) {
	//Check whether one of the keys has the same key code
	if (keys.count(code) > 0)
		//Update it's state
		keys.at(code) = true;
	//Only make completed = true when all of the keys states are also true
	completed = true;
	for (auto& iterator : keys)
		completed = completed && iterator.second;
}

void KeyboardShortcut::onKeyReleased(int code) {
	//Check whether one of the keys has the same key code
	if (keys.count(code) > 0) {
		//Update it's state
		keys.at(code) = false;
		//Completed is now definitely false
		completed = false;
	}
}

/*****************************************************************************
 * The KeyboardShortcuts class
 *****************************************************************************/

KeyboardShortcuts::KeyboardShortcuts() {
	Window::getCurrentInstance()->getInputManager()->addListener(this);
}

KeyboardShortcuts::~KeyboardShortcuts() {

}

void KeyboardShortcuts::addListener(KeyboardShortcutListener* listener) {
	listeners.push_back(listener);
}

void KeyboardShortcuts::removeListener(KeyboardShortcutListener* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void KeyboardShortcuts::callOnShortcut(KeyboardShortcut* e) {
	for (unsigned int a = 0; a < listeners.size(); a++)
		listeners[a]->onShortcut(e);
}

void KeyboardShortcuts::onKeyPressed(int code) {
	for (unsigned int a = 0; a < shortcuts.size(); a++) {
		shortcuts[a]->onKeyPressed(code);

		//Check whether the shortcut is now complete
		if (shortcuts[a]->hasCompleted())
			//Notify the listeners
			callOnShortcut(shortcuts[a]);
	}
}

void KeyboardShortcuts::onKeyReleased(int code) {
	for (unsigned int a = 0; a < shortcuts.size(); a++)
		shortcuts[a]->onKeyReleased(code);
}
