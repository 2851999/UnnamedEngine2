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

#include "GUIDropDownList.h"

#include "../Window.h"

/*****************************************************************************
 * The GUIDropDownList class
 *****************************************************************************/

GUIDropDownList::GUIDropDownList(GUIButton* menuButton, Texture* overlayClosedTexture, Texture* overlayOpenedTexture) : GUIDropDownMenu(menuButton) {
	//Create the overlay for the drop down using the texture
	overlay = new GameObject2D({ new Mesh(MeshBuilder::createQuad(menuButton->getWidth(), menuButton->getHeight(), overlayClosedTexture)) }, "Material");
	overlay->setParent(this);
	overlay->setSize(menuButton->getSize());

	this->overlayClosedTexture = overlayClosedTexture;
	//Account for the case that the closed texture is not assigned
	if (overlayOpenedTexture)
		this->overlayOpenedTexture = overlayOpenedTexture;
	else
		this->overlayOpenedTexture = overlayClosedTexture;

	//Assign the texture to show initially
	overlay->getMaterial()->diffuseTexture = overlayClosedTexture;
}

void GUIDropDownList::onComponentClicked(GUIComponent* component) {
	//Check whether it is the menu button
	if (component == menuButton) {
		//Change the state of the menu
		menuOpen = ! menuOpen;
		//Setup the menu
		setupMenu();

		//Change the overlay texture if it can
		if (overlayClosedTexture) {
			if (menuOpen)
				overlay->getMaterial()->diffuseTexture = overlayOpenedTexture;
			else
				overlay->getMaterial()->diffuseTexture = overlayClosedTexture;
		}
	} else if (menuOpen && component != this) {
		//Assign the text of the menu button
		menuButton->setText(component->getText());
		//Change the state of the menu (to close it)
		menuOpen = false;
		//Setup the menu
		setupMenu();

		if (overlayClosedTexture)
			overlay->getMaterial()->diffuseTexture = overlayClosedTexture;
	}
}

void GUIDropDownList::onComponentUpdate() {
	GUIDropDownMenu::onComponentUpdate();

	if (overlay)
		overlay->update();
}

void GUIDropDownList::onComponentRender() {
	GUIDropDownMenu::onComponentRender();

	if (overlay)
		overlay->render();
}

void GUIDropDownList::onMousePressed(int button) {
	GUIComponent::onMousePressed(button);

	if (active && menuOpen) {
		InputManager::CursorData& data = Window::getCurrentInstance()->getInputManager()->getCursorData();

		//Check whether the mouse was clicked outside of the menu button and close this menu if it is
		//(the menu should close if one of the buttons within this drop down menu is clicked)
		if (! contains(data.lastX, data.lastY)) {
			menuOpen = false;
			if (overlayClosedTexture)
				overlay->getMaterial()->diffuseTexture = overlayClosedTexture;
			setupMenu();
		}
	}
}
