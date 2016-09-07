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

	//Add a group of asteroids
	for (unsigned int x = 0; x < 5; x++) {
		for (unsigned int y = 0; y < 5; y++) {
			for (unsigned int z = 0; z < 5; z++) {
				AsteroidGroup group(Vector3f(x * 200, y * 200, z * 200));
				group.generateAsteroids(10, asteroidRenderer);
				asteroidGroups.push_back(group);
			}
		}
	}

	asteroidRenderer->update();
	//Hiding 0 seems to break with transparency glitch - fixed by discarding fragment
	//asteroidRenderer->hideAsteroid(0);
}

AsteroidsMainGame::~AsteroidsMainGame() {
	//Delete created resources
	delete player;
	delete asteroidRenderer;
}

void AsteroidsMainGame::start() {
	//Hide the mouse
	game->getWindow()->disableCursor();
	//Add the camera
	Renderer::addCamera(player->getCamera());
}

void AsteroidsMainGame::stop() {
	//Show the mouse
	game->getWindow()->disableCursor();
	//Remove the camera
	Renderer::removeCamera();
}

void AsteroidsMainGame::update() {
	//Update the camera
	player->update();

	unsigned int closestIndex = 0;
	float distance = 10000000;

	for (unsigned int i = 0; i < asteroidGroups.size(); i++) {
		float current = (asteroidGroups[i].getPosition() - player->getCamera()->getPosition()).length();

		if (current < distance) {
			distance = current;
			closestIndex = i;
		}
	}

	std::vector<GameObject3D*> closestAsteroids = asteroidGroups[closestIndex].getObjects();
	std::vector<PhysicsObject3D*> lasers = player->getLasers();
	//Go through asteroids in the group
	for (unsigned int i = 0; i < closestAsteroids.size(); i++) {
		//NEED TO CHANGE

		for (unsigned int j = 0; j < lasers.size(); j++) {
			float distance = (lasers[j]->getPosition() - closestAsteroids[i]->getPosition()).length();
			if (distance < 10.0f) {
				asteroidRenderer->hideAsteroid((closestIndex * 10) + i);
			}
		}
	}
}

void AsteroidsMainGame::render() {
	//Setup for rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_MULTISAMPLE_ARB);
//	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	//Use the player's view
	player->useView();

	asteroidRenderer->render();
}
