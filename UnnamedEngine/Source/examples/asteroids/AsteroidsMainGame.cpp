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

/*****************************************************************************
 * The AsteroidsMainGame class
 *****************************************************************************/

AsteroidsMainGame::AsteroidsMainGame(AsteroidsGame* game) : game(game) {
	//Setup the camera
	camera = new DebugCamera(Matrix4f().initPerspective(110.0f, game->getSettings().windowAspectRatio, 0.1f, 100.0f));
	camera->setSkyBox(new SkyBox(game->getResourceLoader().getPath() + "skybox/", "skyboxBK.png", "skyboxFT.png", "skyboxLF.png", "skyboxRT.png", "skyboxUP.png", "skyboxDN.png", 100.0f));
	camera->setFlying(true);

	asteroids = new AsteroidsRenderer(game->getResourceLoader());
	asteroids->update();
}

AsteroidsMainGame::~AsteroidsMainGame() {
	//Delete created resources
	delete camera;
	delete asteroids;
}

void AsteroidsMainGame::start() {
	//Hide the mouse
	game->getWindow()->disableCursor();
	//Add the camera
	Renderer::addCamera(camera);
}

void AsteroidsMainGame::stop() {
	//Show the mouse
	game->getWindow()->disableCursor();
	//Remove the camera
	Renderer::removeCamera();
}

void AsteroidsMainGame::update() {
	//Update the camera
	camera->update(game->getDeltaSeconds());
}

void AsteroidsMainGame::render() {
	//Setup for rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Use the camera's view
	camera->useView();

	asteroids->render();
}
