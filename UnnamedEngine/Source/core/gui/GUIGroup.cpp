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

#include "GUIGroup.h"

#include <algorithm>

/*****************************************************************************
 * The GUIGroup class helps organise a set of components
 *****************************************************************************/

GUIGroup::~GUIGroup() {
	//Go through each component and delete it
	for (unsigned int i = 0; i < components.size(); i++)
		delete components[i];
	components.clear();
}

void GUIGroup::add(GUIComponent* component) {
	//Add the component
	addChild(component);
	component->setGroup(this);
	components.push_back(component);
}


void GUIGroup::remove(GUIComponent* component) {
	//Ensure the component has been added
	if (std::find(components.begin(), components.end(), component) != components.end()) {
		//Remove the parent of the component
		removeChild(component);
		//Remove the component
		components.erase(std::remove(components.begin(), components.end(), component), components.end());
	}
}

void GUIGroup::show() {
	//Make this group visible
	setVisible(true);
	//Go through each component
	for (unsigned int i = 0; i < components.size(); i++) {
		//Enable the current component
		components[i]->enable();
		//Ensure the component is visible
		components[i]->setVisible(true);
		//This is done to ensure the component is updated in case the mouse is currently
		//hovering over them
		components[i]->onMouseEnter();
	}
}

void GUIGroup::hide() {
	//Make this group hidden
	setVisible(false);
	//Go through each component
	for (unsigned int i = 0; i < components.size(); i++) {
		//This is done to ensure the component is updated as if the mouse is leaving it
		//so that if it is shown again, it has an updated state
		components[i]->onMouseLeave();
		//Disable the current component
		components[i]->disable();
		//Ensure the component is not visible
		components[i]->setVisible(false);
	}
}

void GUIGroup::update() {
	//Ensure this group is active
	if (active) {
		//Go through and update all of the components in this group
		for (unsigned int i = 0; i < components.size(); i++)
			components[i]->update();
	}
}

void GUIGroup::render() {
	//Ensure this group is visible
	if (visible) {
		//Go through and render all of the components in this group
		for (unsigned int i = 0; i < components.size(); i++)
			components[i]->render();
	}
}
