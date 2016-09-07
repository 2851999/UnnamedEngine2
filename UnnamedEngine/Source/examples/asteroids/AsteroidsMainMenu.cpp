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

#include "../../core/gui/GUILabel.h"

/*****************************************************************************
 * The MainMenuMain class
 *****************************************************************************/

MainMenuMain::MainMenuMain(AsteroidsGame* game, GUIPanelGroup* panelGroup, std::vector<Texture*> buttonTextures) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	buttonPlay = new GUIButton("Play", 400, 30, buttonTextures);
	buttonPlay->setPosition(windowWidth / 2 - buttonPlay->getWidth() / 2, 140);
	buttonPlay->addListener(this);

	buttonHighscores = new GUIButton("Highscores", 400, 30, buttonTextures);
	buttonHighscores->setPosition(windowWidth / 2 - buttonHighscores->getWidth() / 2, 180);
	buttonHighscores->addListener(this);
	panelGroup->assignButton(buttonHighscores, "Highscores");

	buttonSettings = new GUIButton("Settings", 400, 30, buttonTextures);
	buttonSettings->setPosition(windowWidth / 2 - buttonSettings->getWidth() / 2, 220);
	buttonSettings->addListener(this);
	panelGroup->assignButton(buttonSettings, "Settings");

	buttonExit = new GUIButton("Exit", 400, 30, buttonTextures);
	buttonExit->setPosition(windowWidth / 2 - buttonExit->getWidth() / 2, windowHeight - 50);
	buttonExit->addListener(this);

	//Setup the title font
	Font* titleFont = game->getResourceLoader().loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Asteroids", titleFont);
	titleLabel->setPosition(game->getSettings().windowWidth / 2 - titleFont->getWidth("Asteroids") / 2, 40.0f);

	//Add the components to this panel
	add(titleLabel);
	add(buttonPlay);
	add(buttonHighscores);
	add(buttonSettings);
	add(buttonExit);
}

void MainMenuMain::onComponentClicked(GUIComponent* component) {
	//Check whether any buttons were clicked
	if (component == buttonPlay)
		//Change the game state to start the game
		game->changeState(AsteroidsGame::GAME_PLAYING);
	else if (component == buttonExit)
		//Exit the game
		game->requestClose();
}

/*****************************************************************************
 * The MainMenuHighScores class
 *****************************************************************************/

MainMenuHighScores::MainMenuHighScores(AsteroidsGame* game, GUIPanelGroup* panelGroup, std::vector<Texture*> buttonTextures) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	buttonBack = new GUIButton("Back", 400, 30, buttonTextures);
	buttonBack->setPosition(windowWidth / 2 - buttonBack->getWidth() / 2, windowHeight - 50);
	buttonBack->addListener(this);
	panelGroup->assignButton(buttonBack, "Main");

	//Setup the title font
	Font* titleFont = game->getResourceLoader().loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Highscores", titleFont);
	titleLabel->setPosition(game->getSettings().windowWidth / 2 - titleFont->getWidth("Highscores") / 2, 40.0f);

	//Add the components to this panel
	add(titleLabel);
	add(buttonBack);
}

/*****************************************************************************
 * The MainMenuSettings class
 *****************************************************************************/

MainMenuSettings::MainMenuSettings(AsteroidsGame* game, GUIPanelGroup* panelGroup, std::vector<Texture*> buttonTextures) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	buttonVideo = new GUIButton("Video", 400, 30, buttonTextures);
	buttonVideo->setPosition(windowWidth / 2 - buttonVideo->getWidth() / 2, 140);
	buttonVideo->addListener(this);

	buttonAudio = new GUIButton("Audio", 400, 30, buttonTextures);
	buttonAudio->setPosition(windowWidth / 2 - buttonAudio->getWidth() / 2, 180);
	buttonAudio->addListener(this);

	buttonControls = new GUIButton("Controls", 400, 30, buttonTextures);
	buttonControls->setPosition(windowWidth / 2 - buttonControls->getWidth() / 2, 220);
	buttonControls->addListener(this);

	buttonBack = new GUIButton("Back", 400, 30, buttonTextures);
	buttonBack->setPosition(windowWidth / 2 - buttonBack->getWidth() / 2, windowHeight - 50);
	buttonBack->addListener(this);
	panelGroup->assignButton(buttonBack, "Main");

	//Setup the title font
	Font* titleFont = game->getResourceLoader().loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Settings", titleFont);
	titleLabel->setPosition(game->getSettings().windowWidth / 2 - titleFont->getWidth("Settings") / 2, 40.0f);

	//Add the components to this panel
	add(titleLabel);
	add(buttonVideo);
	add(buttonAudio);
	add(buttonControls);
	add(buttonBack);
}

/*****************************************************************************
 * The AsteroidsMainMenu class
 *****************************************************************************/

AsteroidsMainMenu::AsteroidsMainMenu(AsteroidsGame* game) {
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

	//Setup the buttons
	GUIComponentRenderer::DEFAULT_FONT = game->getResourceLoader().loadFont("TT1240M_.TTF", 24, Colour::WHITE);

	Texture* normal = game->getResourceLoader().loadTexture("Button.png");
	Texture* hover = game->getResourceLoader().loadTexture("Button_Hover.png");
	Texture* clicked = game->getResourceLoader().loadTexture("Button_Clicked.png");

	//Setup the GUIPanelGroup instance
	panelGroup = new GUIPanelGroup();
	//Add the panels to the panel group
	panelGroup->add("Main", new MainMenuMain(game, panelGroup, { normal, hover, clicked }));
	panelGroup->add("Highscores", new MainMenuHighScores(game, panelGroup, { normal, hover, clicked }));
	panelGroup->add("Settings", new MainMenuSettings(game, panelGroup, { normal, hover, clicked }));
}

AsteroidsMainMenu::~AsteroidsMainMenu() {
	//Delete created resources
	delete camera;
	delete background;
}

void AsteroidsMainMenu::show() {
	//Add the camera
	Renderer::addCamera(camera);

	//Show the panel group
	panelGroup->show("Main");
}

void AsteroidsMainMenu::hide() {
	//Remove the camera
	Renderer::removeCamera();

	//Hide the panel group
	panelGroup->hideCurrent();
}

void AsteroidsMainMenu::update() {
	//Update the panel group
	panelGroup->update();
}

void AsteroidsMainMenu::render() {
	//Setup for rendering
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Render the background
	background->render();

	//Render the panel group
	panelGroup->render();
}
