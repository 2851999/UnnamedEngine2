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
	background = new GameObject2D({ new Mesh(MeshBuilder::createQuad(windowWidth, windowHeight, backgroundTexture)) }, "Material");
	background->getMaterial()->diffuseTexture = backgroundTexture;
	background->getMaterial()->diffuseColour = Colour(1.0f, 1.0f, 1.0f, 0.8f);
	background->update();

	buttonExit = new GUIButton("Exit", 400, 30, game->getResources().getTexturesButtons());
	buttonExit->setPosition(windowWidth / 2 - buttonExit->getWidth() / 2, windowHeight - 50.0f);

	//Setup the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Create the label's
	labelGameOver = new GUILabel("Game Over", titleFont);
	labelGameOver->setPosition(windowWidth / 2 - labelGameOver->getWidth() / 2, windowHeight / 2 - labelGameOver->getHeight() - 100.0f);

	Font* headerFont = game->getResources().getFontHeading();

	labelScore = new GUILabel("", headerFont);

	//Setup the name textbox
	nameTextBox = new GUITextBox(game->getResources().getTexturesButtons()[0], 200, 30);
	nameTextBox->setFont(headerFont);
	nameTextBox->setDefaultTextFont(headerFont);
	nameTextBox->setPosition(windowWidth / 2 - nameTextBox->getWidth() / 2, labelGameOver->getPosition().getY() + labelGameOver->getHeight() + 80.0f);
	nameTextBox->setDefaultText("Enter your name");
	nameTextBox->setBorder(new GUIBorder(nameTextBox, 1.0f, Colour(Colour::BLACK, 0.1f)));
	nameTextBox->selection->setColour(Colour(Colour::LIGHT_BLUE, 0.2f));

	//Add the components to this panel
	add(labelGameOver);
	add(labelScore);
	add(nameTextBox);
	add(buttonExit);
}

GameOverMenu::~GameOverMenu() {
	//Delete created resources
	delete camera;
	delete background;
}

void GameOverMenu::show() {
	game->getWindow()->enableCursor();

	GUIPanel::show();

	//Get a reference to the high scores
	HighScores& highScores = game->getHighScores();

	if (highScores.isHighScore(player->getScore())) {
		labelScore->setText("You scored " + StrUtils::str(player->getScore()) + " - A new highscore! :)");
		nameTextBox->setActive(true);
		nameTextBox->setVisible(true);
	} else if (highScores.isOnTable(player->getScore())) {
		labelScore->setText("You scored " + StrUtils::str(player->getScore()) + " - You're on the highscore table!");
		nameTextBox->setActive(true);
		nameTextBox->setVisible(true);
	} else {
		nameTextBox->disable();
		nameTextBox->setActive(false);
		nameTextBox->setVisible(false);

		labelScore->setText("You scored " + StrUtils::str(player->getScore()) + " :(");
	}

	labelScore->setPosition(game->getSettings().windowWidth / 2 - labelScore->getWidth() / 2, labelGameOver->getPosition().getY() + labelGameOver->getHeight() + 10.0f);
}

void GameOverMenu::hide() {
	game->getWindow()->disableCursor();
	GUIPanel::hide();
}

void GameOverMenu::render() {
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
	if (component == buttonExit) {
		//Check whether a highscore should be added
		if (nameTextBox->isActive() && nameTextBox->getText().length() > 0) {
			//Add the score
			game->getHighScores().add(nameTextBox->getText(), player->getScore());
			//Save the high scores
			game->getHighScores().save();
		}
		//Go to the main menu
		game->changeState(AsteroidsGame::MAIN_MENU);
	}
}
