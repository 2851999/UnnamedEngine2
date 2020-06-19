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
#include "vulkan/Vulkan.h"
#include "vulkan/VulkanValidationLayers.h"
#include "vulkan/VulkanExtensions.h"
#include "../utils/GLUtils.h"
#include "../utils/Logging.h"

/*****************************************************************************
 * The BaseEngine class
 *****************************************************************************/

BaseEngine::BaseEngine() {
	window = new Window();
}

void BaseEngine::create() {
	//Initialise the game, the settings should be set when this is called
	initialise();

	//Setup the default engine resource manager
	ResourceManager::addResourceManager(new ResourceManager());

	//Setup the input and create the window
	window->getInputManager()->addListener(this);
	window->create();

	//Setup the FPS utilities
	fpsCalculator.start();
	fpsLimiter.setMaxFPS(getSettings().videoMaxFPS);

	//Initialise the graphics API and don't continue if it fails
	if (initGraphicsAPI()) {
		//Initialise the Renderer therefore loading engine shaders and textures
		Renderer::initialise();

		//Initialise the font system
		Font::initialiseFreeType();

		//Assign the default font and text instance
		defaultFont = new Font("resources/fonts/CONSOLA.TTF", 16);
		//defaultFont = new Font("resources/fonts/testFont.fnt", 22);
		//defaultFont = new Font("resources/fonts/testFont.fnt", 40);
		textInstance = new Text(defaultFont, Colour::WHITE, 400);
		//Create the debug camera
		debugCamera = new Camera2D(Matrix4f().initOrthographic(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1));
		debugCamera->update();

		GUIComponentRenderer::DEFAULT_FONT = defaultFont;

		//Initialise the Audio system
		AudioManager::initialise();

		//Create the debug console if needed
		if (getSettings().debugConsoleEnabled) {
			debugConsole = new DebugConsole(this);
			debugConsole->enable();
			debugConsole->hide();
		}

		if (! getSettings().videoVulkan) {

		//	glScissor(0, 0, getSettings().windowWidth, getSettings().windowHeight);
		//	glViewport(0, 0, getSettings().windowWidth, getSettings().windowHeight);

			if (getSettings().engineSplashScreen) {
				Renderer::addCamera(debugCamera);

				ResourceManager* manager = new ResourceManager();
				ResourceManager::addResourceManager(manager);

				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				Font* font = new Font("resources/fonts/SEGOEUIL.TTF", 64, TextureParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE).setFilter(TextureParameters::Filter::NEAREST));
				Font* font2 = new Font("resources/fonts/SEGOEUIL.TTF", 32, TextureParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE).setFilter(TextureParameters::Filter::NEAREST));
				textInstance->setFont(font);
				textInstance->setColour(Colour::BLACK);
				textInstance->render("Unnamed Engine", getSettings().windowWidth / 2 - font->getWidth("Unnamed Engine") / 2, getSettings().windowHeight / 2 - font->getHeight("Unnamed Engine") / 2);
				textInstance->setFont(font2);
				textInstance->render(Engine::Build + " " + Engine::Version, getSettings().windowWidth / 2 - font2->getWidth(Engine::Build + " " + Engine::Version) / 2, (getSettings().windowHeight / 2 - font->getHeight("Unnamed Engine") / 2) + 42.0f);

				glDisable(GL_BLEND);
				Renderer::removeCamera();

				delete manager;
				ResourceManager::removeResourceManager();

				glfwSwapBuffers(window->getInstance());

				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

				//Go back to default values for rendering text
				textInstance->setFont(defaultFont);
				textInstance->setColour(Colour::WHITE);
			}
		}

		//Notify the game that everything is ready to start creating game objects
		//for rendering
		created();

		//The main game loop - continues until either the window is told to close,
		//or the game requests it to stop
		while ((! window->shouldClose()) && (! closeRequested)) {
			fpsLimiter.startFrame();
			fpsCalculator.update();

			//Ensure the debug console isn't open
			if (! debugConsole || ! debugConsole->isVisible())
				update();
			else
				debugConsole->update();

			if (getSettings().videoVulkan) {
				//Update Vulkan and begin drawing
				//Start draw first to perform synchronisation necessary to perform updates
				Vulkan::startDraw();
				Vulkan::update();
			}

			renderOffscreen();

			//Start the default RenderPass
			Renderer::getDefaultRenderPass()->begin();
			render();

			if (getSettings().debugShowInformation)
				renderDebugInfo();

			if (getSettings().debugConsoleEnabled)
				renderDebugConsole();

			//Stop the default RenderPass
			Renderer::getDefaultRenderPass()->end();

			if (getSettings().videoVulkan)
				Vulkan::stopDraw();

			window->update();

			//vkDeviceWaitIdle(Vulkan::getDevice()->getLogical());

			fpsLimiter.endFrame();
		}

		//Wait for a suitable time
		if (getSettings().videoVulkan)
			Vulkan::waitDeviceIdle();

		//Tell the game to destroy everything it created
		destroy();

		//Destroy all other engine resources
		Renderer::destroy();
		Font::destroyFreeType();
		delete textInstance;
		delete defaultFont;
		delete debugCamera;

		AudioManager::destroy();

		if (debugConsole)
			delete debugConsole;
		ResourceManager::destroyAllManagers();
	} else
		//Failed to initialise the graphics API
		Logger::log("Failed to initialise graphics API", "BaseEngine", LogType::Error);

	if (getSettings().videoVulkan)
		//Destroy everything used by Vulkan
		Vulkan::destroy();

	delete window;
}

using namespace utils_string;

void BaseEngine::renderDebugInfo() {
	if (! BaseEngine::usingVulkan()) {
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (debugConsole->isWireframeEnabled())
			utils_gl::disableWireframe();
	}

	Renderer::addCamera(debugCamera);

	textInstance->render(str("----------- DEBUG -----------\n") +
							 "Engine Version : " + str(Engine::Version) + "\n" +
							 "Engine Date    : " + str(Engine::DateCreated) + "\n" +
							 "Engine Build   : " + str(Engine::Build) + "\n" +
							 "Current Delta  : " + str((int) getDelta()) + "\n" +
							 "Current FPS    : " + str(getFPS()) + "\n" +
							 "----------- VIDEO -----------\n" +
							 "Resolution     : " + str(getSettings().videoResolution.getX()) + "x" + str(getSettings().videoResolution.getY()) + "\n" +
							 "VSync          : " + str(getSettings().videoVSync) + "\n" +
							 "MSAA Samples   : " + str(getSettings().videoSamples) + "\n" +
							 "Max AF Samples : " + str(getSettings().videoMaxAnisotropicSamples) + "\n" +
							 "Vulkan         : " + str(getSettings().videoVulkan) + "\n" +
							 "----------- AUDIO -----------\n" +
							 "Music Volume   : " + str(getSettings().audioMusicVolume) + "\n" +
							 "SFX Volume     : " + str(getSettings().audioSoundEffectVolume) + "\n" +
							 "-----------------------------"
							 , 2, 16);

	Renderer::removeCamera();

	if (! BaseEngine::usingVulkan()) {
		if (debugConsole->isWireframeEnabled())
			utils_gl::enableWireframe();

//		glDisable(GL_BLEND);
		//glDisable(GL_TEXTURE_2D);
	}
}

void BaseEngine::renderDebugConsole() {
	//Render the debug console if needed
	if (debugConsole->isVisible()) {
		//glEnable(GL_TEXTURE_2D);
		//glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Renderer::addCamera(debugCamera);

		if (debugConsole->isWireframeEnabled())
			utils_gl::disableWireframe();

		debugConsole->render();

		if (debugConsole->isWireframeEnabled())
			utils_gl::enableWireframe();

		Renderer::removeCamera();

		//glDisable(GL_BLEND);
		//glDisable(GL_TEXTURE_2D);
	}
}

bool BaseEngine::initGraphicsAPI() {
	if (getSettings().videoVulkan) {
		//Initialise Vulkan
		return Vulkan::initialise(window);
	} else
		//Initialise OpenGL
		glewInit();

	return true;
}
