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

#include "GUIPanel.h"

/*****************************************************************************
 * The GUIPanel class
 *****************************************************************************/

GUIPanel::~GUIPanel() {
	//Go through each component and delete it
	for (unsigned int i = 0; i < components.size(); i++)
		delete components[i];
	components.clear();
}

void GUIPanel::remove(GUIComponent* component) {
	//Ensure the component has been added
	if (std::find(components.begin(), components.end(), component) != components.end()) {
		//Remove the parent of the component
		component->setParent(NULL);
		//Remove the component
		components.erase(std::remove(components.begin(), components.end(), component), components.end());
	}
}

void GUIPanel::show() {
	//Make this panel visible
	setVisible(true);
	//Go through each component
	for (unsigned int i = 0; i < components.size(); i++) {
		//Enable the current component
		components[i]->enable();
		//Ensure the component is visible
		components[i]->setVisible(true);
	}
}

void GUIPanel::hide() {
	//Make this panel hidden
	setVisible(false);
	//Go through each component
	for (unsigned int i = 0; i < components.size(); i++) {
		//Enable the current component
		components[i]->disable();
		//Ensure the component is visible
		components[i]->setVisible(false);
	}
}

