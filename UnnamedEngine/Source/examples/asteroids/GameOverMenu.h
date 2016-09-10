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

#ifndef EXAMPLES_ASTEROIDS_GAMEOVERMENU_H_
#define EXAMPLES_ASTEROIDS_GAMEOVERMENU_H_

#include "../../core/render/Renderer.h"
#include "../../core/render/Camera.h"
#include "../../core/gui/Font.h"
#include "../../core/gui/GUIPanel.h"
#include "../../core/gui/GUILabel.h"
#include "../../core/gui/GUITextBox.h"

/*****************************************************************************
 * The GameOverMenu class sets up and manages the pause menu
 *****************************************************************************/

class AsteroidsGame;
class Player;

class GameOverMenu : public GUIPanel {
private:
	/* The instance of the game */
	AsteroidsGame* game;

	/* The camera used when rendering the menu */
	Camera2D* camera;

	/* The background for the menu */
	GameObject2D* background;

	/* The player instance */
	Player* player;

	/* The labels */
	GUILabel* labelGameOver;
	GUILabel* labelScore;

	/* Name textbox */
	GUITextBox* nameTextBox;

	/* The buttons on the menu */
	GUIButton* buttonExit;
public:
	/* The constructor */
	GameOverMenu(AsteroidsGame* game, Player* player);

	/* The destructor */
	virtual ~GameOverMenu();

	/* Method called to setup ready to show the menu */
	virtual void show() override;

	/* Method called to remove the camera and hide the menu */
	virtual void hide() override;

	/* Methods used to render the menu */
	virtual void render(bool overrideShader = false) override;

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;
};

#endif /* EXAMPLES_ASTEROIDS_GAMEOVERMENU_H_ */
