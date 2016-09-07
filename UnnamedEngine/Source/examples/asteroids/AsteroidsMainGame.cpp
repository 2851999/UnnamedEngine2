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

/*****************************************************************************
 * The AsteroidsMainGame class
 *****************************************************************************/

AsteroidsMainGame::AsteroidsMainGame(AsteroidsGame* game) : game(game) {
	//Setup the player
	player = new Player(game);

	//Setup the asteroid renderer
	unsigned int numAsteroids = 1250;
	asteroidRenderer = new AsteroidsRenderer(game->getResourceLoader(), numAsteroids);

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

	asteroidRenderer->update();
	//Hiding 0 seems to break with transparency glitch - fixed by discarding fragment
	//asteroidRenderer->hideAsteroid(0);

	//Setup the pause button
	game->getInputBindings()->addListener(this);
	pauseButton = game->getInputBindings()->getButtonBinding("Pause");
}

AsteroidsMainGame::~AsteroidsMainGame() {
	//Delete created resources
	delete player;
	delete asteroidRenderer;
}

void AsteroidsMainGame::start() {
	//Hide the mouse
	game->getWindow()->disableCursor();
	//Setup the sound system
	game->getSoundSystem()->setListener(player);
	//Add the camera
	Renderer::addCamera(player->getCamera());
}

void AsteroidsMainGame::stop() {
	//Show the mouse
	game->getWindow()->enableCursor();
	//Setup the sound system
	game->getSoundSystem()->setListener(Vector2f());
	//Remove the camera
	Renderer::removeCamera();
}

void AsteroidsMainGame::update() {
	//The index of the closest asteroid group
	unsigned int closestIndex = 0;
	//The current smallest distance to an asteroid group
	float distance = 10000000;

	//Go though each group
	for (unsigned int i = 0; i < asteroidGroups.size(); i++) {
		//Calculate the distance to the current group
		float current = (asteroidGroups[i].getPosition() - player->getCamera()->getPosition()).length();

		//Check whether the current distance is smaller than the current smallest distance
		if (current < distance) {
			//It is, so assign the new smallest distance and the index of the closest asteroid group
			distance = current;
			closestIndex = i;
		}
	}

	//Update the player
	player->update(asteroidGroups[closestIndex]);
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

	asteroidRenderer->render();
}

void AsteroidsMainGame::onButtonReleased(InputBindingButton* button) {
	if (button == pauseButton) {
		if (game->getCurrentState() == AsteroidsGame::GAME_PLAYING)
			game->changeState(AsteroidsGame::GAME_PAUSED);
		else if (game->getCurrentState() == AsteroidsGame::GAME_PAUSED)
			game->changeState(AsteroidsGame::GAME_PLAYING);
	}
}
