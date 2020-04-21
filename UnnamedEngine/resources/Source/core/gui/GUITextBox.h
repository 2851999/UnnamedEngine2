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

#pragma once

#include "GUIComponent.h"

#include "../input/KeyboardShortcuts.h"
#include "../../utils/Timer.h"

/*****************************************************************************
 * The GUITextBoxCursor class
 *****************************************************************************/

class GUITextBox;

class GUITextBoxCursor : public GUIComponentRenderer {
public:
	/* Pointer to the text box this cursor is for */
	GUITextBox* textBox = NULL;

	/* Timer used for making the cursor blink */
	Timer* timer = NULL;

	/* The time between each cursor blink (in milliseconds) */
	long timeBetweenBlink;

	/* States whether the cursor is currently shown */
	bool cursorShown;

	/* The constructor */
	GUITextBoxCursor(GUITextBox* textBox, float thickness);

	/* The destructor */
	virtual ~GUITextBoxCursor();

	/* Method called to setup this text box cursor given the text box this is
	 * for - automatically called by the constructor */
	void setup(GUITextBox* textBox);

	/* Methods used to get the colour/texture and determine whether they have
	 * been assigned */
	Colour getColour();
	Texture* getTexture();
	bool hasColour();
	bool hasTexture();

	/* Method used to render this cursor */
	void render();

	/* Method called to show the cursor */
	void showCursor();

	/* Setters and getters */
	inline void setTimeBetweenBlink(long timeBetweenBlink) { this->timeBetweenBlink = timeBetweenBlink; }
	inline long getTimeBetweenBlink() { return timeBetweenBlink; }
	inline bool isCursorShowing() { return cursorShown; }
};

/*****************************************************************************
 * The GUITextBoxSelection class
 *****************************************************************************/

class GUITextBoxSelection : public GUIFill {
public:
	/* Pointer to the text box this selection is for */
	GUITextBox* textBox = NULL;

	/* The constructor */
	GUITextBoxSelection(GUITextBox* textBox);

	/* The destructor */
	virtual ~GUITextBoxSelection();

	/* Method called to setup this selection given the text box it is for -
	 * automatically called by the constructor */
	void setup(GUITextBox* textBox);

	/* Method called to render this selection */
	void render();
};

/*****************************************************************************
 * The GUITextBox class
 *****************************************************************************/

class GUITextBox : public GUIComponent, KeyboardShortcutListener {
protected:
	/* Inherited GUIComponent methods used to update/render this text box */
	virtual void onComponentUpdate() override;
	virtual void onComponentRender() override;

	/* Method called when this component is clicked */
	virtual void onComponentClicked() override;
public:
	/* The text within this text box */
	std::string text;

	/* The text that will actually be rendered by this text box */
	std::string renderText;

	/* States whether this text box is selected, and therefore can be typed
	 * in */
	bool selected;

	/* This is used to tell the text box to mask all of the text */
	bool masked;

	/* This mask is used when masked = true, and will replace each character
	 * in text */
	std::string mask;

	/* The default text will be rendered when there is nothing in this text
	 * box and it isn't selected */
	std::string defaultText;

	/* The font used when rendering the default text */
	Font* defaultTextFont = NULL;

	/* The colour the default text should be rendered using */
	Colour defaultTextColour;

	/* The current cursor index - this is the position in the text the cursor
	 * currently is */
	unsigned int cursorIndex;

	/* Instance of the text box cursor */
	GUITextBoxCursor* cursor = NULL;

	/* The viewing indices state the start/end of the text that is currently
	 * in view */
	unsigned int viewIndexStart;
	unsigned int viewIndexEnd;

	/* States whether there is currently a selection */
	bool isSelection;

	/* These are the same as the viewing indices but determine the start/end
	 * of the current selection */
	unsigned int selectionIndexStart;
	unsigned int selectionIndexEnd;

	/* Selection instance used to render the selection */
	GUITextBoxSelection* selection = NULL;

	/* Keyboard shortcuts instance used to handle keyboard shortcuts within
	 * the text box */
	KeyboardShortcuts* shortcuts = NULL;

	/* The constructors */
	GUITextBox(float width, float height);
	GUITextBox(Colour colour, float width, float height);
	GUITextBox(Texture* texture, float width, float height);
	GUITextBox(Texture* texture, Colour colour, float width, float height);

	/* The destructors */
	virtual ~GUITextBox();

	void setColour(Colour colour);
	void setTexture(Texture* texture);
	Colour getColour();
	Texture* getTexture();
	bool hasColour();
	bool hasTexture();

	/* Called to setup the text box - automatically called in the
	 * constructors */
	void setup();

	/* Called to update the render text */
	void updateRenderText();

	/* Returns a masked string given the string to mask and its mask */
	std::string maskStr(const std::string& s, const std::string& mask);

	/* This is called to clip the render text to make sure it fits in the
	 * text box */
	void clipRenderText();

	/* Method called to move the cursor given its position within the
	 * window */
	void moveCursor(double x);

	/* Returns the index of the character closest to a given x position */
	int getIndex(double x);

	/* Method called to clear the text box of all text */
	void clear();

	/* Called to reset the selection within the text box */
	void resetSelection();

	/* Returns the selection */
	std::string getSelection();

	/* Returns the selection that is in view */
	std::string getRenderTextSelection();

	/* Removes the currently selected text and then resets the selection */
	void deleteSelection();

	/* Overridden input methods */
	virtual void onKeyPressed(int code) override;
	virtual void onChar(int code, char character) override;
	virtual void onMousePressed(int button) override;
	virtual void onMouseDragged(double x, double y, double dx, double dy) override;
	virtual void onShortcut(KeyboardShortcut* e) override;

	/* Returns whether the given character is an allowed one that can be
	 * rendered */
	bool isDefined(char character);

	/* Returns whether the default text should be rendered */
	bool shouldUseDefaultText();

	/* Method used to set the text within this text box */
	void setText(std::string text);

	/* Setters and getters */
	inline void setRenderText(std::string renderText) { this->renderText = renderText; }
	inline void setSelected(bool selected) { this->selected = selected; }
	inline void setMasked(bool masked) { this->masked = masked; }
	inline void setMask(std::string mask) { this->mask = mask; }
	inline void setDefaultText(std::string defaultText) { this->defaultText = defaultText; }
	inline void setDefaultTextFont(Font* defaultTextFont) { this->defaultTextFont = defaultTextFont; }
	inline void setDefaultTextColour(Colour defaultTextColour) { this->defaultTextColour = defaultTextColour; }
	inline void setCursorIndex(int cursorIndex) { this->cursorIndex = cursorIndex; }
	inline std::string getText() { return text; }
	inline std::string getRenderText() { return renderText; }
	inline bool isSelected() { return selected; }
	inline bool isMasked() { return masked; }
	inline std::string getMask() { return mask; }
	inline std::string getDefaultText() { return defaultText; }
	inline Font* getDefaultTextFont() { return defaultTextFont; }
	inline int getCursorIndex() { return cursorIndex; }
};

/***************************************************************************************************/

