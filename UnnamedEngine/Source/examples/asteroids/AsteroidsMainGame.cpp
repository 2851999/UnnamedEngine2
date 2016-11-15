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

#include "AsteroidsMainGame.h"
#include "AsteroidsGame.h"
#include "Player.h"
#include "Enemy.h"

/*****************************************************************************
 * The AsteroidsMainGame class
 *****************************************************************************/

AsteroidsMainGame::AsteroidsMainGame(AsteroidsGame* game) : game(game) {
	//Setup the player
	player = new Player(game, enemies);

	//Setup the HUD
	hud = new HUD(game, player);

	//Setup the game over menu
	gameOverMenu = new GameOverMenu(game, player);

	//Setup the upgrades menu
	upgradesMenu = new UpgradesMenu(game, this);
	showUpgradesMenu = false;

	//Get the resource loader
	ResourceLoader& loader = game->getResourceLoader();

	//Setup the asteroid renderer
	unsigned int numAsteroids = 1250;
	asteroidRenderer = new GameRenderer(loader.loadModel("asteroid_model.obj")[0], loader.loadShader("AsteroidShader"), numAsteroids, true, true, true);

	//The current number of asteroids generated
	unsigned int currentNumAsteroids = 0;
	//Add a group of asteroids
	for (int x = -2; x < 3; x++) {
		for (int y = -2; y < 3; y++) {
			for (int z = -2; z < 3; z++) {
				AsteroidGroup group(asteroidRenderer, currentNumAsteroids, Vector3f(x * 100, y * 100, z * 100));
				group.generateAsteroids(10, asteroidRenderer);
				asteroidGroups.push_back(group);

				//Add to the current number of asteroids
				currentNumAsteroids += 10;
			}
		}
	}

	unsigned int numEnemies = 10;
	enemiesRenderer = new GameRenderer(loader.loadModel("enemyship.obj")[0], loader.loadShader("AsteroidShader"), numEnemies, false, true, false);

	//Go through the enemies
	for (unsigned int i = 0; i < numEnemies; i++) {
		//Create an enemy instance
		Enemy* enemy = new Enemy(game, player);
		//Add the enemy
		enemiesRenderer->add(enemy);
		enemies.push_back(enemy);
	}

	//asteroidRenderer->update();
	//Hiding 0 seems to break with transparency glitch - fixed by discarding fragment
	//asteroidRenderer->hideAsteroid(0);

	//Create the timer
	timer = new Timer();
	timeForNextEnemy = 0;

	//Setup the pause button
	game->getInputBindings()->addListener(this);
	pauseButton = game->getInputBindings()->getButtonBinding("Pause");
	upgradesButton = game->getInputBindings()->getButtonBinding("Upgrades");
}

AsteroidsMainGame::~AsteroidsMainGame() {
	//Delete created resources
	delete gameOverMenu;
	delete upgradesMenu;
	delete player;
	delete hud;
	delete asteroidRenderer;
	//Go through and delete the enemy objects
	for (unsigned int i = 0; i < enemies.size(); i++)
		delete enemies[i];
	enemies.clear();
}

void AsteroidsMainGame::start() {
	//Go through the asteroid groups
	for (unsigned int i = 0; i < asteroidGroups.size(); i++)
		//Setup the current group
		asteroidGroups[i].setup();
	//Ensure all of the asteroids can be seen
	asteroidRenderer->showAll();
	asteroidRenderer->updateAll();

	//Setup the enemies
	enemiesRenderer->hideAll();
	for (unsigned int i = 0; i < enemies.size(); i++) {
		enemies[i]->reset();
		enemies[i]->setHealth(0);
	}
	enemiesRenderer->updateAll();

	//Reset the player ship
	player->reset();

	//Show the HUD
	hud->show();
	//Hide the game over menu
	gameOverMenu->hide();

	//Hide the mouse
	game->getWindow()->disableCursor();
	//Setup the sound system
	game->getSoundSystem()->setListener(player);
	//Add the camera
	Renderer::addCamera(player->getCamera());

	//Assign the time until the next enemy
	timeForNextEnemy = 2.0f;

	//Start the timer
	timer->start();
}

void AsteroidsMainGame::stop() {
	//Hide the HUD
	hud->hide();
	//Hide the game over menu
	gameOverMenu->hide();

	//Show the mouse
	game->getWindow()->enableCursor();
	//Setup the sound system
	game->getSoundSystem()->setListener(Vector2f());
	//Remove the camera
	Renderer::removeCamera();
}

AsteroidGroup& AsteroidsMainGame::findClosestAsteroids(const Vector3f& position) {
	//The index of the closest asteroid group
	unsigned int closestIndex = 0;
	//The current smallest distance to an asteroid group
	float distance = 10000000;

	//Go though each group
	for (unsigned int i = 0; i < asteroidGroups.size(); i++) {
		//Calculate the distance to the current group
		float current = (asteroidGroups[i].getPosition() - position).length();

		//Check whether the current distance is smaller than the current smallest distance
		if (current < distance) {
			//It is, so assign the new smallest distance and the index of the closest asteroid group
			distance = current;
			closestIndex = i;
		}
	}
	//Return the closest group
	return asteroidGroups[closestIndex];
}

void AsteroidsMainGame::update() {
	if (showUpgradesMenu)
		upgradesMenu->update();
	else {
		//Get the closest asteroids to the player
		AsteroidGroup& playerClosestAsteroids = findClosestAsteroids(player->getCamera()->getPosition());
		//Update the player
		player->update(game->getDeltaSeconds(), playerClosestAsteroids);

		//Update the closest asteroids to the player
		playerClosestAsteroids.update(game->getDeltaSeconds(), player);

		//Update the enemies
		for (unsigned int i = 0; i < enemies.size(); i++) {
			enemies[i]->update(game->getDeltaSeconds(), findClosestAsteroids(enemies[i]->getPosition()));
			//Check whether it is still alive
			if (! enemies[i]->isAlive() && enemiesRenderer->isVisible(i))
				//Reset the enemy and move them somewhere else
				enemies[i]->reset();
		}

		//Check whether another enemy should spawn
		if (timer->hasTimePassedSeconds(timeForNextEnemy))
			//Spawn a new enemy
			spawnEnemy();

		//Update the enemies renderer
		enemiesRenderer->updateAll();

		if (! player->isAlive()) {
			if (! gameOverMenu->isVisible())
				gameOverMenu->show();
			gameOverMenu->update();
		}
	}

	//Update the HUD
	hud->update();
}

void AsteroidsMainGame::spawnEnemy() {
	//Go through the enemies
	for (unsigned int i = 0; i < enemies.size(); i++) {
		if (! enemies[i]->isAlive()) {
			enemies[i]->reset();
			enemiesRenderer->show(i);

			//Assign the time until the next enemy
			timeForNextEnemy = RandomUtils::randomFloat(10.0f, 60.0f);

			//Reset the timer
			timer->restart();

			break;
		}
	}
}

void AsteroidsMainGame::render() {
	//Setup for rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	//Render the player
	player->render();

	//Render the asteroids
	asteroidRenderer->render();

	//Render the enemies
	enemiesRenderer->render();
	for (unsigned int i = 0; i < enemies.size(); i++)
			enemies[i]->render();

	//Render the HUD
	hud->render();

	if (! player->isAlive()) {
		gameOverMenu->render();
	}

	if (showUpgradesMenu)
		upgradesMenu->render();
}

void AsteroidsMainGame::showUpgrades() {
	upgradesMenu->show();
	showUpgradesMenu = true;
}

void AsteroidsMainGame::hideUpgrades() {
	showUpgradesMenu = false;
	upgradesMenu->hide();
}

void AsteroidsMainGame::onButtonReleased(InputBindingButton* button) {
	if (player->getHealth() > 0) {
		if (button == pauseButton && ! showUpgradesMenu) {
			if (game->getCurrentState() == AsteroidsGame::GAME_PLAYING)
				game->changeState(AsteroidsGame::GAME_PAUSED);
			else if (game->getCurrentState() == AsteroidsGame::GAME_PAUSED)
				game->changeState(AsteroidsGame::GAME_PLAYING);
		} else if (button == upgradesButton && game->getCurrentState() == AsteroidsGame::GAME_PLAYING) {
			if (showUpgradesMenu)
				hideUpgrades();
			else
				showUpgrades();
		}
	}
}
