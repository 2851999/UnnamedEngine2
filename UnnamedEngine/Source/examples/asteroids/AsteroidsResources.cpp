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

#include "AsteroidsResources.h"

/*****************************************************************************
 * The AsteroidsResources class
 *****************************************************************************/

AsteroidsResources::AsteroidsResources() {}

AsteroidsResources::~AsteroidsResources() {}

void AsteroidsResources::setup(ResourceLoader& loader) {
	//Create the resources
	fontTitle = loader.loadFont("TT1240M_.ttf", 64.0f, Colour::WHITE);
	fontGUI = loader.loadFont("TT1240M_.TTF", 24.0f, Colour::WHITE);
	fontHeading = loader.loadFont("TT1240M_.ttf", 24.0f, Colour::WHITE);
	fontHeadingMono = loader.loadFont("CONSOLA.ttf", 24.0f, Colour::WHITE);

	texturesButtons = { loader.loadTexture("Button.png"), loader.loadTexture("Button_Hover.png"), loader.loadTexture("Button_Clicked.png") };

	audioLaser = loader.loadAudio("lasermono.wav");
	audioExplosion = loader.loadAudio("explosion.wav");
}
