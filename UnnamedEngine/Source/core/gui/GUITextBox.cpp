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

/*****************************************************************************
 * The GUITextBoxCursor class
 *****************************************************************************/

GUITextBoxCursor::GUITextBoxCursor(GUITextBox* textBox, float thickness) : GUIComponentRenderer(thickness, textBox->getHeight() - 2) {
	//Set the width
	setWidth(thickness);
	//Setup this cursor
	setup(textBox);
}

GUITextBoxCursor::~GUITextBoxCursor() {

}

void GUITextBoxCursor::setup(GUITextBox* textBox) {
	//Assign the text box instance
	this->textBox = textBox;
	//setHeight(textBox->getFont()->getHeight("A"));
	//Assign the default colour
	setColour(Colour::BLACK);
	//Create and start the timer
	timer = new Timer();
	timer->start();
	//Assign the default time between blink
	timeBetweenBlink = 600;
	//Cursor starts off hidden
	cursorShown = false;
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
	//Check whether the required time has passed for the cursor to change
	//state
	if (timer->hasTimePassed(timeBetweenBlink)) {
		//Change the cursor's current state
		cursorShown = ! cursorShown;
		//Restart the timer
		timer->restart();
	}
	//Check whether the cursor is currently shown
	if (cursorShown) {
		//Get the text box's current position
		Vector2f p = textBox->getPosition();
		//Calculate the x and y position the cursor should be on the screen
		float x = 1 + p.getX() + (textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->cursorIndex - textBox->viewIndexStart)));
		float y = (p.getY() + (textBox->getHeight() / 2)) - (getHeight() / 2);
		//Assign the cursor position
		setPosition(x, y);
		//Update and render the cursor
		GUIComponentRenderer::update();
		GUIComponentRenderer::render();
	}
}

void GUITextBoxCursor::showCursor() {
	//Restart the timer and show the cursor
	timer->restart();
	cursorShown = true;
}

/*****************************************************************************
 * The GUITextBoxSelection class
 *****************************************************************************/

GUITextBoxSelection::GUITextBoxSelection(GUITextBox* textBox) : GUIFill(textBox->getWidth(), textBox->getHeight(), Colour::BLACK) {
	//Setup this selecion
	setup(textBox);
}

GUITextBoxSelection::~GUITextBoxSelection() {

}

void GUITextBoxSelection::setup(GUITextBox* textBox) {
	//Assign the text box
	this->textBox = textBox;
	//Assign the maximum width/height
	setSize(textBox->getWidth(), textBox->getHeight());
}

void GUITextBoxSelection::render() {
	//Check whether there is a selection within the text box
	if (textBox->isSelection) {
		//Get the position of the text box
		Vector2f p = textBox->getPosition();
		//The x position of the start of the selection
		float selectionX = 0;
		try {
			//Check whether the start of the selection is before or after
			//the end of the selection
			if (textBox->selectionIndexStart < textBox->selectionIndexEnd)
				selectionX = p.getX() + textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->selectionIndexStart - textBox->viewIndexStart));
			else
				selectionX = p.getX() + textBox->getFont()->getWidth(StrUtils::substring(textBox->renderText, 0, textBox->selectionIndexEnd - textBox->viewIndexStart));
		} catch (int e) {

		}
		//The y position of the selection is the same as the y position
		//of the text box
		float selectionY = p.getY();
		//Get the width and height of the selection
		float selectionWidth = textBox->getFont()->getWidth(textBox->getRenderTextSelection());
		float selectionHeight = textBox->getHeight();
		//Update this fill to the current dimensions of the selection
		updateWidth(selectionWidth);
		updateHeight(selectionHeight);
		//Assign the position
		setPosition(selectionX, selectionY);

		//Update and render this fill
		GUIFill::update();
		GUIFill::render();
	}
}

/*****************************************************************************
 * The GUITextBox class
 *****************************************************************************/

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

GUITextBox::~GUITextBox() {
	//Delete the created resources
	delete cursor;
	delete selection;
	delete shortcuts;
}

void GUITextBox::setup() {
	//Assign all of the default values
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
	//Setup the keyboard shortcuts
	shortcuts = new KeyboardShortcuts();
	shortcuts->addListener(this);

	shortcuts->add(new KeyboardShortcut("Shift-Left", std::vector<int> { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT }));
	shortcuts->add(new KeyboardShortcut("Shift-Left", std::vector<int> { GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_LEFT }));

	shortcuts->add(new KeyboardShortcut("Shift-Right", std::vector<int> { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT }));
	shortcuts->add(new KeyboardShortcut("Shift-Right", std::vector<int> { GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_RIGHT }));

	shortcuts->add(new KeyboardShortcut("Cut", std::vector<int> { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_X }));
	shortcuts->add(new KeyboardShortcut("Cut", std::vector<int> { GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_X }));

	shortcuts->add(new KeyboardShortcut("Copy", std::vector<int> { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C }));
	shortcuts->add(new KeyboardShortcut("Copy", std::vector<int> { GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_C }));

	shortcuts->add(new KeyboardShortcut("Paste", std::vector<int> { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_V }));
	shortcuts->add(new KeyboardShortcut("Paste", std::vector<int> { GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_V }));
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

std::string GUITextBox::maskStr(const std::string& s, const std::string& mask) {
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
	if (e->getName() == "Shift-Left") {
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
	} else if (e->getName() == "Shift-Right") {
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
	} else if (e->getName() == "Cut") {
		if (isSelection) {
			ClipboardUtils::setText(getSelection());
			deleteSelection();
		}
	} else if (e->getName() == "Paste") {
		if (isSelection)
			deleteSelection();

		std::string front = StrUtils::substring(text, 0, cursorIndex);
		std::string back = text.substr(cursorIndex);

		text = front + ClipboardUtils::getText() + back;

		cursorIndex = text.length() - back.length();
		viewIndexEnd = text.length();
	} else if (e->getName() == "Copy") {
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
