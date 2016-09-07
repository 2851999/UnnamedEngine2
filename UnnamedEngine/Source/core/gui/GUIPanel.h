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

#ifndef CORE_GUI_GUIPANEL_H_
#define CORE_GUI_GUIPANEL_H_

#include "GUIComponent.h"

/*****************************************************************************
 * The GUIPanel class helps to manage a set of GUIComponent instances
 *****************************************************************************/

class GUIPanel : public GUIComponent {
private:
	/* The GUIComponents within this panel */
	std::vector<GUIComponent*> components;
public:
	/* The constructor */
	GUIPanel() {}
	GUIPanel(std::vector<GUIComponent*> components) : components(components) {}

	/* The destructor */
	virtual ~GUIPanel();

	/* Method used to add a component to this panel */
	inline void add(GUIComponent* component) { component->setParent(this); components.push_back(component); }

	/* Method used to remove a component from this panel */
	void remove(GUIComponent* component);

	/* Methods used to hide/show this panel */
	void show();
	void hide();
};

#endif /* CORE_GUI_GUIPANEL_H_ */
