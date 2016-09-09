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

#include "GUISlider.h"

/*****************************************************************************
 * The GUISlider class
 *****************************************************************************/

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour) :
	GUIComponent(width, height, std::vector<Colour> { colour }), button(button), direction(direction) {
	value = 0;
	dragging = false;
	button->setParent(this);
}

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Texture* texture) :
	GUIComponent(width, height, std::vector<Texture*> { texture }), button(button), direction(direction) {
	value = 0;
	dragging = false;
	button->setParent(this);
}

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour, Texture* texture) :
	GUIComponent(width, height, std::vector<Colour> { colour }, std::vector<Texture*> { texture }), button(button), direction(direction) {
	value = 0;
	dragging = false;
	button->setParent(this);
}

GUISlider::~GUISlider() {
	//Destroy the created resources
	delete button;
}

void GUISlider::onComponentUpdate() {
	//Update the slider button
	button->update();
}

void GUISlider::onComponentRender() {
	float width = getWidth();
	float height = getHeight();
	float buttonWidth = button->getWidth();
	float buttonHeight = button->getHeight();

	//Check the slider direction
	if (direction == VERTICAL) {
		//Clamp the boundaries
		if (button->getRelPosition().getY() < 0)
			button->getRelPosition().setY(0);
		else if (button->getRelPosition().getY() > height - buttonHeight)
			button->getRelPosition().setY(height - buttonHeight);
		//Make sure the button is in the middle
		button->getRelPosition().setX(-buttonWidth / 2 + width / 2);
	} else if (direction == HORIZONTAL) {
		//Clamp the boundaries
		if (button->getRelPosition().getX() < 0)
			button->getRelPosition().setX(0);
		else if (button->getRelPosition().getX() > width - buttonWidth)
			button->getRelPosition().setX(width - buttonWidth);
		//Make sure the button is in the middle
		button->getRelPosition().setY(-buttonHeight / 2 + height / 2);
	}

	//Render the button
	button->render();
}

void GUISlider::enable() {
	GUIComponent::enable();
	//Enable the slider button
	button->enable();
}

void GUISlider::disable() {
	GUIComponent::disable();
	//Disable the slider button
	button->disable();
}

void GUISlider::onMouseDragged(double x, double y, double dx, double dy) {
	GUIComponent::onMouseDragged(x, y, dx, dy);
	//Make sure this is visible and active
	if (visible && active) {
		float width = getWidth();
		float height = getHeight();
		//Get this sider's position
		Vector2f p = getPosition();
		//Check the direction of this slider
		if (direction == VERTICAL) {
			if (button->isMouseHovering() || dragging) {
				dragging = true;
				if (y > p.getY() && y < p.getY() + height) {
					button->getRelPosition().setY(button->getRelPosition().getY() + dy);
					//Set the slider value
					value = (button->getRelPosition().getY() / (height - button->getHeight())) * 100;
				}
			}
		} else if (direction == HORIZONTAL) {
			if (button->isMouseHovering() || dragging) {
				dragging = true;
				if (x > p.getX() && x < p.getX() + width) {
					button->getRelPosition().setX(button->getRelPosition().getX() + dx);
					//Set the slider value
					value = (button->getRelPosition().getX() / (height - button->getWidth())) * 100;
				}
			}
		}
		//Clamp the slider value
		if (value < 0)
			value = 0;
		else if (value > 100)
			value = 100;
	}
}

void GUISlider::onMouseReleased(int button) {
	GUIComponent::onMouseReleased(button);

	if (visible && active) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && dragging)
			dragging = false;
	}
}
