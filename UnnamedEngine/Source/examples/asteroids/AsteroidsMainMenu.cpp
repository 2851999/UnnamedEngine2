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

#include "AsteroidsMainMenu.h"
#include "AsteroidsGame.h"

/*****************************************************************************
 * The AsteroidsMainMenu class
 *****************************************************************************/

AsteroidsMainMenu::AsteroidsMainMenu(AsteroidsGame* game) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Setup the camera
	camera = new Camera2D(Matrix4f().initOrthographic(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f));
	camera->update();
	//Setup the background
	Texture* backgroundTexture = game->getResourceLoader().loadTexture("MainMenu_Background.png");
	background = new GameObject2D(new Mesh(MeshBuilder::createQuad(windowWidth, windowHeight, backgroundTexture)), Renderer::getRenderShader("Material"));
	background->getMaterial()->setDiffuseTexture(backgroundTexture);
	background->update();
	//Setup the title font
	titleFont = game->getResourceLoader().loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);
	titleFont->update(
			"Asteroids",
			game->getSettings().windowWidth / 2 - titleFont->getWidth("Asteroids") / 2,
			80.0f
	);
	//Setup the buttons
	GUIComponentRenderer::DEFAULT_FONT = game->getResourceLoader().loadFont("TT1240M_.TTF", 24, Colour::WHITE);

	Texture* normal = game->getResourceLoader().loadTexture("Button.png");
	Texture* hover = game->getResourceLoader().loadTexture("Button_Hover.png");
	Texture* clicked = game->getResourceLoader().loadTexture("Button_Clicked.png");

	buttonPlay = new GUIButton("Play", 400, 30, { normal, hover, clicked });
	buttonPlay->setPosition(windowWidth / 2 - buttonPlay->getWidth() / 2, 140);
	buttonPlay->addListener(this);

	buttonExit = new GUIButton("Exit", 400, 30, { normal, hover, clicked });
	buttonExit->setPosition(windowWidth / 2 - buttonPlay->getWidth() / 2, windowHeight - 50);
	buttonExit->addListener(this);
}

AsteroidsMainMenu::~AsteroidsMainMenu() {
	//Delete created resources
	delete camera;
	delete background;
	delete buttonPlay;
	delete buttonExit;
}

void AsteroidsMainMenu::show() {
	//Add the camera
	Renderer::addCamera(camera);
}

void AsteroidsMainMenu::hide() {
	//Remove the camera
	Renderer::removeCamera();
}

void AsteroidsMainMenu::update() {
	//Update the buttons
	buttonPlay->update();
	buttonExit->update();
}

void AsteroidsMainMenu::render() {
	//Setup for rendering
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Render the background
	background->render();

	//Render the font
	titleFont->render();

	//Render the buttons
	buttonPlay->render();
	buttonExit->render();
}

void AsteroidsMainMenu::onComponentClicked(GUIComponent* component) {
	//Check whether any buttons were clicked
	if (component == buttonPlay)
		//Change the game state to start the game
		game->changeState(AsteroidsGame::GAME_PLAYING);
	else if (component == buttonExit)
		//Exit the game
		game->requestClose();
}
