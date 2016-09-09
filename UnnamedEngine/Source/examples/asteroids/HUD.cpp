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

	//Setup the health bar
	barPlayerHealth = new GUILoadingBar(400, 20, player->getHealth());
	barPlayerHealth->setPosition(windowWidth - barPlayerHealth->getWidth() - 10, 10);
	barPlayerHealth->setBorder(new GUIBorder(barPlayerHealth, 1, Colour::ORANGE));

	add(barPlayerHealth);
}

HUD::~HUD() {
	//Delete created resources
	delete camera;
}

void HUD::update() {
	GUIPanel::update();
	//Update the health bar
	barPlayerHealth->setCurrentStage(player->getHealth());
}

void HUD::render(bool overrideShader) {
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Add the camera
	Renderer::addCamera(camera);

	//Render the panel
	GUIPanel::render();

	//Remove the camera
	Renderer::removeCamera();
}
