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

#include "GUIButton.h"

/*****************************************************************************
 * The GUIPanel class helps to manage a set of GUIComponent instances
 *****************************************************************************/

class GUIPanel : public GUIComponent, public GUIComponentListener {
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
	inline void add(GUIComponent* component) { component->setParent(this); component->addListener(this); components.push_back(component); }

	/* Method used to remove a component from this panel */
	void remove(GUIComponent* component);

	/* Methods used to hide/show this panel */
	virtual void show();
	virtual void hide();

	/* Method used to update this panel */
	virtual void update() override;

	/* Method used to render this panel */
	virtual void render(bool overrideShader = false) override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override {}
};

/*****************************************************************************
 * The GUIPanelGroup class helps to manage a set of GUIPanels and allows
 * GUIButton instances to be assigned to switch between panels
 *****************************************************************************/

class GUIPanelGroup : public GUIComponentListener {
private:
	/* The current active panel */
	GUIPanel* current = NULL;

	/* The GUIPanel instances in this group */
	std::map<std::string, GUIPanel*> panels;

	/* The buttons used for switching between groups */
	std::map<GUIComponent*, std::string> buttons;
public:
	/* The constructor */
	GUIPanelGroup() {}

	/* The destructor */
	virtual ~GUIPanelGroup();

	/* Method used to add a panel */
	inline void add(std::string id, GUIPanel* panel) { panels.insert(std::pair<std::string, GUIPanel*>(id, panel)); }

	/* Method used to add a button and assign it to a certain panel */
	void assignButton(GUIButton* button, std::string panelId);

	/* The method used to show a panel given its id */
	void show(std::string id);

	/* The method used to hide the current panel */
	void hideCurrent();

	/* Method used to update the current panel */
	void update();

	/* Method used to render the current panel */
	void render();

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;
};

#endif /* CORE_GUI_GUIPANEL_H_ */
