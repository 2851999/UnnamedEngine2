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

#ifndef EXAMPLES_ASTEROIDS_MAINMENU_H_
#define EXAMPLES_ASTEROIDS_MAINMENU_H_

#include "../../core/gui/GUIButton.h"

class Asteroids;

class MainMenu {
private:
	/* The font used for the title */
	Font* font;

	/* The play button */
	GUIButton* buttonPlay;

	/* The background */
	GameObject2D* background;
public:
	/* The constructor */
	MainMenu(float windowWidth, float windowHeight);

	/* The destructor */
	virtual ~MainMenu();

	/* The update and render methods */
	void update(Asteroids* asteroids);
	void render();

	bool shouldStartGame();
};

#endif /* EXAMPLES_ASTEROIDS_MAINMENU_H_ */
