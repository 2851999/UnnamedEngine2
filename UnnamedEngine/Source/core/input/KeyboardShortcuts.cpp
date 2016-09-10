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

/***************************************************************************************************
 * The KeyboardShortcut class
 ***************************************************************************************************/

KeyboardShortcut::KeyboardShortcut(std::string name, std::vector<int> keys) {
	this->name = name;
	this->keys = keys;
	for (unsigned int a = 0; a < keys.size(); a++)
		this->states.push_back(false);
	instance = NULL;
}

void KeyboardShortcut::check() {
	if (hasCompleted()) {
		if (instance != NULL) {
			instance->callOnShortcut(this);
		}
	}
}

bool KeyboardShortcut::hasCompleted() {
	bool completed = true;
	for (unsigned int a = 0; a < keys.size(); a++)
		completed = completed && states[a];
	return completed;
}

void KeyboardShortcut::onKeyPressed(int code) {
	for (unsigned int a = 0; a < keys.size(); a++) {
		if (code == keys[a]) {
			states[a] = true;
			check();
		}
	}
}

void KeyboardShortcut::onKeyReleased(int code) {
	for (unsigned int a = 0; a < keys.size(); a++) {
		if (code == keys[a])
			states[a] = false;
	}
}

/***************************************************************************************************/

/***************************************************************************************************
 * The KeyboardShortcuts class
 ***************************************************************************************************/

void KeyboardShortcuts::add(KeyboardShortcut* shortcut) {
	shortcut->instance = this;
	shortcuts.push_back(shortcut);
}

void KeyboardShortcuts::callOnShortcut(KeyboardShortcut* e) {
	for (unsigned int a = 0; a < listeners.size(); a++)
		listeners[a]->onShortcut(e);
}

void KeyboardShortcuts::onKeyPressed(int code) {
	for (unsigned int a = 0; a < shortcuts.size(); a++)
		shortcuts[a]->onKeyPressed(code);
}

void KeyboardShortcuts::onKeyReleased(int code) {
	for (unsigned int a = 0; a < shortcuts.size(); a++)
		shortcuts[a]->onKeyReleased(code);
}

/***************************************************************************************************/
