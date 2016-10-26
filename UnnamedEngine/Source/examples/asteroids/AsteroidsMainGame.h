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
#include "EnemiesRenderer.h"
#include "HUD.h"
#include "GameOverMenu.h"
#include "UpgradesMenu.h"

class Enemy;
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

	/* The HUD */
	HUD* hud;

	/* The game over menu */
	GameOverMenu* gameOverMenu;

	/* The upgrades menu */
	UpgradesMenu* upgradesMenu;

	/* States whether the upgrades menu is showing */
	bool showUpgradesMenu = false;

	/* The asteroid renderer */
	AsteroidsRenderer* asteroidRenderer;

	/* The asteroid groups */
	std::vector<AsteroidGroup> asteroidGroups;

	/* The enemies */
	std::vector<Enemy*> enemies;

	/* The enemies renderer */
	EnemiesRenderer* enemiesRenderer;

	/* The pause button */
	InputBindingButton* pauseButton;

	/* The upgrades button */
	InputBindingButton* upgradesButton;

	/* Timer for the game */
	Timer* timer;

	/* The time before the next enemy */
	float timeForNextEnemy;
public:
	/* The constructor */
	AsteroidsMainGame(AsteroidsGame* game);

	/* The destructor */
	virtual ~AsteroidsMainGame();

	/* The method used to start the game */
	void start();

	/* The method used to stop the game */
	void stop();

	/* Method that finds and returns a reference to the closest asteroid
	 * group to a position */
	AsteroidGroup& findClosestAsteroids(const Vector3f& position);

	/* Method used to spawn another enemy (if possible) */
	void spawnEnemy();

	/* The update and render methods */
	void update();
	void render();

	/* Methods to show/hide the upgrades menu */
	void showUpgrades();
	void hideUpgrades();

	inline bool showingUpgrades() { return showUpgradesMenu; }

	inline Player* getPlayer() { return player; }

	/* InputBindingListener methods */
	virtual void onButtonReleased(InputBindingButton* button) override;
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSMAINGAME_H_ */
