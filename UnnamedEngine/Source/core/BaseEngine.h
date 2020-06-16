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

#pragma once

#include <string>

#include "Window.h"
#include "gui/Font.h"
#include "render/Camera.h"
#include "render/RenderPass.h"
#include "../utils/DebugConsole.h"
#include "../utils/FPSUtils.h"

namespace Engine {
	/* Various engine values
	 * Version     - Should change every development version
	 * DateCreated - Should change every development version
	 */
	const std::string Version     = "V0.5.0";
	const std::string DateCreated = "13/06/2020";
	const std::string Build       = "Experimental";
}

/*****************************************************************************
 * The BaseEngine class provides basic engine functionality
 *****************************************************************************/

class BaseEngine : public InputListener {
private:
	/* The main window instance */
	Window* window;
	/* Used to request the game loop to stop */
	bool   closeRequested = false;

	/* Used to keep track of and limit the FPS */
	FPSCalculator fpsCalculator;
	FPSLimiter    fpsLimiter;

	/* The default font */
	Font* defaultFont = NULL;

	/* Default text instance */
	Text* textInstance = NULL;

	/* Debug camera used when rendering debug information */
	Camera2D* debugCamera = NULL;
	/* Debug console */
	DebugConsole* debugConsole = NULL;

	/* Method used to initialise the graphics API being used for rendering (Returns whether initialisation was successful) */
	bool initGraphicsAPI();
public:
	/* The constructor */
	BaseEngine();

	/* The destructor */
	virtual ~BaseEngine() {}

	/* This will create the window and start the game loop */
	void create();

	/* Called before the Window is created so Settings should be assigned
	 * here */
	virtual void initialise() {}
	/* Called after the Window is created all objects that need to be
	 * rendered can be created */
	virtual void created() {}
	/* Update method */
	virtual void update() {}
	/* Method to render before a render pass is started*/
	virtual void renderOffscreen() {}
	/* Method to render after the default framebuffer render pass has been started*/
	virtual void render() {}
	/* Called when the Engine is stopping but before the Window has
	 * closed */
	virtual void destroy() {}

	/* Method called to render debugging information */
	void renderDebugInfo();

	/* Method called to render the debug console */
	void renderDebugConsole();

	/* This will end the main game loop */
	void requestClose() { closeRequested = true; }

	/* Setters and getters */
	Window* getWindow() { return window; }
	Settings& getSettings() { return window->getSettings(); }
	Font* getDefaultFont() { return defaultFont; }
	Camera2D* getDebugCamera() { return debugCamera; }
	DebugConsole* getDebugConsole() { return debugConsole; }
	bool hasDebugConsole() { return debugConsole; }

	unsigned int getFPS() { return fpsCalculator.getFPS(); }
	float getDelta() { return fpsCalculator.getDelta(); }
	float getDeltaSeconds() { return fpsCalculator.getDeltaSeconds(); }

	/* Input methods */
	virtual void onKeyPressed(int key) override {}
	virtual void onKeyReleased(int key) override {}
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override {}
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}

	virtual void onControllerAxis(Controller* controller, int axis, float value) override {}
	virtual void onControllerButtonPressed(Controller* controller, int index) override {}
	virtual void onControllerButtonReleased(Controller* controller, int index) override {}

	/* Static method to obtain a boolean representing whether Vulkan is being used */
	inline static bool usingVulkan() { return Window::getCurrentInstance()->getSettings().videoVulkan; }
};


