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
	GUIButton* buttonHighscores;
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

	/* The highscores label */
	GUILabel* highscoresLabel;

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
private:
	/* The buttons on the menu */
	GUIButton* buttonVideo;
	GUIButton* buttonAudio;
	GUIButton* buttonControls;
	GUIButton* buttonBack;
public:
	/* The constructor */
	MainMenuSettings(AsteroidsGame* game, GUIPanelGroup* panelGroup);

	/* The destructor */
	virtual ~MainMenuSettings() {}
};

/*****************************************************************************
 * The MainMenuVideo class
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
	/* The buttons on the menu */
	GUIButton* buttonApply;
	GUIButton* buttonBack;
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
 * The AsteroidsMainMenu class sets up and manages the main menu GUI
 *****************************************************************************/

class AsteroidsMainMenu {
private:
	/* The camera used when rendering the main menu */
	Camera2D* camera;

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
