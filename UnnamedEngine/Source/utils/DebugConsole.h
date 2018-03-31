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

#ifndef UTILS_DEBUGCONSOLE_H_
#define UTILS_DEBUGCONSOLE_H_

#include <functional>

#include "../core/gui/GUIPanel.h"
#include "../core/gui/GUITextBox.h"

/*****************************************************************************
 * The DebugConsole class used to create a console that can be used to issue
 * commands to the engine
 *****************************************************************************/

class BaseEngine;

class DebugConsole : public GUIPanel {
private:
	/* The BaseEngine instance */
	BaseEngine* baseEngine;

	/* The text box used to type commands into */
	GUITextBox* textBox;

	/* List of functions to be called to process a command */
	std::vector<std::function<void(std::vector<std::string>)>> functions;

	/* States whether the wireframe mode is on */
	bool wireframeMode = false;
public:
	/* The constructor */
	DebugConsole(BaseEngine* baseEngine);

	/* The destructor */
	virtual ~DebugConsole() {}

	/* Method used to process a command */
	void process(std::string command);

	/* Method used to add a method for processing the commands */
	inline void addProcessor(std::function<void(std::vector<std::string>)> function) { functions.push_back(function); }

	/* Input methods */
	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;

	/* Getters */
	inline bool isWireframeEnabled() { return wireframeMode; }
};

#endif /* UTILS_DEBUGCONSOLE_H_ */
