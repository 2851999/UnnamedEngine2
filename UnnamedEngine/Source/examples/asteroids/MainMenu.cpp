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

#include "MainMenu.h"
#include "Asteroids.h"

#include "../../core/Object.h"

MainMenu::MainMenu(float windowWidth, float windowHeight) {
	font = new Font("resources/fonts/TT1240M_.TTF", 64, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));
	font->update("Asteroids", windowWidth / 2 - font->getWidth("Asteroids") / 2, 80);

	GUIComponentRenderer::DEFAULT_FONT = new Font("resources/fonts/TT1240M_.TTF", 24, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));

	Texture* normal = Texture::loadTexture("C:/UnnamedEngine/examples/asteroids/MenuButton.png");
	Texture* hover = Texture::loadTexture("C:/UnnamedEngine/examples/asteroids/MenuButton_Hover.png");
	Texture* clicked = Texture::loadTexture("C:/UnnamedEngine/examples/asteroids/MenuButton_Clicked.png");

	buttonPlay = new GUIButton("Play", 300, 30, { normal, hover, clicked });
	buttonPlay->setPosition(windowWidth / 2 - buttonPlay->getWidth() / 2, 140);

	Texture* backgroundTexture = Texture::loadTexture("C:/UnnamedEngine/examples/asteroids/MenuBackground.png");
	background = new GameObject2D(new Mesh(MeshBuilder::createQuad(windowWidth, windowHeight, backgroundTexture)), Renderer::getRenderShader("Material"));
	background->getMaterial()->setDiffuseTexture(backgroundTexture);
	background->update();
}

MainMenu::~MainMenu() {
	delete buttonPlay;
}

void MainMenu::update(Asteroids* asteroids) {
	buttonPlay->update();

	if (buttonPlay->isClicked())
		asteroids->startGame();
}

void MainMenu::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	background->render();
	font->render();
	buttonPlay->render();
}
