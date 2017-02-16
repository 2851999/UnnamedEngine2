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

#include <algorithm>

#include "../../utils/Logging.h"

/*****************************************************************************
 * The GUIPanel class
 *****************************************************************************/

GUIComponent* GUIPanel::getTop(double x, double y) {
	//The current top-most component
	GUIComponent* top = NULL;

	//Go through each component
	for (unsigned int i = 0; i < components.size(); i++) {
		//Check whether the current component contains the point
		if (components[i]->contains(x, y))
			//Assign the top-most component (As the components are
			//rendered in descending order)
			top = components[i];
	}

	//Return the component
	return top;
}

void GUIPanel::add(GUIComponent* component) {
	GUIGroup::add(component);
	component->addListener(this);
}

void GUIPanel::onMouseMoved(double x, double y, double dx, double dy) {
	//Ensure this panel is active
	if (active) {
		bool first = true;
		//Go through each component
		for (int i = components.size() - 1; i >= 0; i--) {
			//Check whether the current component contains the cursor
			if (components[i]->contains(x, y)) {
				//Check whether this is the first one found
				if (first) {
					//Make sure it isn't occluded
					components[i]->setOccluded(false);
					first = false;
				} else
					components[i]->setOccluded(true);
			} else
				components[i]->setOccluded(false);
		}
	}
}

/*****************************************************************************
 * The GUIPanelGroup class
 *****************************************************************************/

GUIPanelGroup::~GUIPanelGroup() {
	//Go though each panel
	for (auto& iterator : panels)
		delete iterator.second;
	panels.clear();
}

void GUIPanelGroup::assignButton(GUIButton* button, std::string panelId) {
	//Add this component listener to the button
	button->addListener(this);
	//Add the button
	buttons.insert(std::pair<GUIComponent*, std::string>(button, panelId));
}

void GUIPanelGroup::show(std::string id) {
	//Hide the current panel
	hideCurrent();
	//Ensure a panel with the id exists
	if (panels.count(id) > 0) {
		//Assign the current panel
		current = panels.at(id);
		//Show the current panel
		current->show();
	} else
		Logger::log("GUIPanel with the id " + id + " hasn't been added", "GUIPanelGroup", LogType::Debug);
}

void GUIPanelGroup::hideCurrent() {
	//Check whether there is a current panel
	if (current) {
		//Hide the current panel
		current->hide();
		current = NULL;
	}
}

void GUIPanelGroup::update() {
	//Check whether there is a current panel
	if (current)
		current->update();
}

void GUIPanelGroup::render() {
	if (current)
		current->render();
}

void GUIPanelGroup::onComponentClicked(GUIComponent* component) {
	//Assign the new panel
	show(buttons.at(component));
}
