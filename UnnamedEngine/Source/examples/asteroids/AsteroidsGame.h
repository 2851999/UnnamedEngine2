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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSGAME_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSGAME_H_

#include "../../core/BaseEngine.h"
#include "../../core/ResourceLoader.h"
#include "../../core/input/InputBindings.h"
#include "../../core/audio/SoundSystem.h"

#include "AsteroidsMainMenu.h"
#include "AsteroidsMainGame.h"
#include "AsteroidsPauseMenu.h"
#include "AsteroidsResources.h"
#include "HighScores.h"

/*****************************************************************************
 * The AsteroidsGame class sets up and manages the game
 *****************************************************************************/

class AsteroidsGame : public BaseEngine {
public:
	/* The current game state */
	enum GameState {
		MAIN_MENU,
		GAME_PLAYING,
		GAME_PAUSED
	};
private:
	/* The resource loader instance used for loading resources */
	ResourceLoader resourceLoader;

	/* The commonly used resources */
	AsteroidsResources resources;

	/* The current game state */
	GameState currentState;

	/* The sound system managing all of the sounds */
	SoundSystem* soundSystem = NULL;

	/* Input bindings for the game */
	InputBindings* inputBindings = NULL;

	/* The main menu */
	AsteroidsMainMenu* mainMenu = NULL;

	/* The main game */
	AsteroidsMainGame* mainGame = NULL;

	/* The pause menu */
	AsteroidsPauseMenu* pauseMenu = NULL;

	/* The highscores */
	HighScores highScores;

	/* The camera 2D instance used for 2D graphics */
	Camera2D* camera2D = NULL;
public:
	/* The constructor */
	AsteroidsGame();

	/* The destructor */
	virtual ~AsteroidsGame();

	/* Overrided methods from BaseEngine */
	virtual void initialise() override;
	virtual void created() override;
	virtual void update() override;
	virtual void render() override;
	virtual void destroy() override;

	/* Method used to change the current state of the game */
	void changeState(GameState newState);

	/* Setters and getters */
	ResourceLoader& getResourceLoader() { return resourceLoader; }
	HighScores& getHighScores() { return highScores; }
	const AsteroidsResources& getResources() { return resources; }
	GameState getCurrentState() { return currentState; }
	AsteroidsMainGame* getMainGame() { return mainGame; }
	SoundSystem* getSoundSystem() { return soundSystem; }
	InputBindings* getInputBindings() { return inputBindings; }
	Camera2D* getCamera2D() { return camera2D; }
};


#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSGAME_H_ */
