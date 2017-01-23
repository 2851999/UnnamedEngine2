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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSMAINMENU_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSMAINMENU_H_

#include "../../core/render/Renderer.h"
#include "../../core/render/Camera.h"
#include "../../core/gui/Font.h"
#include "../../core/gui/GUIPanel.h"
#include "../../core/gui/GUIDropDownList.h"
#include "../../core/gui/GUICheckBox.h"
#include "../../core/gui/GUILabel.h"
//#include "../../core/gui/GUISlider.h"
#include "../../core/input/InputBindings.h"

class AsteroidsGame;

/*****************************************************************************
 * The MainMenuMain class
 *****************************************************************************/

class MainMenuMain : public GUIPanel {
private:
	/* The instance of the game */
	AsteroidsGame* game;

	/* The buttons on the menu */
	GUIButton* buttonPlay;
	GUIButton* buttonHighScores;
	GUIButton* buttonSettings;
	GUIButton* buttonExit;
public:
	/* The constructor */
	MainMenuMain(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuMain() {}

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;
};

/*****************************************************************************
 * The MainMenuHighScores class
 *****************************************************************************/

class MainMenuHighScores : public GUIPanel {
private:
	/* The game instance */
	AsteroidsGame* game;

	/* The high scores label */
	GUILabel* highScoresLabel;

	/* The buttons on the menu */
	GUIButton* buttonBack;
public:
	/* The constructor */
	MainMenuHighScores(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuHighScores() {}

	/* Method used to show this GUIPanel */
	virtual void show() override;
};

/*****************************************************************************
 * The MainMenuSettings class
 *****************************************************************************/

class MainMenuSettings : public GUIPanel {
public:
	/* The constructor */
	MainMenuSettings(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuSettings() {}
};

/*****************************************************************************
 * The MainMenuSettingsVideo class
 *****************************************************************************/

class MainMenuSettingsVideo : public GUIPanel {
private:
	/* The game instance */
	AsteroidsGame* game;
	/* Various other GUIComponents */
	GUIDropDownList* dropDownListResolutions;
	GUICheckBox* checkBoxFullscreen;
	GUICheckBox* checkBoxBorderless;
	GUICheckBox* checkBoxVSync;
//	GUISlider* sliderFOV;
	/* The buttons on the menu */
	GUIButton* buttonApply;
public:
	/* The constructor */
	MainMenuSettingsVideo(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuSettingsVideo() {}

	/* Method used to show this GUIPanel */
	virtual void show() override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;
};

/*****************************************************************************
 * The MainMenuSettingsControls class
 *****************************************************************************/

class MainMenuSettingsControls : public GUIPanel, InputBindingsListener {
private:
	/* The game instance */
	AsteroidsGame* game;
	/* The game's input bindings */
	InputBindings* bindings;
	/* The buttons on the menu */
	GUIButton* buttonForwardPos;
	GUIButton* buttonForwardNeg;
	GUIButton* buttonLookXPos;
	GUIButton* buttonLookXNeg;
	GUIButton* buttonLookYPos;
	GUIButton* buttonLookYNeg;
	GUIButton* buttonShoot;
	GUIButton* buttonUpgrades;
	GUIButton* buttonSave;

	/* Method used to update the GUI */
	void updateGUI();
public:
	/* The constructor */
	MainMenuSettingsControls(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuSettingsControls() {}

	/* Method used to show this GUIPanel */
	virtual void show() override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;

	/* Called when a button/axis has been waiting for input and has now
	 * received it */
	virtual void onButtonAssigned(InputBindingButton* button) override;
	virtual void onAxisAssigned(InputBindingAxis* axis) override;
};

/*****************************************************************************
 * The AsteroidsMainMenu class sets up and manages the main menu GUI
 *****************************************************************************/

class AsteroidsMainMenu {
private:
	/* The game instance */
	AsteroidsGame* game;

	/* The background for the main menu */
	GameObject2D* background;

	/* The GUIPanelGroup instance */
	GUIPanelGroup* panelGroup;
public:
	/* The constructor */
	AsteroidsMainMenu(AsteroidsGame* game);

	/* The destructor */
	virtual ~AsteroidsMainMenu();

	/* Method called to setup ready to show the main menu */
	void show();

	/* Method called to remove the camera and hide the main menu */
	void hide();

	/* Methods used to update and render the main menu */
	void update();
	void render();
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSMAINMENU_H_ */
