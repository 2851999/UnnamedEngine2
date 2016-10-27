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

#include "HUD.h"
#include "Player.h"

/*****************************************************************************
 * The HUD class
 *****************************************************************************/

HUD::HUD(AsteroidsGame* game, Player* player) : player(player) {
	//The window width/height
	float windowWidth = game->getSettings().windowWidth;
	float windowHeight = game->getSettings().windowHeight;

	//Setup the camera
	camera = new Camera2D(Matrix4f().initOrthographic(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f));
	camera->update();

	//Get the heading font
	Font* headingFont = game->getResources().getFontHeading();

	//Setup the health bar
	barPlayerHealth = new GUILoadingBar(400, 20, player->getHealth(), game->getResources().getTexturesButtons()[0], game->getResources().getTexturesButtons()[1]);
	barPlayerHealth->setPosition(windowWidth - barPlayerHealth->getWidth() - 10, 40.0f);

	//Setup the health label
	GUILabel* labelHealth = new GUILabel("Health:", headingFont);
	labelHealth->setPosition(barPlayerHealth->getPosition().getX() - labelHealth->getWidth() - 10.0f, 40.0f);

	//Setup the shield bar
	barPlayerShield = new GUILoadingBar(400, 20, player->getShield(), game->getResources().getTextureShieldBar(), game->getResources().getTextureShieldBar());
	barPlayerShield->setPosition(windowWidth - barPlayerShield->getWidth() - 10, 70.0f);

	//Setup the shield label
	GUILabel* labelShield = new GUILabel("Shield:", headingFont);
	labelShield->setPosition(barPlayerShield->getPosition().getX() - labelShield->getWidth() - 10.0f, 70.0f);

	//Setup the score label
	labelScore = new GUILabel("Score: ", headingFont);
	labelScore->setPosition(barPlayerHealth->getPosition().getX(), 10.0f);

	//Add the components
	add(barPlayerHealth);
	add(labelHealth);
	add(barPlayerShield);
	add(labelShield);
	add(labelScore);

	//Create the cross hair
	crossHair = new GameObject2D(new Mesh(MeshBuilder::createQuad(8, 8, game->getResources().getTextureCrossHair())), "Material");
	crossHair->getMaterial()->setDiffuseTexture(game->getResources().getTextureCrossHair());
	crossHair->setPosition(windowWidth / 2 - crossHair->getWidth() / 2, windowHeight / 2 - crossHair->getHeight() / 2);
	crossHair->update();
}

HUD::~HUD() {
	//Delete created resources
	delete crossHair;
	delete camera;
}

void HUD::update() {
	GUIPanel::update();
	//Update the health bar if necessary
	if (barPlayerHealth->getCurrentStage() != player->getHealth())
		barPlayerHealth->setCurrentStage(player->getHealth());
	if (barPlayerShield->getCurrentStage() != player->getShield())
		barPlayerShield->setCurrentStage(player->getShield());
	labelScore->setText("Score: " + StrUtils::str(player->getScore()));
}

void HUD::render() {
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Add the camera
	Renderer::addCamera(camera);

	//Render the panel
	GUIPanel::render();

	//Render the cross hair
	crossHair->render();

	//Remove the camera
	Renderer::removeCamera();
}
