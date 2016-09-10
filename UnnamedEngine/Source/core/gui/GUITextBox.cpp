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

#include <cctype>
#include "GUITextBox.h"

/***************************************************************************************************
 * The GUITextBoxCursor class
 ***************************************************************************************************/

GUITextBoxCursor::GUITextBoxCursor(GUITextBox* textBox, float thickness) : GUIComponentRenderer(thickness, textBox->getHeight() - 2) {
	colour = Colour(-1.0f, -1.0f, -1.0f, -1.0f);
	setWidth(thickness);
	setup(textBox);
}

void GUITextBoxCursor::setup(GUITextBox* textBox) {
	this->textBox = textBox;
	//setHeight(textBox->getFont()->getHeight("A"));
	setColour(Colour::BLACK);
	timer = new Timer();
	timer->start();
	timeBetweenBlink = 600;
	cursorShown = false;
	if (colour.getR() != -1.0f)
		setColour(colour);
	if (texture != NULL)
		setTexture(texture);
}

Colour GUITextBoxCursor::getColour() {
	return getColours()[0];
}
Texture* GUITextBoxCursor::getTexture() {
	return getTextures()[0];
}

bool GUITextBoxCursor::hasColour() {
	return hasColours();
}

bool GUITextBoxCursor::hasTexture() {
	return hasTextures();
}

void GUITextBoxCursor::render() {
	if (timer->hasTimePassed(timeBetweenBlink)) {
		cursorShown = ! cursorShown;
		timer->restart();
	}
	if (cursorShown) {
		Vector2f p = textBox->getPosition();
		float x = 1 + p.getX() + (textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->cursorIndex - textBox->viewIndexStart)));
		float y = (p.getY() + (textBox->getHeight() / 2)) - (getHeight() / 2);
		setPosition(x, y);
		GUIComponentRenderer::update();
		GUIComponentRenderer::render();
	}
}

void GUITextBoxCursor::showCursor() {
	timer->restart();
	cursorShown = true;
}

/***************************************************************************************************/

/***************************************************************************************************
 * The GUITextBoxSelection class
 ***************************************************************************************************/

GUITextBoxSelection::GUITextBoxSelection(GUITextBox* textBox) : GUIFill(textBox->getWidth(), textBox->getHeight(), Colour::BLACK) {
	colour = Colour(-1.0f, -1.0f, -1.0f, -1.0f);
	setup(textBox);
}

void GUITextBoxSelection::setup(GUITextBox* textBox) {
	this->textBox = textBox;
	setSize(textBox->getWidth(), textBox->getHeight());
	if (colour.getR() != -1.0f)
		setColour(colour);
	if (texture != NULL)
		setTexture(texture);
}

void GUITextBoxSelection::render() {
	if (textBox->isSelection) {
		Vector2f p = textBox->getPosition();
		float selectionX = 0;
		try {
			if (textBox->selectionIndexStart < textBox->selectionIndexEnd)
				selectionX = p.getX() + textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->selectionIndexStart - textBox->viewIndexStart));
			else
				selectionX = p.getX() + textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->selectionIndexEnd - textBox->viewIndexStart));
		} catch (int e) {

		}
		float selectionY = p.getY();
		float selectionWidth = textBox->getFont()->getWidth(textBox->getRenderTextSelection());
		float selectionHeight = textBox->getHeight();
		updateWidth(selectionWidth);
		updateHeight(selectionHeight);
		position = Vector2f(selectionX, selectionY);
		GUIFill::update();
		GUIFill::render();
	}
}

/***************************************************************************************************/

/***************************************************************************************************
 * The GUITextBox class
 ***************************************************************************************************/

GUITextBox::GUITextBox(float width, float height) :
			GUIComponent(width, height) {
	setup();
}

GUITextBox::GUITextBox(Colour colour, float width, float height) :
			GUIComponent(width, height, std::vector<Colour> { colour }) {
	setup();
}

GUITextBox::GUITextBox(Texture* texture, float width, float height) :
			GUIComponent(width, height, std::vector<Texture*> { texture }) {
	setup();
}

GUITextBox::GUITextBox(Texture* texture, Colour colour, float width, float height) :
			GUIComponent(width, height, std::vector<Colour> { colour }, std::vector<Texture*> { texture }) {
	setup();
}

void GUITextBox::setup() {
	text = "";
	renderText = "";
	selected = false;
	masked = false;
	mask = "*";
	defaultText = "";
	cursorIndex = 0;
	cursor = new GUITextBoxCursor(this, 1.0f);
	viewIndexStart = 0;
	viewIndexEnd = 0;
	isSelection = false;
	selectionIndexStart = 0;
	selectionIndexEnd = 0;
	selection = new GUITextBoxSelection(this);
	shortcuts = new KeyboardShortcuts();
	shortcuts->addListener(this);

	std::vector<int> shiftLeftKeys1;
	shiftLeftKeys1.push_back(GLFW_KEY_LEFT_SHIFT);
	shiftLeftKeys1.push_back(GLFW_KEY_LEFT);
	std::vector<int> shiftLeftKeys2;
	shiftLeftKeys2.push_back(GLFW_KEY_RIGHT_SHIFT);
	shiftLeftKeys2.push_back(GLFW_KEY_LEFT);

	std::vector<int> shiftRightKeys1;
	shiftRightKeys1.push_back(GLFW_KEY_LEFT_SHIFT);
	shiftRightKeys1.push_back(GLFW_KEY_RIGHT);
	std::vector<int> shiftRightKeys2;
	shiftRightKeys2.push_back(GLFW_KEY_RIGHT_SHIFT);
	shiftRightKeys2.push_back(GLFW_KEY_RIGHT);

	std::vector<int> cutKeys1;
	cutKeys1.push_back(GLFW_KEY_LEFT_CONTROL);
	cutKeys1.push_back(GLFW_KEY_X);
	std::vector<int> cutKeys2;
	cutKeys2.push_back(GLFW_KEY_RIGHT_CONTROL);
	cutKeys2.push_back(GLFW_KEY_X);

	std::vector<int> copyKeys1;
	copyKeys1.push_back(GLFW_KEY_LEFT_CONTROL);
	copyKeys1.push_back(GLFW_KEY_C);
	std::vector<int> copyKeys2;
	copyKeys2.push_back(GLFW_KEY_RIGHT_CONTROL);
	copyKeys2.push_back(GLFW_KEY_C);

	std::vector<int> pasteKeys1;
	pasteKeys1.push_back(GLFW_KEY_LEFT_CONTROL);
	pasteKeys1.push_back(GLFW_KEY_V);
	std::vector<int> pasteKeys2;
	pasteKeys2.push_back(GLFW_KEY_RIGHT_CONTROL);
	pasteKeys2.push_back(GLFW_KEY_V);

	shortcuts->add(new KeyboardShortcut("Shift-Left", shiftLeftKeys1));
	shortcuts->add(new KeyboardShortcut("Shift-Left", shiftLeftKeys2));

	shortcuts->add(new KeyboardShortcut("Shift-Right", shiftRightKeys1));
	shortcuts->add(new KeyboardShortcut("Shift-Right", shiftRightKeys2));

	shortcuts->add(new KeyboardShortcut("Cut", cutKeys1));
	shortcuts->add(new KeyboardShortcut("Cut", cutKeys2));

	shortcuts->add(new KeyboardShortcut("Copy", copyKeys1));
	shortcuts->add(new KeyboardShortcut("Copy", copyKeys2));

	shortcuts->add(new KeyboardShortcut("Paste", pasteKeys1));
	shortcuts->add(new KeyboardShortcut("Paste", pasteKeys2));
}

void GUITextBox::onComponentUpdate() {}

void GUITextBox::onComponentRender() {
	Font* f = getFont();
	if (shouldUseDefaultText()) {
		renderText = defaultText;
		if (defaultTextFont != NULL)
			f = defaultTextFont;
	} else {
		updateRenderText();
		clipRenderText();
	}

	Vector2f p = getPosition();

	float textX = p.getX() + 1;
	float textY = (p.getY() + (getHeight() / 2) + (f->getHeight(renderText) / 2));

	f->render(renderText, textX, textY);

	if (selected)
		cursor->render();
	selection->render();
}

void GUITextBox::updateRenderText() {
	//Try and prevent any problems with the view index being out of bounds by accident
	if (viewIndexStart < 0)
		viewIndexStart = 0;
	if (viewIndexEnd > text.length())
		viewIndexEnd = text.length();

	renderText = StrUtils::substring(text, viewIndexStart, viewIndexEnd);

	if (masked)
		renderText = maskStr(renderText, mask);
}

std::string GUITextBox::maskStr(std::string s, std::string mask) {
	std::string maskedString = "";
	for (unsigned int a = 0; a < s.length(); a++)
		maskedString += mask;
	return maskedString;
}

void GUITextBox::clipRenderText() {
	float width = getWidth();
	while (getFont()->getWidth(renderText) >= width - 2 && cursorIndex != viewIndexStart) {
		viewIndexStart++;
		updateRenderText();
	}
	while (getFont()->getWidth(renderText) >= width && cursorIndex <= viewIndexEnd) {
		viewIndexEnd--;
		updateRenderText();
	}
}

void GUITextBox::moveCursor(double x) {
	cursorIndex = getIndex(x);
	cursor->showCursor();
}

int GUITextBox::getIndex(double x) {
	updateRenderText(); //Is this really necessary??
	const char* textValue = renderText.c_str();
	std::string currentString = "";
	int newPlace = 0;

	for (unsigned int a = 0; a < renderText.length(); a++) {
		currentString += textValue[a];

		double lookX = 0;
		double widthOfString = getFont()->getWidth(currentString);
		double widthOfLastCharacter = getFont()->getWidth(currentString.substr(currentString.length() - 1));

		//Add onto lookX the position this text box starts rendering the text
		lookX += position.getX() + 1;
		//Add onto lookX the width of the string - (the width of the last character / 2)
		lookX += widthOfString - (widthOfLastCharacter / 2);

		if (lookX < x)
			newPlace++;
		else
			break;
	}
	newPlace += viewIndexStart;
	return newPlace;
}

void GUITextBox::clear() {
	text = "";
	cursorIndex = 0;
	viewIndexStart = 0;
	viewIndexEnd = 0;
	resetSelection();
}

void GUITextBox::resetSelection() {
	isSelection = false;
	selectionIndexStart = 0;
	selectionIndexEnd = 0;
}

std::string GUITextBox::getSelection() {
	if (isSelection) {
		if (selectionIndexStart < selectionIndexEnd)
			return StrUtils::substring(text, selectionIndexStart, selectionIndexEnd);
		else
			return StrUtils::substring(text, selectionIndexEnd, selectionIndexStart);
	} else
		return "";
}

std::string GUITextBox::getRenderTextSelection() {
	if (isSelection) {
		updateRenderText();
		clipRenderText();

		unsigned int sis = selectionIndexStart;
		if (sis < viewIndexStart)
			sis = viewIndexStart;
		else if (sis > viewIndexEnd)
			sis = viewIndexEnd;

		unsigned int sie = selectionIndexEnd;
		if (sie < viewIndexStart)
			sie = viewIndexStart;
		else if (sie > viewIndexEnd)
			sie = viewIndexEnd;

		if (selectionIndexStart <= selectionIndexEnd)
			return StrUtils::substring(renderText, sis - viewIndexStart, sie - viewIndexStart);
		else
			return StrUtils::substring(renderText, sie - viewIndexStart, sis - viewIndexStart);
	} else
		return "";
}

void GUITextBox::deleteSelection() {
	std::string front = "";
	std::string back = "";

	if (selectionIndexStart < selectionIndexEnd) {
		front = StrUtils::substring(text, 0, selectionIndexStart);
		back = text.substr(selectionIndexEnd);
	} else {
		front = StrUtils::substring(text, 0, selectionIndexEnd);
		back = text.substr(selectionIndexStart);
	}

	int amountRemoved = text.length() - (front + back).length();

	int a = 0;

	while (a < amountRemoved) {
		if (viewIndexStart > 0)
			viewIndexStart--;
		a++;
	}
	text = front + back;

	if (cursorIndex > text.length())
		cursorIndex = text.length();

	resetSelection();
}

void GUITextBox::onComponentClicked() {
	if (! isSelection) {
		selected = true;
		resetSelection();
		cursor->showCursor();
	}
}

void GUITextBox::onKeyPressed(int key) {
	GUIComponent::onKeyPressed(key);
	if (visible && active && selected) {
		if (key == GLFW_KEY_BACKSPACE) {
			if (isSelection)
				deleteSelection();
			else {
				if (text.length() > 0 && cursorIndex > 0) {
					std::string front = StrUtils::substring(text, 0, cursorIndex);
					std::string back = text.substr(cursorIndex);

					text = StrUtils::substring(front, 0, front.length() - 1) + back;

					if (cursorIndex == viewIndexStart) {
						cursorIndex --;
						if (viewIndexStart > 0)
							viewIndexStart--;
						viewIndexEnd--;
					} else {
						cursorIndex--;
						viewIndexEnd--;

						if (viewIndexStart > 0)
							viewIndexStart--;
						else
							viewIndexEnd++;
					}
				}
			}
		} else if (key == GLFW_KEY_DELETE) {
			if (isSelection)
				deleteSelection();
			else {
				if (text.length() > 0 && cursorIndex < viewIndexEnd) {
					std::string front = StrUtils::substring(text, 0, cursorIndex);
					std::string back = text.substr(cursorIndex);

					text = front + back.substr(1);

					if (! (viewIndexEnd <= text.length())) {
						//Decrement the view's end index
						viewIndexEnd--;
						if (viewIndexStart > 0)
							//Decrement the view's start index (Keeps text at the beginning and end)
							viewIndexStart--;
					}
				}
			}
		} else if (key == GLFW_KEY_LEFT && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
			resetSelection();
			//Make sure the cursor's current index is more than 0
			if (cursorIndex > 0) {
				//Check the cursor index and viewing index
				if (cursorIndex == viewIndexStart) {
					//Check to see whether there is any unseen text
					if (viewIndexStart > 0) {
						//Decrement the cursor index
						cursorIndex--;
						//Decrement the start of the viewing index
						viewIndexStart--;
					}
				} else {
					//Decrement the cursor index
					cursorIndex--;
				}
			}
			//Show the cursor
			cursor->showCursor();
		} else if (key == GLFW_KEY_RIGHT && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
			resetSelection();
			//Make sure the cursor's current index is less than the length of the text
			if (cursorIndex < text.length()) {
				//Check the cursor index and viewing index
				if (cursorIndex == viewIndexEnd) {
					//Check to see whether there is any unseen text
					if (viewIndexEnd > 0) {
						//Increment the cursor index
						cursorIndex++;
						//Increment the end of the viewing index
						viewIndexEnd++;
					}
				} else {
					//Increment the cursor index
					cursorIndex++;
				}
			}
		}
		//Show the cursor
		cursor->showCursor();
	}
}

void GUITextBox::onChar(int key, char character) {
	GUIComponent::onChar(key, character);
	if (visible && active && selected) {
		if (key == GLFW_KEY_BACKSPACE) {
		} else if (key == GLFW_KEY_DELETE) {
		} else if (key == GLFW_KEY_LEFT && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
		} else if (key == GLFW_KEY_RIGHT && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && ! Window::getCurrentInstance()->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
		} else {
			if (isDefined(character) || character == ' ') {
				if (isSelection)
					deleteSelection();
				std::string front = StrUtils::substring(text, 0, cursorIndex);
				std::string back = text.substr(cursorIndex);

				text = front + character + back;

				//Check the viewing index and cursor index
				if (viewIndexStart == cursorIndex && viewIndexStart > 0) {
					//Increase the cursor index
					cursorIndex++;
				} else {
					//Increase the cursor index
					cursorIndex++;
					viewIndexEnd++;
				}
			}
		}
		//Show the cursor
		cursor->showCursor();
	}
}

void GUITextBox::onMousePressed(int button) {
	GUIComponent::onMousePressed(button);
	if (! isMouseHovering())
		selected = false;
	else if (selected && button == GLFW_MOUSE_BUTTON_LEFT) {
		moveCursor(Window::getCurrentInstance()->getInputManager()->getCursorData().lastX);
		resetSelection();
	}
}

void GUITextBox::onMouseDragged(double x, double y, double dx, double dy) {
	GUIComponent::onMouseDragged(x, y, dx, dy);
	//Make sure this is selected
	if (visible && active && selected) {
		//Check to see whether there is a selection
		if (! isSelection) {
			//Set the selection values
			selectionIndexStart = getIndex(x);
			selectionIndexEnd = selectionIndexStart;
			isSelection = true;
		} else {
			//Set the new selection index end
			selectionIndexEnd = getIndex(x);
			//Move the cursor
			moveCursor(x);

			//Check the index values
			if (viewIndexStart == cursorIndex) {
				//Keep some text visible if there is more
				if (cursorIndex > 0)
					cursorIndex--;
				if (viewIndexStart > 0)
					viewIndexStart--;
				if (selectionIndexEnd > 0)
					selectionIndexEnd--;
			}
			if (viewIndexEnd == cursorIndex) {
				//Keep some text visible if there is more
				if (cursorIndex < text.length())
					cursorIndex++;
				if (viewIndexEnd < text.length())
					viewIndexEnd++;
				if (selectionIndexEnd < text.length())
					selectionIndexEnd++;
			}
		}
	}
}

void GUITextBox::onShortcut(KeyboardShortcut* e) {
	if (e->name == "Shift-Left") {
		//Make sure the cursor's current index is more than 0
		if (cursorIndex > 0) {
			//Check the cursor index and viewing index
			if (cursorIndex == viewIndexStart) {
				//Check to see whether there is any unseen text
				if (viewIndexStart > 0) {
					//Decrement the cursor index
					cursorIndex--;
					//Decrement the start of the viewing index
					viewIndexStart--;
				}
			} else {
				//Decrement the cursor index
				cursorIndex--;
			}
		}
		if (! isSelection) {
			isSelection = true;
			selectionIndexStart = cursorIndex + 1;
			selectionIndexEnd = selectionIndexStart;
		}
		if (selectionIndexEnd > 0)
			selectionIndexEnd--;
	} else if (e->name == "Shift-Right") {
		//Make sure the cursor's current index is less than the length of the text
		if (cursorIndex < text.length()) {
			//Check the cursor index and viewing index
			if (cursorIndex == viewIndexEnd) {
				//Check to see whether there is any unseen text
				if (viewIndexEnd > 0) {
					//Increment the cursor index
					cursorIndex++;
					//Increment the end of the viewing index
					viewIndexEnd++;
				}
			} else {
				//Increment the cursor index
				cursorIndex++;
			}
		}
		if (! isSelection) {
			isSelection = true;
			selectionIndexStart = cursorIndex - 1;
			selectionIndexEnd = selectionIndexStart;
		}
		if (selectionIndexEnd < text.length())
			selectionIndexEnd++;
	} else if (e->name == "Cut") {
		if (isSelection) {
			ClipboardUtils::setText(getSelection());
			deleteSelection();
		}
	} else if (e->name == "Paste") {
		if (isSelection)
			deleteSelection();

		std::string front = StrUtils::substring(text, 0, cursorIndex);
		std::string back = text.substr(cursorIndex);

		text = front + ClipboardUtils::getText() + back;

		cursorIndex = text.length() - back.length();
		viewIndexEnd = text.length();
	} else if (e->name == "Copy") {
		if (isSelection)
			ClipboardUtils::setText(getSelection());
	}
}

bool GUITextBox::isDefined(char character) {
	return ((character != '\u0000') && (! iscntrl(character)) && isgraph(character) && isprint(character));
}

bool GUITextBox::shouldUseDefaultText() {
	return ! selected && text.length() == 0 && defaultText.length() > 0;
}

void GUITextBox::setText(std::string text) {
	this->text = text;
	viewIndexEnd = text.length();
}

/***************************************************************************************************/
