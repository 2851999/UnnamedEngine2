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
	//Assign the current game state
	currentState = MAIN_MENU;
}

AsteroidsGame::~AsteroidsGame() {

}

void AsteroidsGame::initialise() {
	//Setup the resource loader
	resourceLoader.setPath("C:/UnnamedEngine/examples/asteroids/");
	resourceLoader.setPathTextures("textures/");
	resourceLoader.setPathFonts("fonts/");
	resourceLoader.setPathModels("models/");
	resourceLoader.setPathShaders("shaders/");
	resourceLoader.setPathAudio("audio/");

	//Setup the settings
	getSettings() = SettingsUtils::readFromFile(resourceLoader.getPath() + "settings/settings.txt");
	getSettings().windowTitle = "Asteroids";
	getSettings().videoSamples = 16;
	getSettings().videoMaxAnisotropicSamples = 16;
	getSettings().videoVSync = true;
	getSettings().videoMaxFPS = 0;

	//Setup the highscores
	highscores.setPath(resourceLoader.getPath() + "highscores.txt");
	//Load the highscores
	highscores.load();

	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;
}

void AsteroidsGame::created() {
	//Setup the resources
	resources.setup(resourceLoader);

	//Setup the sound system
	soundSystem = new SoundSystem();

	//Load the sounds
	soundSystem->createListener();

	//Create the InputBindings instance
	inputBindings = new InputBindings();
	//Create the axis bindings
	InputBindingAxis* axisForward = inputBindings->createAxisBinding("Forward");
	InputBindingAxis* axisSideways = inputBindings->createAxisBinding("Sideways");
	//Create the button bindings
	InputBindingButton* buttonShoot = inputBindings->createButtonBinding("Shoot");
	InputBindingButton* buttonPause = inputBindings->createButtonBinding("Pause");
	//Setup default keys
	axisForward->assignKeys(GLFW_KEY_W, GLFW_KEY_S);
	axisSideways->assignKeys(GLFW_KEY_A, GLFW_KEY_D);
	buttonShoot->assignKey(GLFW_KEY_SPACE);
	buttonPause->assignKey(GLFW_KEY_ESCAPE);

	//Setup the main menu
	mainMenu = new AsteroidsMainMenu(this);
	mainMenu->show();

	//Setup the main game
	mainGame = new AsteroidsMainGame(this);

	//Setup the pause menu
	pauseMenu = new AsteroidsPauseMenu(this);
}

void AsteroidsGame::update() {
	//Update the sound system
	soundSystem->update();
	//Check the current state
	if (currentState == MAIN_MENU)
		mainMenu->update();
	else if (currentState == GAME_PLAYING)
		mainGame->update();
	else if (currentState == GAME_PAUSED)
		pauseMenu->update();
}

void AsteroidsGame::render() {
	//Check the current state
	if (currentState == MAIN_MENU)
		mainMenu->render();
	else if (currentState == GAME_PLAYING)
		mainGame->render();
	else if (currentState == GAME_PAUSED) {
		mainGame->render();
		pauseMenu->render();
	}
}

void AsteroidsGame::destroy() {
	//Delete all created resources
	delete soundSystem;
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
		else if (currentState == GAME_PLAYING && newState != GAME_PAUSED) {
			mainGame->stop();
			soundSystem->stopAll();
		} else if (currentState == GAME_PAUSED)
			pauseMenu->hide();

		//Check the new state
		if (newState == MAIN_MENU)
			//Show the main menu
			mainMenu->show();
		else if (newState == GAME_PLAYING && currentState != GAME_PAUSED)
			mainGame->start();
		else if (newState == GAME_PLAYING && currentState == GAME_PAUSED)
			soundSystem->resumeAll();
		else if (newState == GAME_PAUSED) {
			pauseMenu->show();
			soundSystem->pauseAll();
		}

		//Assign the state
		currentState = newState;
	}
}

