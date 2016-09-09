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

#include "AsteroidsPauseMenu.h"
#include "AsteroidsGame.h"

#include "../../core/gui/GUILabel.h"

/*****************************************************************************
 * The AsteroidsPauseMenu class
 *****************************************************************************/

AsteroidsPauseMenu::AsteroidsPauseMenu(AsteroidsGame* game) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Setup the camera
	camera = new Camera2D(Matrix4f().initOrthographic(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f));
	camera->update();
	//Setup the background
	Texture* backgroundTexture = game->getResourceLoader().loadTexture("MainMenu_Background.png");
	background = new GameObject2D({ new Mesh(MeshBuilder::createQuad(windowWidth, windowHeight, backgroundTexture)) }, Renderer::getRenderShader("Material"));
	background->getMaterial()->setDiffuseTexture(backgroundTexture);
	background->getMaterial()->setDiffuseColour(Colour(1.0f, 1.0f, 1.0f, 0.4f));
	background->update();

	Texture* normal = game->getResourceLoader().loadTexture("Button.png");
	Texture* hover = game->getResourceLoader().loadTexture("Button_Hover.png");
	Texture* clicked = game->getResourceLoader().loadTexture("Button_Clicked.png");

	buttonContinue = new GUIButton("Continue", 400, 30, { normal, hover, clicked });
	buttonContinue->setPosition(windowWidth / 2 - buttonContinue->getWidth() / 2, 140);
	buttonContinue->addListener(this);

	buttonExit = new GUIButton("Exit", 400, 30, { normal, hover, clicked });
	buttonExit->setPosition(windowWidth / 2 - buttonExit->getWidth() / 2, windowHeight - 50);
	buttonExit->addListener(this);

	//Setup the title font
	Font* titleFont = game->getResourceLoader().loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Paused", titleFont);
	titleLabel->setPosition(game->getSettings().windowWidth / 2 - titleFont->getWidth("Paused") / 2, 40.0f);

	//Add the components to this panel
	add(titleLabel);
	add(buttonContinue);
	add(buttonExit);
}

AsteroidsPauseMenu::~AsteroidsPauseMenu() {
	//Delete created resources
	delete camera;
	delete background;
}

void AsteroidsPauseMenu::show() {
	game->getWindow()->enableCursor();
	GUIPanel::show();
}

void AsteroidsPauseMenu::hide() {
	game->getWindow()->disableCursor();
	GUIPanel::hide();
}

void AsteroidsPauseMenu::render(bool overrideShader) {
	//Add the camera
	Renderer::addCamera(camera);

	//Render the background
	background->render();

	//Render the panel
	GUIPanel::render();

	//Remove the camera
	Renderer::removeCamera();
}

void AsteroidsPauseMenu::onComponentClicked(GUIComponent* component) {
	//Check whether any buttons were clicked
	if (component == buttonContinue)
		//Change the game state to start the game
		game->changeState(AsteroidsGame::GAME_PLAYING);
	else if (component == buttonExit) {
		//Go to the main menu
		game->changeState(AsteroidsGame::GAME_PLAYING);
		game->changeState(AsteroidsGame::MAIN_MENU);
	}
}
