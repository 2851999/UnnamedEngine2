/*
 * GUIDropDownMenu.h
 *
 *  Created on: 3 Sep 2016
 *      Author: Joel
 */

#ifndef CORE_GUI_GUIDROPDOWNMENU_H_
#define CORE_GUI_GUIDROPDOWNMENU_H_

#include "GUIButton.h"

/*****************************************************************************
 * The GUIDropDownMenu class
 *****************************************************************************/

class GUIDropDownMenu : public GUIComponent, public GUIComponentListener {
protected:
	/* States whether the menu is open */
	bool menuOpen;

	/* The menu button */
	GUIButton* menuButton;

	/* The other buttons in the menu */
	std::vector<GUIButton*> buttons;

	/* Method called to setup the bounds of the menu, and the button's
	 * active and visible properties */
	void setupMenu();
public:
	/* The constructor */
	GUIDropDownMenu(GUIButton* menuButton);

	/* The destructor */
	virtual ~GUIDropDownMenu();

	/* The method used to add a button */
	void addButton(GUIButton* button);

	/* The method used to remove a button */
	inline void removeButton(GUIButton* button) { buttons.erase(std::remove(buttons.begin(), buttons.end(), button), buttons.end()); }

	/* The method used to update this component */
	virtual void update() override;

	/* The method used to render this component */
	virtual void render(bool overrideShader = false) override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;

	/* Called when the mouse is pressed */
	virtual void onMousePressed(int button) override;

	/* Setters and getters */
	bool isOpen() { return menuOpen; }
	GUIButton* getMenuButton() { return menuButton; }
	std::vector<GUIButton*>& getButtons() { return buttons; }
};

#endif /* CORE_GUI_GUIDROPDOWNMENU_H_ */
