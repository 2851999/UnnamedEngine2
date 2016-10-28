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

#ifndef CORE_GUI_GUIGROUP_H_
#define CORE_GUI_GUIGROUP_H_

#include "GUIComponent.h"

/*****************************************************************************
 * The GUIGroup class helps organise a set of components
 *****************************************************************************/

class GUIGroup : public GUIComponent {
protected:
	/* The GUIComponents within this group */
	std::vector<GUIComponent*> components;
public:
	/* The constructor */
	GUIGroup(float width = 0, float height = 0) { setWidth(width); setHeight(height); }
	GUIGroup(std::vector<GUIComponent*>& components, float width = 0, float height = 0) : components(components) { setWidth(width), setHeight(height); }

	/* The destructor */
	virtual ~GUIGroup();

	/* Method used to add a component to this panel */
	virtual void add(GUIComponent* component);

	/* Method used to remove a component from this panel */
	virtual void remove(GUIComponent* component);

	/* Methods used to hide/show this group */
	virtual void show();
	virtual void hide();

	/* Method used to update this group */
	virtual void update() override;

	/* Method used to render this group */
	virtual void render() override;

	/* Getters */
	inline std::vector<GUIComponent*>& getComponents() { return components; }
};



#endif /* CORE_GUI_GUIGROUP_H_ */
