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

#include "DebugConsole.h"

#include "../core/BaseEngine.h"
#include "GLUtils.h"

/*****************************************************************************
 * The DebugConsole class
 *****************************************************************************/

DebugConsole::DebugConsole(BaseEngine* baseEngine) : baseEngine(baseEngine) {
	//Create the text box
	textBox = new GUITextBox(Colour(0.1f, 0.1f, 0.1f, 0.6f), baseEngine->getSettings().windowWidth, 20.0f);
	textBox->setPosition(0.0f, baseEngine->getSettings().windowHeight - textBox->getHeight());
	textBox->selection->setColour(Colour(0.1f, 0.1f, 0.1f, 0.4f));

	//Add the components
	add(textBox);
}

void DebugConsole::process(std::string command) {
	//Split up the command using a space
	std::vector<std::string> split = StrUtils::strSplit(command, ' ');

	//Ensure a command was entered
	if (split.size() > 0) {
		//Check for any known commands
		if (split[0] == "exit")
			//Request the engine to stop
			baseEngine->requestClose();
		else if (split[0] == "wireframe") {
			if (split[1] == "enable")
				GLUtils::enableWireframe();
			else if (split[1] == "disable")
				GLUtils::disableWireframe();
		}

		//Go through the functions and attempt to process the data
		for (unsigned int i = 0; i < functions.size(); i++)
			functions[i](split);
	}
}

void DebugConsole::onKeyPressed(int key) {
	if (key == GLFW_KEY_GRAVE_ACCENT)
		textBox->disable();
}

void DebugConsole::onKeyReleased(int key) {
	//Check what key was pressed
	if (key == GLFW_KEY_GRAVE_ACCENT) {
		//Toggle this panel's visibility
		if (visible)
			hide();
		else {
			show();
			//Make sure the text box is selected and clear
			textBox->clear();
			textBox->setSelected(true);
		}
	} else if (key == GLFW_KEY_ENTER) {
		//Process the command and clear the text box
		process(textBox->getText());
		textBox->clear();
	}
}
