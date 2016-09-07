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

#include "GUIComponent.h"

#include <GLFW/glfw3.h>

/*****************************************************************************
 * The GUIComponentRenderer class
 *****************************************************************************/

Font* GUIComponentRenderer::DEFAULT_FONT = NULL;

void GUIComponentRenderer::setup() {
	//Set the default colour and texture
	getMaterial()->setDiffuseColour(Colour::WHITE);
	getMaterial()->setDiffuseTexture(Renderer::getBlankTexture());
}

void GUIComponentRenderer::update() {
	//Update the base class
	GameObject2D::update();

	//Check that there are colours and the render index is within its bounds
	if (hasColours() && colours.size() > renderIndex)
		//Assign the colour
		getMaterial()->setDiffuseColour(colours.at(renderIndex));
	//Now to do the same for the textures
	if (hasTextures() && textures.size() > renderIndex)
		getMaterial()->setDiffuseTexture(textures.at(renderIndex));
}

void GUIComponentRenderer::renderText(std::string text, Vector2f relPos) {
	//Make sure there is a font instance
	if (font)
		//Render the text
		font->render(text, getPosition() + relPos);
}

void GUIComponentRenderer::renderTextAtCentre(std::string text) {
	//Make sure there is a font instance
	if (font)
		//Render the text
		renderText(text, Vector2f(getWidth() / 2 - font->getWidth(text) / 2, getHeight() / 2 + font->getHeight(text) / 2));
}

unsigned int GUIComponentRenderer::getMaxRenderIndex() {
	return MathsUtils::max(colours.size(), textures.size());
}

/*****************************************************************************
 * The GUIFill class
 *****************************************************************************/

void GUIFill::updateWidth(float width) {
	getRelScale().setX(width / (getWidth() / getScale().getX()));
}

void GUIFill::updateHeight(float height) {
	getRelScale().setY(height / (getHeight() / getScale().getY()));
}

/*****************************************************************************
 * The GUIBorder class
 *****************************************************************************/

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Colour colour) :
		GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colour),
		component(component), thickness(thickness) {

	setPosition(-thickness, -thickness);
	setParent(component);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Texture* texture) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), texture),
				component(component), thickness(thickness) {

		setPosition(-thickness, -thickness);
		setParent(component);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Colour colour, Texture* texture) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colour, texture),
				component(component), thickness(thickness) {

		setPosition(-thickness, -thickness);
		setParent(component);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colours),
				component(component), thickness(thickness) {

		setPosition(-thickness, -thickness);
		setParent(component);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Texture*> textures) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), textures),
				component(component), thickness(thickness) {

		setPosition(-thickness, -thickness);
		setParent(component);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours, std::vector<Texture*> textures) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colours, textures),
				component(component), thickness(thickness) {

		setPosition(-thickness, -thickness);
		setParent(component);
}

/*****************************************************************************
 * The GUIComponent class
 *****************************************************************************/

void GUIComponent::enable() {
	Window::getCurrentInstance()->getInputManager()->addListener(this);
}

void GUIComponent::disable() {
	Window::getCurrentInstance()->getInputManager()->removeListener(this);
}

void GUIComponent::update() {
	if (active) {
		GUIComponentRenderer::update();

		if (border)
			border->update();
	}
}

void GUIComponent::render(bool overrideShader) {
	if (visible) {
		if (border)
			border->render();

		GUIComponentRenderer::render();
	}
}

void GUIComponent::onMousePressed(int button) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (mouseClicked != (active && visible && mouseHover)) {
			mouseClicked = ! mouseClicked;
			onChangeState();
		}
	}
}

void GUIComponent::onMouseReleased(int button) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (active && visible && mouseClicked) {
			mouseClicked = ! mouseClicked;
			onChangeState();
			callOnComponentClicked(this);
		}
	}
}

void GUIComponent::onMouseMoved(double x, double y, double dx, double dy) {
	if (mouseHover != (active && visible && getBounds().contains(x, y))) {
		mouseHover = ! mouseHover;
		onChangeState();
	}
}

void GUIComponent::onMouseEnter() {
	if (active && visible) {
		if (mouseHover != (getBounds().contains(Window::getCurrentInstance()->getInputManager()->getCursorData().lastX, Window::getCurrentInstance()->getInputManager()->getCursorData().lastY))) {
			mouseHover = ! mouseHover;
			onChangeState();
		}
	}
}

void GUIComponent::onMouseLeave() {
	if (active && visible && (mouseClicked || mouseHover)) {
		mouseClicked = false;
		mouseHover = false;
		onChangeState();
	}
}
