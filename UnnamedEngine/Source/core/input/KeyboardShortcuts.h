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
#include <unordered_map>
#include <map>

#include "Input.h"

/*****************************************************************************
 * The KeyboardShortcut class
 *****************************************************************************/

class KeyboardShortcuts;

class KeyboardShortcut {
private:
	/* The name given to this keyboard shortcut */
	std::string name;

	/* States whether this shortcut has been completed */
	bool completed;
public:
	/* The keys that make up this shortcut - made up of their key code and
	 * whether they are currently pressed */
	std::unordered_map<int, bool> keys;

	/* The constructor */
	KeyboardShortcut(std::string name, std::vector<int> keys);

	/* The destrutor */
	virtual ~KeyboardShortcut();

	/* Returns whether this shorcut's keys are all pressed */
	inline bool hasCompleted() { return completed; }

	/* Should be called when a key is pressed */
	void onKeyPressed(int code);

	/* Should be called when a key is released */
	void onKeyReleased(int code);

	/* Setters and getters */
	inline const std::string& getName() { return name; }
};

/*****************************************************************************
 * The KeyboardShortcutListener class
 *****************************************************************************/

class KeyboardShortcutListener {
public:
	/* The constructor */
	KeyboardShortcutListener() {}

	/* The destructor */
	virtual ~KeyboardShortcutListener() {}

	/* Method called when a shortcut is completed */
	virtual void onShortcut(KeyboardShortcut* e) {}
};

/*****************************************************************************
 * The KeyboardShortcuts class
 *****************************************************************************/

class KeyboardShortcuts : public InputListener {
private:
	/* The listeners */
	std::vector<KeyboardShortcutListener*> listeners;

	/* The shortcuts */
	std::vector<KeyboardShortcut*> shortcuts;
public:
	/* The constructor */
	KeyboardShortcuts();

	/* The destructor */
	virtual ~KeyboardShortcuts();

	/* Method to enable the input listener by adding it to the current window
	 * instance */
	void enable();

	/* Method to disable the input listener by adding it to the current window
	 * instance */
	void disable();

	/* Method used to add a keyboard shortcut */
	inline void add(KeyboardShortcut* shortcut) { shortcuts.push_back(shortcut); }

	/* Method used to add a listener */
	void addListener(KeyboardShortcutListener* listener);

	/* Method used to remove a listener */
	void removeListener(KeyboardShortcutListener* listener);

	/* Method used to call the onShorcut() method in all of the listeners */
	void callOnShortcut(KeyboardShortcut* e);

	/* Overridden methods from InputListener */
	void onKeyPressed(int code) override;
	void onKeyReleased(int code) override;
};

#endif /* CORE_INPUT_KEYBOARDSHORTCUTS_H_ */
