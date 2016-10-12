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

#include <GLFW/glfw3.h>

/*****************************************************************************
 * The GUISlider class
 *****************************************************************************/

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour) :
	GUIComponent(width, height, std::vector<Colour> { colour }), button(button), direction(direction) {
	position = 0;
	value = 0;
	interval = 0;
	dragging = false;
	add(button);
}

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Texture* texture) :
	GUIComponent(width, height, std::vector<Texture*> { texture }), button(button), direction(direction) {
	position = 0;
	value = 0;
	interval = 0;
	dragging = false;
	add(button);
}

GUISlider::GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour, Texture* texture) :
	GUIComponent(width, height, std::vector<Colour> { colour }, std::vector<Texture*> { texture }), button(button), direction(direction) {
	position = 0;
	value = 0;
	interval = 0;
	dragging = false;
	add(button);
}

GUISlider::~GUISlider() {

}

void GUISlider::onComponentUpdate() {

}

void GUISlider::onComponentRender() {
	float width = getWidth();
	float height = getHeight();
	float buttonWidth = button->getWidth();
	float buttonHeight = button->getHeight();

	//Check the slider direction
	if (direction == VERTICAL) {
		//Assign the appropriate position in the correct direction
		if (interval == 0.0f)
			button->setPosition(0.0f, position);
		else
			button->setPosition(0.0f, (int) (position / interval) * interval);

		//Make sure the button is in the middle
		button->getRelPosition().setX(-buttonWidth / 2 + width / 2);
	} else if (direction == HORIZONTAL) {
		//Assign the appropriate position in the correct direction
		if (interval == 0.0f)
			button->setPosition(position, 0.0f);
		else
			button->setPosition((int) (position / interval) * interval, 0.0f);

		//Make sure the button is in the middle
		button->getRelPosition().setY(-buttonHeight / 2 + height / 2);
	}
}

void GUISlider::enable() {
	GUIComponent::enable();
}

void GUISlider::disable() {
	GUIComponent::disable();
}

void GUISlider::onMouseMoved(double x, double y, double dx, double dy) {
	GUIComponent::onMouseMoved(x, y, dx, dy);
}

void GUISlider::onMouseDragged(double x, double y, double dx, double dy) {
	GUIComponent::onMouseDragged(x, y, dx, dy);

	//Make sure this is visible and active
	if (visible && active) {
		float width = getWidth();
		float height = getHeight();
		//Check the direction of this slider
		if (direction == VERTICAL) {
			if (dragging) {
				position += dy;
				position = MathsUtils::clamp(position, 0.0f, height - button->getHeight());
				//Set the slider value
				if (interval == 0.0f)
					value = (button->getRelPosition().getY() / (height - button->getHeight())) * 100.0f;
				else
					value = (((int) ((int) position / (int) interval) * interval) / (int) (height - button->getHeight())) * 100.0f;
			}
		} else if (direction == HORIZONTAL) {
			if (dragging) {
				position += dx;
				position = MathsUtils::clamp(position, 0.0f, width - button->getWidth());
				//Set the slider value
				if (interval == 0.0f)
					value = (button->getRelPosition().getX() / (width - button->getWidth())) * 100.0f;
				else
					value = (((int) ((int) position / (int) interval) * interval) / (int) (width - button->getWidth())) * 100.0f;
			}
		}
		//Clamp the slider value
		if (value < 0)
			value = 0;
		else if (value > 100)
			value = 100;
	}
}

void GUISlider::onMousePressed(int button) {
	GUIComponent::onMousePressed(button);

	if (visible && active) {
		if (this->button->isClicked() && ! dragging)
			dragging = true;
	}
}

void GUISlider::onMouseReleased(int button) {
	GUIComponent::onMouseReleased(button);

	if (visible && active) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && dragging)
			dragging = false;
	}
}
