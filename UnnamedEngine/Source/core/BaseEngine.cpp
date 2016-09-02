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

#include "BaseEngine.h"
#include "render/Renderer.h"
#include "audio/Audio.h"
#include "gui/GUIComponent.h"

/*****************************************************************************
 * The BaseEngine class
 *****************************************************************************/

BaseEngine::BaseEngine() {
	window = new Window();
}

void BaseEngine::create() {
	//Initialise the game, the settings should be set when this is called
	initialise();

	//Setup the input and create the window
	window->getInputManager()->addListener(this);
	window->create();

	//Setup the FPS utilities
	fpsCalculator = new FPSCalculator();
	fpsCalculator->start();
	fpsLimiter = new FPSLimiter(window->getSettings().videoMaxFPS);

	//Initialise the Renderer therefore loading engine shaders and textures
	Renderer::initialise();

	//Initialise the Audio system
	AudioManager::initialise();

	//Initialise the font system
	Font::initialiseFreeType();

	//Assign the default font
	defaultFont = new Font("resources/fonts/CONSOLA.TTF", 16, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));
	//Create the debug camera
	debugCamera = new Camera2D(Matrix4f().initOrthographic(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1));
	debugCamera->update();

	GUIComponentRenderer::DEFAULT_FONT = defaultFont;

//	glScissor(0, 0, getSettings().windowWidth, getSettings().windowHeight);
//	glViewport(0, 0, getSettings().windowWidth, getSettings().windowHeight);

	if (getSettings().engineSplashScreen) {
		Renderer::addCamera(debugCamera);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Font* font = new Font("resources/fonts/SEGOEUIL.TTF", 64, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));
		Font* font2 = new Font("resources/fonts/SEGOEUIL.TTF", 32, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));
		font->render("Unnamed Engine", getSettings().windowWidth / 2 - font->getWidth("Unnamed Engine") / 2, getSettings().windowHeight / 2 - font->getHeight("Unnamed Engine") / 2);
		font2->render(Engine::Build + " " + Engine::Version, getSettings().windowWidth / 2 - font2->getWidth(Engine::Build + " " + Engine::Version) / 2, (getSettings().windowHeight / 2 - font->getHeight("Unnamed Engine") / 2) + 38.0f);
		glDisable(GL_BLEND);
		Renderer::removeCamera();

		glfwSwapBuffers(window->getInstance());
	}

	//Notify the game that everything is ready to start creating game objects
	//for rendering
	created();

	//The main game loop - continues until either the window is told to close,
	//or the game requests it to stop
	while ((! window->shouldClose()) && (! closeRequested)) {
		fpsCalculator->update();

		update();
		render();

		if (getSettings().debuggingShowInformation)
			renderDebuggingInfo();

		window->update();

		fpsLimiter->update(fpsCalculator->getDelta());
	}

	//Tell the game to destroy everything it created
	destroy();

	//Destroy all other engine resources
	Font::destroyFreeType();
	AudioManager::destroy();
	ResourceManager::destroyAll();

	//Delete all of the objects required by the BaseEngine class
	delete debugCamera;
	delete fpsCalculator;
	delete fpsLimiter;

	delete window;
}

using namespace StrUtils;

void BaseEngine::renderDebuggingInfo() {
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Renderer::addCamera(debugCamera);
	defaultFont->render(str("--------- DEBUGGING ---------\n") +
							"Engine Version : " + str(Engine::Version) + "\n" +
							"Engine Date    : " + str(Engine::DateCreated) + "\n" +
							"Engine Build   : " + str(Engine::Build) + "\n" +
							"Current Delta  : " + str(getDelta()) + "\n" +
							"Current FPS    : " + str(getFPS()) + "\n" +
							"----------- VIDEO -----------\n" +
							"Resolution     : " + str(getSettings().videoResolution.getX()) + "x" + str(getSettings().videoResolution.getY()) + "\n" +
							"VSync          : " + str(getSettings().videoVSync) + "\n" +
							"MSAA Samples   : " + str(getSettings().videoSamples) + "\n" +
							"Max AF Samples : " + str(getSettings().videoMaxAnisotropicSamples) + "\n" +
							"----------- AUDIO -----------\n" +
							"Music Volume   : " + str(getSettings().audioMusicVolume) + "\n" +
							"SFX Volume     : " + str(getSettings().audioMusicVolume) + "\n" +
							"-----------------------------"
							, 2, 16);
	Renderer::removeCamera();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
