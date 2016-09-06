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

#include "AsteroidsGame.h"

/*****************************************************************************
 * The AsteroidsGame class
 *****************************************************************************/

AsteroidsGame::AsteroidsGame() {
	//Setup the resource loader
	resourceLoader.setPath("C:/UnnamedEngine/examples/asteroids/");
	resourceLoader.setPathTextures("textures/");
	resourceLoader.setPathFonts("fonts/");
	resourceLoader.setPathModels("models/");
	resourceLoader.setPathShaders("shaders/");
	//Assign the current game state
	currentState = MAIN_MENU;
}

AsteroidsGame::~AsteroidsGame() {

}

void AsteroidsGame::initialise() {
	//Setup the settings
	getSettings().windowTitle = "Asteroids";
	getSettings().videoSamples = 16;
	getSettings().videoMaxAnisotropicSamples = 16;
	getSettings().videoVSync = true;
	getSettings().videoMaxFPS = 0;
}

void AsteroidsGame::created() {
	//Setup the main menu
	mainMenu = new AsteroidsMainMenu(this);
	mainMenu->show();

	//Setup the main game
	mainGame = new AsteroidsMainGame(this);
}

void AsteroidsGame::update() {
	//Check the current state
	if (currentState == MAIN_MENU)
		mainMenu->update();
	else if (currentState == GAME_PLAYING)
		mainGame->update();
}

void AsteroidsGame::render() {
	//Check the current state
	if (currentState == MAIN_MENU)
		mainMenu->render();
	else if (currentState == GAME_PLAYING)
		mainGame->render();
}

void AsteroidsGame::destroy() {
	//Delete all created resources
	delete mainMenu;
	delete mainGame;
}

void AsteroidsGame::changeState(GameState newState) {
	//Ensure it is a different state
	if (currentState != newState) {
		//Check the current state
		if (currentState == MAIN_MENU)
			//Hide the main menu
			mainMenu->hide();
		else if (currentState == GAME_PLAYING)
			mainGame->stop();

		//Check the new state
		if (newState == MAIN_MENU)
			//Show the main menu
			mainMenu->show();
		else if (newState == GAME_PLAYING)
			mainGame->start();

		//Assign the state
		currentState = newState;
	}
}

