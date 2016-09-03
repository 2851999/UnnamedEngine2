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

#ifndef CORE_GUI_GUIDROPDOWNLIST_H_
#define CORE_GUI_GUIDROPDOWNLIST_H_

#include "GUIDropDownMenu.h"

/*****************************************************************************
 * The GUIDropDownList class
 *****************************************************************************/

class GUIDropDownList : public GUIDropDownMenu {
private:
	/* The overlay */
	GameObject2D* overlay = NULL;

	/* The overlay texture (when closed - or for both if the second one isn't
	 * assigned) */
	Texture* overlayClosedTexture = NULL;

	/* The other texture (when open - if assigned) */
	Texture* overlayOpenedTexture = NULL;
public:
	/* The constructors */
	GUIDropDownList(GUIButton* menuButton) : GUIDropDownMenu(menuButton) {}
	GUIDropDownList(GUIButton* menuButton, Texture* overlayClosedTexture, Texture* overlayOpenedTexture = NULL);

	/* The destructor */
	virtual ~GUIDropDownList() {}

	/* The method used to update this component */
	virtual void update() override;

	/* The method used to render this component */
	virtual void render(bool overrideShader = false) override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;

	/* Returns the selection (the text of the menu button) */
	inline std::string getSelection() { return menuButton->getText(); }
};

#endif /* CORE_GUI_GUIDROPDOWNLIST_H_ */
