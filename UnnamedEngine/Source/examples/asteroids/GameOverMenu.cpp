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

#include "GameOverMenu.h"
#include "AsteroidsGame.h"
#include "Player.h"

/*****************************************************************************
 * The GameOverMenu class
 *****************************************************************************/

GameOverMenu::GameOverMenu(AsteroidsGame* game, Player* player) : game(game), player(player) {
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
	background->getMaterial()->setDiffuseColour(Colour(1.0f, 1.0f, 1.0f, 0.8f));
	background->update();

	buttonExit = new GUIButton("Exit", 400, 30, game->getResources().getTexturesButtons());
	buttonExit->setPosition(windowWidth / 2 - buttonExit->getWidth() / 2, windowHeight - 50);
	buttonExit->addListener(this);

	//Setup the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Create the label's
	labelGameOver = new GUILabel("Game Over", titleFont);
	labelGameOver->setPosition(windowWidth / 2 - labelGameOver->getWidth() / 2, windowHeight / 2 - labelGameOver->getHeight());

	Font* headerFont = game->getResources().getFontHeading();

	labelScore = new GUILabel("", headerFont);

	//Add the components to this panel
	add(labelGameOver);
	add(labelScore);
	add(buttonExit);
}

GameOverMenu::~GameOverMenu() {
	//Delete created resources
	delete camera;
	delete background;
}

void GameOverMenu::show() {
	game->getWindow()->enableCursor();
	labelScore->setText("You scored " + StrUtils::str(player->getScore()) + "!");
	labelScore->setPosition(game->getSettings().windowWidth / 2 - labelScore->getWidth() / 2, labelGameOver->getPosition().getY() + labelGameOver->getHeight() + 10.0f);
	GUIPanel::show();
}

void GameOverMenu::hide() {
	game->getWindow()->disableCursor();
	GUIPanel::hide();
}

void GameOverMenu::render(bool overrideShader) {
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Add the camera
	Renderer::addCamera(camera);

	//Render the background
	background->render();

	//Render the panel
	GUIPanel::render();

	//Remove the camera
	Renderer::removeCamera();
}

void GameOverMenu::onComponentClicked(GUIComponent* component) {
	//Check whether any buttons were clicked
	if (component == buttonExit)
		//Go to the main menu
		game->changeState(AsteroidsGame::MAIN_MENU);
}
