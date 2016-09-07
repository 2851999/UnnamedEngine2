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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSMAINGAME_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSMAINGAME_H_

class AsteroidsGame;

#include "../../core/input/InputBindings.h"
#include "AsteroidsRenderer.h"
#include "AsteroidGroup.h"

class Player;

/*****************************************************************************
 * The AsteroidsMainGame class sets up and manages the main game
 *****************************************************************************/

class AsteroidsMainGame : public InputBindingsListener {
private:
	/* The instance of the game */
	AsteroidsGame* game;

	/* The player */
	Player* player;

	/* The asteroid renderer */
	AsteroidsRenderer* asteroidRenderer;

	/* The asteroid groups */
	std::vector<AsteroidGroup> asteroidGroups;

	/* The pause button */
	InputBindingButton* pauseButton;
public:
	/* The constructor */
	AsteroidsMainGame(AsteroidsGame* game);

	/* The destructor */
	virtual ~AsteroidsMainGame();

	/* The method used to start the game */
	void start();

	/* The method used to stop the game */
	void stop();

	/* The update and render methods */
	void update();
	void render();

	/* InputBindingListener methods */
	virtual void onButtonReleased(InputBindingButton* button) override;
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSMAINGAME_H_ */
