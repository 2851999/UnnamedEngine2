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

#include "GUIButton.h"

/*****************************************************************************
 * The GUISlider class
 *****************************************************************************/

class GUISlider : public GUIComponent {
public:
	/* The directions */
	enum Direction {
		VERTICAL,
		HORIZONTAL
	};
private:
	/* The slider button */
	GUIButton* button;

	/* The slider direction */
	Direction direction;

	/* The current position along the direction of this slider */
	float position;

	/* The current slider value */
	float value;

	/* The maximum and minimum values of this slider */
	float valueMin = 0.0f;
	float valueMax = 1.0f;

	/* The interval value - only values with this set interval are allowed,
	 * unless it is set to 0 */
	float interval;

	/* States whether the slider is dragging the button */
	bool dragging;
protected:
	/* The method called to update this component  */
	virtual void onComponentUpdate() override;
	/* The method called to render this component */
	virtual void onComponentRender() override;
public:
	/* The constructors */
	GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour);
	GUISlider(GUIButton* button, Direction direction, float width, float height, Texture* texture);
	GUISlider(GUIButton* button, Direction direction, float width, float height, Colour colour, Texture* texture);

	/* The destructor */
	virtual ~GUISlider();

	/* Methods used to enable/disable this component (Adds/Removes the input listener instance */
	virtual void enable() override;
	virtual void disable() override;

	/* The needed input methods */
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;
	virtual void onMouseDragged(double x, double y, double dx, double dy) override;
	virtual void onMousePressed(int button) override;
	virtual void onMouseReleased(int button) override;

	/* Setters and getters */
	inline void setValueMin(float min) { valueMin = min; }
	inline void setValueMax(float max) { valueMax = max; }
	inline void setInterval(float interval) { this->interval = interval; }

	inline GUIButton* getButton() { return button; }
	inline Direction getDirection() { return direction; }
	inline float getValue() { return value; }
	inline float getValueMin() { return valueMin; }
	inline float getValueMax() { return valueMax; }
	inline float getInterval() { return interval; }
};

