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

#include "GUIDropDownMenu.h"

#include <algorithm>

#include "../Window.h"

/*****************************************************************************
 * The GUIDropDownMenu class
 *****************************************************************************/

void GUIDropDownMenu::setupMenu() {
	//Check if the menu is open
	if (menuOpen) {
		//The current width/height
		float width = menuButton->getWidth();
		float height = menuButton->getHeight();

		//Go through each button added
		for (unsigned int i = 0; i < buttons.size(); i++) {
			//Assign the width to the maximum width
			width = MathsUtils::max(width, buttons[i]->getWidth());
			//Add onto the height
			height += buttons[i]->getHeight();

			//Assign the buttons active and visible properies
			buttons[i]->setActive(active && menuOpen);
			buttons[i]->setVisible(visible && menuOpen);
		}

		//Assign the width/height
		setWidth(width);
		setHeight(height);
	} else {
		//It isn't so assign the width/height to the menu button's width/height
		setWidth(menuButton->getWidth());
		setHeight(menuButton->getHeight());

		//Go through each button added
		for (unsigned int i = 0; i < buttons.size(); i++) {
			//Assign the buttons active and visible properties
			buttons[i]->setActive(false);
			buttons[i]->setVisible(false);
		}
	}
}

GUIDropDownMenu::GUIDropDownMenu(GUIButton* menuButton) : menuButton(menuButton) {
	menuOpen = false;
	//Assign the parent of the menu button to this
	menuButton->setParent(this);
	//Assign the component size
	setupMenu();
	//Add this component listener to the button and this
	addListener(this);
	menuButton->addListener(this);
}

GUIDropDownMenu::~GUIDropDownMenu() {
	delete menuButton;
	for (unsigned int i = 0; i < buttons.size(); i++)
		delete buttons[i];
	buttons.clear();
}

void GUIDropDownMenu::addButton(GUIButton* button) {
	//Set the button's parent to this
	button->setParent(this);
	//Set the buttons active and visible properties
	button->setActive(false);
	button->setVisible(false);
	//Assign the buttons position
	if (buttons.size() == 0)
		button->setPosition(Vector2f(menuButton->getWidth() / 2 - button->getWidth() / 2, menuButton->getHeight()));
	else
		button->setPosition(Vector2f(menuButton->getWidth() / 2 - button->getWidth() / 2, buttons[buttons.size() - 1]->getRelPosition().getY() + buttons[buttons.size() - 1]->getHeight()));
	//Add this component listener to the button
	button->addListener(this);
	//Add the button
	buttons.push_back(button);
}

GUIButton* GUIDropDownMenu::createButton(std::string text) {
	//Create the new button based on the menu button
	GUIButton* button = NULL;
	if (menuButton->getTextures().size() > 0)
		button = new GUIButton(text, menuButton->getWidth(), menuButton->getHeight(), menuButton->getColours(), menuButton->getTextures());
	else
		button = new GUIButton(text, menuButton->getWidth(), menuButton->getHeight(), menuButton->getColours());
	button->setFont(menuButton->getFont());
	//Add and return the button
	addButton(button);
	return button;
}

void GUIDropDownMenu::removeButton(GUIButton* button) {
	buttons.erase(std::remove(buttons.begin(), buttons.end(), button), buttons.end());
}

void GUIDropDownMenu::enable() {
	GUIComponent::enable();
	//Enable the menu button
	menuButton->enable();
	//Enable all of the buttons
	for (unsigned int i = 0; i < buttons.size(); i++)
		buttons[i]->enable();
}

void GUIDropDownMenu::disable() {
	GUIComponent::disable();
	//Disable the menu button
	menuButton->disable();
	//Disable all of the buttons
	for (unsigned int i = 0; i < buttons.size(); i++)
		buttons[i]->disable();
}

void GUIDropDownMenu::onComponentUpdate() {
	//Update all of the buttons
	menuButton->update();

	if (menuOpen) {
		for (unsigned int i = 0; i < buttons.size(); i++)
			buttons[i]->update();
	}
}

void GUIDropDownMenu::onComponentRender() {
	//Render all of the buttons
	menuButton->render();

	if (menuOpen) {
		for (unsigned int i = 0; i < buttons.size(); i++)
			buttons[i]->render();
	}
}

void GUIDropDownMenu::onComponentClicked(GUIComponent* component) {
	//Check whether it is the menu button
	if (component == menuButton) {
		//Change the state of the menu
		menuOpen = ! menuOpen;
		//Setup the menu
		setupMenu();
	} else if (menuOpen && component != this) {
		//Change the state of the menu (to close it)
		menuOpen = false;
		//Setup the menu
		setupMenu();
	}
}

void GUIDropDownMenu::onMousePressed(int button) {
	GUIComponent::onMousePressed(button);

	if (active && menuOpen) {
		InputManager::CursorData& data = Window::getCurrentInstance()->getInputManager()->getCursorData();

		if (! contains(data.lastX, data.lastY)) {
			menuOpen = false;
			setupMenu();
		}
	}
}
