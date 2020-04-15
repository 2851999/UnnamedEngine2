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
protected:
	/* The method called to render this component */
	virtual void onComponentUpdate() override;

	/* The method called to render this component */
	virtual void onComponentRender() override;
public:
	/* The constructors */
	GUIDropDownList(GUIButton* menuButton) : GUIDropDownMenu(menuButton) {}
	GUIDropDownList(GUIButton* menuButton, Texture* overlayClosedTexture, Texture* overlayOpenedTexture = NULL);

	/* The destructor */
	virtual ~GUIDropDownList() {}

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;

	/* Called when the mouse is pressed */
	virtual void onMousePressed(int button) override;

	/* Assigns the selection (the text of the menu button) */
	inline void setSelection(std::string text) { menuButton->setText(text); }

	/* Returns the selection (the text of the menu button) */
	inline std::string getSelection() { return menuButton->getText(); }
};

