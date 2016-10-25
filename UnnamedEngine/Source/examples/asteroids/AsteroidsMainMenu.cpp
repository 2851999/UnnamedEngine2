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

MainMenuMain::MainMenuMain(AsteroidsGame* game, GUIPanelGroup* panelGroup) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Get the button textures
	std::vector<Texture*> texturesButtons = game->getResources().getTexturesButtons();

	//Create the buttons
	buttonPlay = new GUIButton("Play", 400, 30, texturesButtons);
	buttonPlay->setPosition(windowWidth / 2 - buttonPlay->getWidth() / 2, 140);

	buttonHighscores = new GUIButton("Highscores", 400, 30, texturesButtons);
	buttonHighscores->setPosition(windowWidth / 2 - buttonHighscores->getWidth() / 2, 180);
	panelGroup->assignButton(buttonHighscores, "Highscores");

	buttonSettings = new GUIButton("Settings", 400, 30, texturesButtons);
	buttonSettings->setPosition(windowWidth / 2 - buttonSettings->getWidth() / 2, 220);
	panelGroup->assignButton(buttonSettings, "Settings");

	buttonExit = new GUIButton("Exit", 400, 30, texturesButtons);
	buttonExit->setPosition(windowWidth / 2 - buttonExit->getWidth() / 2, windowHeight - 50);

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

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

MainMenuHighScores::MainMenuHighScores(AsteroidsGame* game, GUIPanelGroup* panelGroup) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Get the button textures
	std::vector<Texture*> texturesButtons = game->getResources().getTexturesButtons();

	buttonBack = new GUIButton("Back", 400, 30, texturesButtons);
	buttonBack->setPosition(windowWidth / 2 - buttonBack->getWidth() / 2, windowHeight - 50);
	panelGroup->assignButton(buttonBack, "Main");

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Highscores", titleFont);
	titleLabel->setPosition(game->getSettings().windowWidth / 2 - titleFont->getWidth("Highscores") / 2, 40.0f);

	//Get the header font
	Font* headerFont = game->getResources().getFontHeadingMono();

	//Create the highscores label
	highscoresLabel = new GUILabel("", headerFont);

	//Add the components to this panel
	add(titleLabel);
	add(highscoresLabel);
	add(buttonBack);
}

void MainMenuHighScores::show() {
	//The text to display
	std::string text;

	//The maximum name length
	unsigned int maxLength = 16;

	//The length of the current/longest names
	unsigned int currentNameLength = 0;

	//Get a referene to the highscores
	Highscores& highscores = game->getHighscores();

	//Go through the highscores
	for (unsigned int i = 0; i < highscores.getNumHighscores(); i++) {
		//Add onto the text
		text += StrUtils::str(i + 1) + ". ";
		//Get the current name's length
		currentNameLength = highscores.getName(i).length();
		//Check the length
		if (currentNameLength > maxLength - 3)
			//Add only the first part of the name
			text += highscores.getName(i).substr(0, maxLength - 3) + "...";
		else {
			//Add on the name
			text += highscores.getName(i);
			//Add some spaces
			for (unsigned int j = 0; j < maxLength - currentNameLength; j++)
				text += " ";
		}
		//Add the score
		text += "    " + StrUtils::str(highscores.getScore(i)) + "\n";
	}

	highscoresLabel->setText(text);
	highscoresLabel->setPosition(game->getSettings().windowWidth / 2 - highscoresLabel->getWidth() / 2, 160.0f);

	GUIPanel::show();
}

/*****************************************************************************
 * The MainMenuSettings class
 *****************************************************************************/

MainMenuSettings::MainMenuSettings(AsteroidsGame* game, GUIPanelGroup* panelGroup) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Get the button textures
	std::vector<Texture*> texturesButtons = game->getResources().getTexturesButtons();

	//Create the buttons
	buttonVideo = new GUIButton("Video", 400, 30, texturesButtons);
	buttonVideo->setPosition(windowWidth / 2 - buttonVideo->getWidth() / 2, 140);
	panelGroup->assignButton(buttonVideo, "SettingsVideo");

	buttonAudio = new GUIButton("Audio", 400, 30, texturesButtons);
	buttonAudio->setPosition(windowWidth / 2 - buttonAudio->getWidth() / 2, 180);

	buttonControls = new GUIButton("Controls", 400, 30, texturesButtons);
	buttonControls->setPosition(windowWidth / 2 - buttonControls->getWidth() / 2, 220);
	panelGroup->assignButton(buttonControls, "SettingsControls");

	buttonBack = new GUIButton("Back", 400, 30, texturesButtons);
	buttonBack->setPosition(windowWidth / 2 - buttonBack->getWidth() / 2, windowHeight - 50);
	panelGroup->assignButton(buttonBack, "Main");

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

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
 * The MainMenuSettingsVideo class
 *****************************************************************************/

MainMenuSettingsVideo::MainMenuSettingsVideo(AsteroidsGame* game, GUIPanelGroup* panelGroup) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Video", titleFont);
	titleLabel->setPosition(windowWidth / 2 - titleFont->getWidth("Video") / 2, 40.0f);

	//Get the heading font
	Font* headingFont = game->getResources().getFontHeading();

	//Setup the resolution header
	GUILabel* headerResolution = new GUILabel("Resolution", headingFont);
	headerResolution->setPosition(windowWidth / 2 - headerResolution->getWidth() / 2, 110.0f);

	//Get the button textures
	std::vector<Texture*> texturesButtons = game->getResources().getTexturesButtons();

	//Setup the resolutions drop down list
	GUIButton* dropDownListButton = new GUIButton(VideoResolution::toString(game->getSettings().videoResolution), 400, 30, texturesButtons);
	dropDownListResolutions = new GUIDropDownList(dropDownListButton, game->getResourceLoader().loadTexture("DropDownOverlayClosed.png"), game->getResourceLoader().loadTexture("DropDownOverlayOpened.png"));
	dropDownListResolutions->addButton(new GUIButton("800 x 600", 400, 30, texturesButtons));
	dropDownListResolutions->addButton(new GUIButton("1280 x 720", 400, 30, texturesButtons));
	dropDownListResolutions->addButton(new GUIButton("1920 x 1080", 400, 30, texturesButtons));
	dropDownListResolutions->setPosition(windowWidth / 2 - dropDownListResolutions->getWidth() / 2, 140.0f);

	//Setup the checkboxes
	checkBoxFullscreen = new GUICheckBox("Fullscreen", 20, 20, texturesButtons);
	checkBoxFullscreen->setPosition(dropDownListResolutions->getPosition().getX() + checkBoxFullscreen->getFont()->getWidth("Fullscreen") * 1.1f, 180.0f);

	checkBoxBorderless = new GUICheckBox("Borderless", 20, 20, texturesButtons);
	checkBoxBorderless->setPosition(dropDownListResolutions->getPosition().getX() + dropDownListResolutions->getWidth() - checkBoxBorderless->getWidth(), 180.0f);

	checkBoxVSync = new GUICheckBox("VSync", 20, 20, texturesButtons);
	checkBoxVSync->setPosition(dropDownListResolutions->getPosition().getX() + checkBoxFullscreen->getFont()->getWidth("Fullscreen") * 1.1f, 210.0f);

	//Setup the notice label
	GUILabel* noticeLabel = new GUILabel("Note: Changes require restart", headingFont);
	noticeLabel->setPosition(windowWidth / 2 - noticeLabel->getWidth() / 2, windowHeight - 90);

	//Setup the buttons
	buttonApply = new GUIButton("Apply", 195, 30, texturesButtons);
	buttonApply->setPosition(windowWidth / 2 - 200, windowHeight - 50);
	buttonApply->addListener(this);

	buttonBack = new GUIButton("Back", 195, 30, texturesButtons);
	buttonBack->setPosition(windowWidth / 2 + 5, windowHeight - 50);
	buttonBack->addListener(this);
	panelGroup->assignButton(buttonBack, "Settings");

	//Add the components to this panel
	add(titleLabel);
	add(headerResolution);
	add(checkBoxFullscreen);
	add(checkBoxBorderless);
	add(checkBoxVSync);
	add(dropDownListResolutions);
	add(noticeLabel);
	add(buttonApply);
	add(buttonBack);

	enable();
}

void MainMenuSettingsVideo::show() {
	GUIPanel::show();

	dropDownListResolutions->setSelection(VideoResolution::toString(game->getSettings().videoResolution));
	checkBoxFullscreen->setChecked(game->getSettings().windowFullscreen);
	checkBoxBorderless->setChecked(game->getSettings().windowBorderless);
	checkBoxVSync->setChecked(game->getSettings().videoVSync);
}

void MainMenuSettingsVideo::onComponentClicked(GUIComponent* component) {
	//Check which component was clicked
	if (component == buttonApply) {
		//Assign the resolution
		game->getSettings().videoResolution = VideoResolution::toVector(dropDownListResolutions->getSelection());
		game->getSettings().windowFullscreen = checkBoxFullscreen->isChecked();
		game->getSettings().windowBorderless = checkBoxBorderless->isChecked();
		game->getSettings().videoVSync = checkBoxVSync->isChecked();

		//Save the new settings
		SettingsUtils::writeToFile(game->getResourceLoader().getPath() + "settings/settings.txt", game->getSettings());
	}
}

/*****************************************************************************
 * The MainMenuSettingsControls class
 *****************************************************************************/

MainMenuSettingsControls::MainMenuSettingsControls(AsteroidsGame* game, GUIPanelGroup* panelGroup) : game(game) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Create the title label
	GUILabel* titleLabel = new GUILabel("Controls", titleFont);
	titleLabel->setPosition(windowWidth / 2 - titleLabel->getWidth() / 2, 40.0f);

	//Get the heading font
	Font* headingFont = game->getResources().getFontHeading();

	//Setup the headers
	GUILabel* headerForwardAxis = new GUILabel("Forward Axis", headingFont);
	headerForwardAxis->setPosition(windowWidth / 2 - headerForwardAxis->getWidth() / 2, 110.0f);

	GUILabel* headerLookXAxis = new GUILabel("Look X Axis", headingFont);
	headerLookXAxis->setPosition(windowWidth / 2 - headerLookXAxis->getWidth() / 2, 190.0f);

	GUILabel* headerLookYAxis = new GUILabel("Look Y Axis", headingFont);
	headerLookYAxis->setPosition(windowWidth / 2 - headerLookYAxis->getWidth() / 2, 270.0f);

	GUILabel* headerShootButton = new GUILabel("Shoot Button", headingFont);
	headerShootButton->setPosition(windowWidth / 2 - headerShootButton->getWidth() / 2, 350.0f);

	//Get the button textures
	std::vector<Texture*> texturesButtons = game->getResources().getTexturesButtons();

	//Setup the buttons
	buttonForwardPos = new GUIButton("", 200, 30, texturesButtons);
	buttonForwardPos->setPosition(windowWidth / 2 - 210, 140);

	buttonForwardNeg = new GUIButton("", 200, 30, texturesButtons);
	buttonForwardNeg->setPosition(windowWidth / 2 + 10, 140);

	buttonLookXPos = new GUIButton("", 200, 30, texturesButtons);
	buttonLookXPos->setPosition(windowWidth / 2 - 210, 220);

	buttonLookXNeg = new GUIButton("", 200, 30, texturesButtons);
	buttonLookXNeg->setPosition(windowWidth / 2 + 10, 220);

	buttonLookYPos = new GUIButton("", 200, 30, texturesButtons);
	buttonLookYPos->setPosition(windowWidth / 2 - 210, 300);

	buttonLookYNeg = new GUIButton("", 200, 30, texturesButtons);
	buttonLookYNeg->setPosition(windowWidth / 2 + 10, 300);

	buttonShoot = new GUIButton("", 200, 30, texturesButtons);
	buttonShoot->setPosition(windowWidth / 2 + - buttonShoot->getWidth() / 2, 380);

	buttonApply = new GUIButton("Apply", 195, 30, texturesButtons);
	buttonApply->setPosition(windowWidth / 2 - 200, windowHeight - 50);
	buttonApply->addListener(this);

	buttonBack = new GUIButton("Back", 195, 30, texturesButtons);
	buttonBack->setPosition(windowWidth / 2 + 5, windowHeight - 50);
	buttonBack->addListener(this);
	panelGroup->assignButton(buttonBack, "Settings");

	//Add the components to this panel
	add(titleLabel);
	add(headerForwardAxis);
	add(headerLookXAxis);
	add(headerLookYAxis);
	add(headerShootButton);
	add(buttonForwardPos);
	add(buttonForwardNeg);
	add(buttonLookXPos);
	add(buttonLookXNeg);
	add(buttonLookYPos);
	add(buttonLookYNeg);
	add(buttonShoot);
	add(buttonApply);
	add(buttonBack);

	enable();
}

void MainMenuSettingsControls::show() {
	GUIPanel::show();

	//Setup the buttons
	InputBindingAxis* forwardAxis = game->getInputBindings()->getAxisBinding("Forward");
	buttonForwardPos->setText("Key: '" + StrUtils::str((char) forwardAxis->getKeyboardKeyPos()) + "'");
	buttonForwardNeg->setText("Key: '" + StrUtils::str((char) forwardAxis->getKeyboardKeyNeg()) + "'");

//	InputBindingAxis* lookXAxis = game->getInputBindings()->getAxisBinding("LookX");
//	buttonLookXPos->setText("Key: " + StrUtils::str((char) lookXAxis->getKeyboardKeyPos()));
//	buttonLookXNeg->setText("Key: " + StrUtils::str((char) lookXAxis->getKeyboardKeyNeg()));

//	InputBindingAxis* lookYAxis = game->getInputBindings()->getAxisBinding("LookY");
//	buttonLookYPos->setText("Key: " + StrUtils::str((char) lookYAxis->getKeyboardKeyPos()));
//	buttonLookYNeg->setText("Key: " + StrUtils::str((char) lookYAxis->getKeyboardKeyNeg()));

	InputBindingButton* shootButton = game->getInputBindings()->getButtonBinding("Shoot");
	buttonShoot->setText("Key: '" + StrUtils::str((char) shootButton->getKeyboardKey()) + "'");
}

void MainMenuSettingsControls::onComponentClicked(GUIComponent* component) {
	//Check which component was clicked
	if (component == buttonApply) {
		//
	}
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
	background = new GameObject2D({ new Mesh(MeshBuilder::createQuad(windowWidth, windowHeight, backgroundTexture)) }, "Material");
	background->getMaterial()->setDiffuseTexture(backgroundTexture);
	background->update();

	//Setup the buttons
	GUIComponentRenderer::DEFAULT_FONT = game->getResources().getFontGUI();

	//Setup the GUIPanelGroup instance
	panelGroup = new GUIPanelGroup();
	//Add the panels to the panel group
	panelGroup->add("Main", new MainMenuMain(game, panelGroup));
	panelGroup->add("Highscores", new MainMenuHighScores(game, panelGroup));
	panelGroup->add("Settings", new MainMenuSettings(game, panelGroup));
	panelGroup->add("SettingsVideo", new MainMenuSettingsVideo(game, panelGroup));
	panelGroup->add("SettingsControls", new MainMenuSettingsControls(game, panelGroup));
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
