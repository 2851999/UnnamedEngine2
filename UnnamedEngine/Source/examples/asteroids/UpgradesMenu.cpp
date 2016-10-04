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

#include "UpgradesMenu.h"
#include "AsteroidsGame.h"
#include "Player.h"

/*****************************************************************************
 * The UpgradesMenu class
 *****************************************************************************/

using namespace StrUtils;

UpgradesMenu::UpgradesMenu(AsteroidsGame* game, AsteroidsMainGame* mainGame) : game(game), mainGame(mainGame) {
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

	buttonUpgradeFireSpeed = new GUIButton("Upgrade", 280, 30, std::vector<Colour> { Colour::WHITE }, game->getResources().getTexturesButtons());
	buttonUpgradeFireSpeed->setPosition(windowWidth / 2 - buttonUpgradeFireSpeed->getWidth() / 2, 220.0f);

	buttonUpgradeMovementSpeed = new GUIButton("Upgrade", 280, 30, std::vector<Colour> { Colour::WHITE }, game->getResources().getTexturesButtons());
	buttonUpgradeMovementSpeed->setPosition(windowWidth / 2 - buttonUpgradeMovementSpeed->getWidth() / 2, 380.0f);

	buttonBack = new GUIButton("Back", 400, 30, game->getResources().getTexturesButtons());
	buttonBack->setPosition(windowWidth / 2 - buttonBack->getWidth() / 2, windowHeight - 50.0f);

	//Get the title font
	Font* titleFont = game->getResources().getFontTitle();

	//Get the heading font
	Font* headingFont = game->getResources().getFontHeading();

	//Create the label's
	labelTitle = new GUILabel("Upgrades", titleFont);
	labelTitle->setPosition(windowWidth / 2 - labelTitle->getWidth() / 2, 40.0f);

	labelFireSpeed = new GUILabel("", headingFont);
	labelMovementSpeed = new GUILabel("", headingFont);

	//Add the components to this panel
	add(labelTitle);
	add(labelFireSpeed);
	add(buttonUpgradeFireSpeed);
	add(labelMovementSpeed);
	add(buttonUpgradeMovementSpeed);
	add(buttonBack);
}

UpgradesMenu::~UpgradesMenu() {
	//Delete created resources
	delete camera;
	delete background;
}

void UpgradesMenu::show() {
	game->getWindow()->enableCursor();
	updateButtons();
	GUIPanel::show();
}

void UpgradesMenu::hide() {
	game->getWindow()->disableCursor();
	GUIPanel::hide();
}

void UpgradesMenu::render() {
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

void UpgradesMenu::updateButtons() {
	//Disable/Activate the buttons as required
	buttonUpgradeFireSpeed->setActive(mainGame->getPlayer()->getLasers()->getCooldown() > 0.5f && mainGame->getPlayer()->getScore() > 2000);
	if (buttonUpgradeFireSpeed->isActive())
		buttonUpgradeFireSpeed->setColour(Colour::WHITE);
	else
		buttonUpgradeFireSpeed->setColour(Colour(0.6f, 0.6f, 0.6f, 1.0f));
	buttonUpgradeMovementSpeed->setActive(mainGame->getPlayer()->getMaximumSpeed() < 20.0f && mainGame->getPlayer()->getScore() > 2000);
	if (buttonUpgradeMovementSpeed->isActive())
		buttonUpgradeMovementSpeed->setColour(Colour::WHITE);
	else
		buttonUpgradeMovementSpeed->setColour(Colour(0.6f, 0.6f, 0.6f, 1.0f));

	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	//float windowHeight = game->getSettings().windowHeight;
	//Update the labels as required
	labelFireSpeed->setText(
			str("Fire Cooldown\n") +
			str("Current: ") + str(mainGame->getPlayer()->getLasers()->getCooldown()) + str("  ") +
			str("Next: ") + str(mainGame->getPlayer()->getLasers()->getCooldown() - 0.5f) + str("\n") +
			str("Cost: 2000"));
	labelFireSpeed->setPosition(windowWidth / 2 - labelFireSpeed->getWidth() / 2, 140.0f);

	labelMovementSpeed->setText(
			str("Movement Speed\n") +
			str("Current: ") + str(mainGame->getPlayer()->getMaximumSpeed()) + str("  ") +
			str("Next: ") + str(mainGame->getPlayer()->getMaximumSpeed() + 2.0f) + str("\n") +
			str("Cost: 2000"));
	labelMovementSpeed->setPosition(windowWidth / 2 - labelMovementSpeed->getWidth() / 2, 300.0f);
}

void UpgradesMenu::onComponentClicked(GUIComponent* component) {
	if (mainGame->showingUpgrades()) {
		if (component == buttonBack)
			mainGame->hideUpgrades();
		else if (component == buttonUpgradeMovementSpeed) {
			mainGame->getPlayer()->removeScore(2000);
			mainGame->getPlayer()->setMaximumSpeed(mainGame->getPlayer()->getMaximumSpeed() + 2);
			updateButtons();
		} else if (component == buttonUpgradeFireSpeed) {
			mainGame->getPlayer()->removeScore(2000);
			mainGame->getPlayer()->getLasers()->setCooldown(mainGame->getPlayer()->getLasers()->getCooldown() - 0.5f);
			updateButtons();
		}
	}
}
