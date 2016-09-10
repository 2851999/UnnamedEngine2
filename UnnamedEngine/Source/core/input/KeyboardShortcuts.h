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

#ifndef CORE_INPUT_KEYBOARDSHORTCUTS_H_
#define CORE_INPUT_KEYBOARDSHORTCUTS_H_

#include <string>
#include <vector>

#include "Input.h"
#include "../Window.h"

/***************************************************************************************************
 * The KeyboardShortcut class
 ***************************************************************************************************/

class KeyboardShortcuts;

class KeyboardShortcut {
public:
	std::string name;
	std::vector<int> keys;
	std::vector<bool> states;

	KeyboardShortcuts* instance;

	KeyboardShortcut(std::string name, std::vector<int> keys);

	void check();

	bool hasCompleted();

	void onKeyPressed(int code);
	void onKeyReleased(int code);
};

/***************************************************************************************************/

/***************************************************************************************************
 * The KeyboardShortcutListener class
 ***************************************************************************************************/

class KeyboardShortcutListener {
public:
	KeyboardShortcutListener() {}
	virtual ~KeyboardShortcutListener() {}
	virtual void onShortcut(KeyboardShortcut* e) {}
};

/***************************************************************************************************/

/***************************************************************************************************
 * The KeyboardShortcuts class
 ***************************************************************************************************/

class KeyboardShortcuts : public InputListener {
public:
	std::vector<KeyboardShortcutListener*> listeners;
	std::vector<KeyboardShortcut*> shortcuts;

	KeyboardShortcuts() {
		Window::getCurrentInstance()->getInputManager()->addListener(this);
	}

	void add(KeyboardShortcut* instance);
	inline void addListener(KeyboardShortcutListener* listener) { listeners.push_back(listener); }
	void callOnShortcut(KeyboardShortcut* e);
	void onKeyPressed(int code) override;
	void onKeyReleased(int code) override;
};

/***************************************************************************************************/

#endif /* CORE_INPUT_KEYBOARDSHORTCUTS_H_ */
