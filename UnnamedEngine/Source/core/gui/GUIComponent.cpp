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
#include <algorithm>

#include "../Window.h"

/*****************************************************************************
 * The GUIComponentRenderer class
 *****************************************************************************/

Font* GUIComponentRenderer::DEFAULT_FONT = NULL;

GUIComponentRenderer::~GUIComponentRenderer() {
	delete textInstance;
}

void GUIComponentRenderer::setup() {
	//Assign the font
	setFont(DEFAULT_FONT);
	//Set the default colour and texture
	getMaterial()->setDiffuse(Colour::WHITE);
	getMaterial()->setDiffuse(Renderer::getBlankTexture());
	getMaterial()->update();
}

void GUIComponentRenderer::update() {
	//Update the base class
	GameObject2D::update();

	//Check that there are colours and the render index is within its bounds
	if (renderIndex < colours.size())
		//Assign the colour
		getMaterial()->setDiffuse(colours[renderIndex]);

	//Now to do the same for the textures
	if (renderIndex < textures.size())
		getMaterial()->setDiffuse(textures[renderIndex]);

	//Avoid repeated unnecessary updates
	if (getMesh() && (lastRenderIndex < 0 || lastRenderIndex != renderIndex)) {
		getMaterial()->update();
		lastRenderIndex = renderIndex;
	}
}

void GUIComponentRenderer::render() {
	//Queue the rendering
	Renderer::getGraphicsPipelineQueue(Renderer::GRAPHICS_PIPELINE_GUI)->queueRender(this);
}

void GUIComponentRenderer::queuedRender() {
	GameObject::render();
}

void GUIComponentRenderer::renderText(std::string text, Vector2f relPos) {
	//Make sure there is a text instance
	if (textInstance)
		//Render the text
		textInstance->render(text, Vector2f(getPosition().getX(), getPosition().getY()) + relPos);
}

void GUIComponentRenderer::renderTextAtCentre(std::string text) {
	//Make sure there is a font instance
	if (font)
		//Render the text
		renderText(text, Vector2f(getWidth() / 2 - font->getWidth(text) / 2, getHeight() / 2 + font->getHeight(text) / 2));
}

unsigned int GUIComponentRenderer::getMaxRenderIndex() {
	return utils_maths::max(colours.size(), textures.size());
}

void GUIComponentRenderer::setColour(Colour colour) {
	if (colours.size() == 0)
		colours.push_back(colour);
	else {
		for (unsigned int i = 0; i < colours.size(); i++)
			colours[i] = colour;
	}
	//Ensure an update is performed
	lastRenderIndex = -1;
}

void GUIComponentRenderer::setTexture(Texture* texture) {
	if (textures.size() == 0)
		textures.push_back(texture);
	else {
		for (unsigned int i = 0; i < textures.size(); i++)
			textures[i] = texture;
	}
		//Ensure an update is performed
	lastRenderIndex = -1;
}

void GUIComponentRenderer::setFont(Font* font) {
	this->font = font;
	if (! textInstance)
		textInstance = new Text(DEFAULT_FONT, Colour::WHITE, Text::DEFAULT_MAX_CHARACTERS, false, true);
	textInstance->setFont(font);
}

/*****************************************************************************
 * The GUIFill class
 *****************************************************************************/

void GUIFill::updateWidth(float width) {
	setScale(width / getLocalWidth(), getLocalScale().getY());
}

void GUIFill::updateHeight(float height) {
	setScale(getLocalScale().getX(), height / getLocalHeight());
}

/*****************************************************************************
 * The GUIBorder class
 *****************************************************************************/

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Colour colour) :
		GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colour),
		component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Texture* texture) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), texture),
				component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, Colour colour, Texture* texture) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colour, texture),
				component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colours),
				component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Texture*> textures) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), textures),
				component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

GUIBorder::GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours, std::vector<Texture*> textures) :
				GUIFill(component->getWidth() + (thickness * 2), component->getHeight() + (thickness * 2), colours, textures),
				component(component), thickness(thickness) {

	setParent(component);
	setPosition(-thickness, -thickness);
}

/*****************************************************************************
 * The GUIComponent class
 *****************************************************************************/

GUIComponent::~GUIComponent() {
	if (border)
		delete border;
	listeners.clear();

	//Delete the attached components
	for (unsigned int i = 0; i < attachedComponents.size(); i++)
		delete attachedComponents[i];
	attachedComponents.clear();
}

void GUIComponent::addListener(GUIComponentListener* listener) {
	listeners.push_back(listener);
}

void GUIComponent::removeListener(GUIComponentListener* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

bool GUIComponent::contains(double x, double y) {
	Vector3f position = getPosition();
	Vector2f size = getSize();
	float px = position.getX();
	float py = position.getY();
	float width = size.getX();
	float height = size.getY();

	return (x >= px && x < px + width && y >= py && y < py + height);
}

void GUIComponent::enable() {
	Window::getCurrentInstance()->getInputManager()->addListener(this);
}

void GUIComponent::disable() {
	Window::getCurrentInstance()->getInputManager()->removeListener(this);
}

void GUIComponent::update() {
	GUIComponentRenderer::update();

	if (border)
		border->update();

	if (active) {
		//Update all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->update();

		//Update this component
		onComponentUpdate();
	}
}

void GUIComponent::render() {
	if (visible) {
		if (border)
			border->render();

		//Render this component
		GUIComponentRenderer::render();
		onComponentRender();

		//Update all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->render();
	}
}

void GUIComponent::onKeyPressed(int key) {
	if (active && visible) {
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onKeyPressed(key);
	}
}

void GUIComponent::onKeyReleased(int key) {
	if (active && visible) {
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onKeyReleased(key);
	}
}

void GUIComponent::onChar(int key, char character) {
	if (active && visible) {
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onChar(key, character);
	}
}

void GUIComponent::onMousePressed(int button) {
	if (active && visible) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (active && visible && ! occluded && mouseHover) {
				mouseClicked = true;
				onChangeState();
			}
		}

		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMousePressed(button);
	}
}

void GUIComponent::onMouseReleased(int button) {
	if (active && visible) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (active && visible && mouseClicked) {
				mouseClicked = false;
				onChangeState();
				if (! occluded)
					callOnComponentClicked(this);
			}
		}
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMouseReleased(button);
	}
}

void GUIComponent::onMouseMoved(double x, double y, double dx, double dy) {
	if (active && visible) {
		if (contains(x, y)) {
			if (! occluded && ! mouseHover) {
				mouseHover = true;
				onChangeState();
			} else if (occluded && mouseHover) {
				mouseHover = false;
				onChangeState();
			}
		} else if (mouseHover) {
			mouseHover = false;
			onChangeState();
		}
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMouseMoved(x, y, dx, dy);
	}
}

void GUIComponent::onMouseDragged(double x, double y, double dx, double dy) {
	if (active && visible) {
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMouseDragged(x, y, dx, dy);
	}
}

void GUIComponent::onMouseEnter() {
	if (active && visible) {
		if (! occluded && ! mouseHover && contains(Window::getCurrentInstance()->getInputManager()->getCursorData().lastX, Window::getCurrentInstance()->getInputManager()->getCursorData().lastY)) {
			mouseHover = true;
			onChangeState();
		}
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMouseEnter();
	}
}

void GUIComponent::onMouseLeave() {
	if (active && visible) {
		if (mouseClicked || mouseHover) {
			mouseClicked = false;
			mouseHover = false;
			onChangeState();
		}

		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onMouseLeave();
	}
}

void GUIComponent::onScroll(double dx, double dy) {
	if (active && visible) {
		//Pass the event to all attached components
		for (unsigned int i = 0; i < attachedComponents.size(); i++)
			attachedComponents[i]->onScroll(dx, dy);
	}
}

void GUIComponent::setActive(bool active) {
	this->active = active;

	//Reset the render index
	this->renderIndex = 0;

	//Assign the same thing in all attached components
	for (unsigned int i = 0; i < attachedComponents.size(); i++)
		attachedComponents[i]->setActive(active);
}

void GUIComponent::setVisible(bool visible) {
	this->visible = visible;

	//Assign the same thing in all attached components
	for (unsigned int i = 0; i < attachedComponents.size(); i++)
		attachedComponents[i]->setVisible(visible);
}

void GUIComponent::setOccluded(bool occluded) {
	this->occluded = occluded;

	//Assign the same thing in all attached components
	for (unsigned int i = 0; i < attachedComponents.size(); i++)
		attachedComponents[i]->setOccluded(occluded);
}
