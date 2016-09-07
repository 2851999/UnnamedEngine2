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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSPAUSEMENU_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSPAUSEMENU_H_

#include "../../core/render/Renderer.h"
#include "../../core/render/Camera.h"
#include "../../core/gui/Font.h"
#include "../../core/gui/GUIButton.h"

/*****************************************************************************
 * The AsteroidsPauseMenu class sets up and manages the pause menu
 *****************************************************************************/

class AsteroidsGame;

class AsteroidsPauseMenu : GUIComponentListener {
private:
	/* The instance of the game */
	AsteroidsGame* game;

	/* The camera used when rendering the main menu */
	Camera2D* camera;

	/* The background for the main menu */
	GameObject2D* background;

	/* The title font */
	Font* titleFont;

	/* The buttons on the menu */
	GUIButton* buttonContinue;
	GUIButton* buttonExit;
public:
	/* The constructor */
	AsteroidsPauseMenu(AsteroidsGame* game);

	/* The destructor */
	virtual ~AsteroidsPauseMenu();

	/* Method called to setup ready to show the main menu */
	void show();

	/* Method called to remove the camera and hide the main menu */
	void hide();

	/* Methods used to update and render the main menu */
	void update();
	void render();

	/* Called when a component is clicked */
	virtual void onComponentClicked(GUIComponent* component) override;
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSMAINMENU_H_ */
