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

	/* The method called to render this component */
	virtual void onComponentUpdate() override;

	/* The method called to render this component */
	virtual void onComponentRender() override;
public:
	/* The constructor */
	GUIDropDownMenu(GUIButton* menuButton);

	/* The destructor */
	virtual ~GUIDropDownMenu();

	/* The method used to add a button */
	void addButton(GUIButton* button);

	/* The method used to remove a button */
	void removeButton(GUIButton* button);

	/* Methods used to enable/disable this component (Adds/Removes the input listener instance */
	virtual void enable() override;
	virtual void disable() override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;

	/* Called when the mouse is pressed */
	virtual void onMousePressed(int button) override;

	/* Setters and getters */
	inline bool isOpen() { return menuOpen; }
	inline GUIButton* getMenuButton() { return menuButton; }
	inline std::vector<GUIButton*>& getButtons() { return buttons; }
};

#endif /* CORE_GUI_GUIDROPDOWNMENU_H_ */
