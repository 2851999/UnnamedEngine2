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

/*****************************************************************************
 * The GUIDropDownList class
 *****************************************************************************/

GUIDropDownList::GUIDropDownList(GUIButton* menuButton, Texture* overlayClosedTexture, Texture* overlayOpenedTexture) : GUIDropDownMenu(menuButton) {
	overlay = new GameObject2D({ new Mesh(MeshBuilder::createQuad(menuButton->getWidth(), menuButton->getHeight(), overlayClosedTexture)) }, Renderer::getRenderShader("Material"));
	overlay->setParent(this);
	overlay->setSize(menuButton->getSize());

	this->overlayClosedTexture = overlayClosedTexture;
	if (overlayOpenedTexture)
		this->overlayOpenedTexture = overlayOpenedTexture;
	else
		this->overlayOpenedTexture = overlayClosedTexture;

	overlay->getMaterial()->setDiffuseTexture(overlayClosedTexture);
}

void GUIDropDownList::onComponentClicked(GUIComponent* component) {
	//Check whether it is the menu button
	if (component == menuButton) {
		//Change the state of the menu
		menuOpen = ! menuOpen;
		//Setup the menu
		setupMenu();

		if (overlayClosedTexture) {
			if (menuOpen)
				overlay->getMaterial()->setDiffuseTexture(overlayOpenedTexture);
			else
				overlay->getMaterial()->setDiffuseTexture(overlayClosedTexture);
		}
	} else if (menuOpen && component != this) {
		//Assign the text of the menu button
		menuButton->setText(component->getText());
		//Change the state of the menu (to close it)
		menuOpen = false;
		//Setup the menu
		setupMenu();

		if (overlayClosedTexture)
			overlay->getMaterial()->setDiffuseTexture(overlayClosedTexture);
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

		if (! contains(data.lastX, data.lastY)) {
			menuOpen = false;
			if (overlayClosedTexture)
				overlay->getMaterial()->setDiffuseTexture(overlayClosedTexture);
			setupMenu();
		}
	}
}
