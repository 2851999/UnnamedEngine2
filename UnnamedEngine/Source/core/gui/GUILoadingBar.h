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

#ifndef CORE_GUI_GUILOADINGBAR_H_
#define CORE_GUI_GUILOADINGBAR_H_

#include "GUIComponent.h"

/*****************************************************************************
 * The GUILoadingBar class
 *****************************************************************************/

class GUILoadingBar : public GUIComponent {
private:
	/* The GUIFIll instance used to fill up the loading bar */
	GUIFill* fill;

	/* The total number of stages and current number that are completed */
	unsigned int totalStages;
	unsigned int currentStage = 0;
protected:
	/* The method called to update this component */
	virtual void onComponentUpdate() override;

	/* The method called to render this component */
	virtual void onComponentRender() override;
public:
	/* The constructors */
	GUILoadingBar(float width, float height, unsigned int totalStages) :
		GUIComponent(width, height, { Colour::WHITE }), totalStages(totalStages) {

		fill = new GUIFill(width, height, Colour::YELLOW);
		fill->setParent(this);
	}

	GUILoadingBar(float width, float height, unsigned int totalStages, Colour backgroundColour, Colour fillColour) :
		GUIComponent(width, height, { backgroundColour }), totalStages(totalStages) {

		fill = new GUIFill(width, height, fillColour);
		fill->setParent(this);
	}

	GUILoadingBar(float width, float height, unsigned int totalStages, Texture* backgroundTexture, Texture* fillTexture) :
		GUIComponent(width, height, { backgroundTexture }), totalStages(totalStages) {

		fill = new GUIFill(width, height, fillTexture);
		fill->setParent(this);
	}

	GUILoadingBar(float width, float height, unsigned int totalStages, Colour backgroundColour, Texture* backgroundTexture, Colour fillColour, Texture* fillTexture) :
		GUIComponent(width, height, { backgroundColour }, { backgroundTexture }), totalStages(totalStages) {

		fill = new GUIFill(width, height, fillColour, fillTexture);
		fill->setParent(this);
	}

	/* The destructor */
	virtual ~GUILoadingBar() { delete fill; }

	/* The method called to change the loading bar as a stage has been completed */
	void completedStage();
};

#endif /* CORE_GUI_GUILOADINGBAR_H_ */
