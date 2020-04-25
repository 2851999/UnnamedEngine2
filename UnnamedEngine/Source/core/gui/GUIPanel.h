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

#pragma once

#include "GUIButton.h"
#include "GUIGroup.h"

/*****************************************************************************
 * The GUIPanel class helps to manage a set of GUIComponent instances
 *****************************************************************************/

class GUIPanel : public GUIGroup, public GUIComponentListener {
private:
	/* The graphics pipelines used to render this panel */
	GraphicsPipeline* pipelineGUI;
	GraphicsPipeline* pipelineFont;
	GraphicsPipeline* pipelineFontSDF;

	/* Method used to get the top most component that contains a certain
	 * point */
	GUIComponent* getTop(double x, double y);
public:
	/* The constructor */
	GUIPanel(float width = 0, float height = 0);
	GUIPanel(std::vector<GUIComponent*>& components, float width = 0, float height = 0);

	/* The destructor */
	virtual ~GUIPanel();

	/* Method used to add a component to this panel */
	virtual void add(GUIComponent* component) override;

	/* Method used to render this panel */
	virtual void render() override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override {}

	/* Called when the mouse moves */
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;
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
	std::unordered_map<std::string, GUIPanel*> panels;

	/* The buttons used for switching between groups */
	std::unordered_map<GUIComponent*, std::string> buttons;
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

