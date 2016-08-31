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

#include "Asteroids.h"

Asteroids::~Asteroids() {
	delete mainMenu;
	delete mainGame;
	delete camera2D;
}

void Asteroids::initialise() {
	getSettings().windowTitle = "Asteroids";
	getSettings().videoSamples = 16;
	getSettings().videoMaxAnisotropicSamples = 16;

	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;
}

void Asteroids::created() {
	mainMenu = new MainMenu(getSettings().windowWidth, getSettings().windowHeight);

	camera2D = new Camera2D(Matrix4f().initOrthographic(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1));
	camera2D->update();

	Renderer::addCamera(camera2D);

	mainGame = new MainGame(this, getSettings().windowWidth, getSettings().windowHeight);
}

void Asteroids::update() {
	if (currentState == MAIN_MENU)
		mainMenu->update(this);
	else if (currentState == PLAYING)
		mainGame->update();
}

void Asteroids::render() {
	if (currentState == MAIN_MENU)
		mainMenu->render();
	else if (currentState == PLAYING)
		mainGame->render();
}

void Asteroids::destroy() {

}

void Asteroids::startGame() {
	currentState = PLAYING;
	getWindow()->toggleCursor();
	mainGame->startGame();
}
