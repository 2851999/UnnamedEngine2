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

#ifndef CORE_BASEENGINE_H_
#define CORE_BASEENGINE_H_

#include <string>

#include "Window.h"
#include "../utils/FPSUtils.h"
#include "gui/Font.h"
#include "render/Camera.h"

namespace Engine {
	/* Various engine values
	 * Version     - Should change every development version
	 * DateCreated - Should change every development version
	 */
	const std::string Version     = "V0.0.4";
	const std::string DateCreated = "07/09/2016";
	const std::string Build       = "Experimental";
}

/*****************************************************************************
 * The BaseEngine class provides basic engine functionality
 *****************************************************************************/

class BaseEngine : public InputListener {
private:
	Window* window;
	bool   closeRequested = false;

	FPSCalculator* fpsCalculator = NULL;
	FPSLimiter*    fpsLimiter = NULL;

	/* The default font */
	Font* defaultFont = NULL;

	/* Debug camera used when rendering debug information */
	Camera2D* debugCamera = NULL;

	void engineKeyCallback(int key, int scancode, int action, int mods);
	void engineCharCallback(unsigned int codepoint);
	void engineCursorPosCallback(double xpos, double ypos);
	void engineCursorEnterCallback(int entered);
	void engineMouseButtonCallback(int button, int action, int mods);
	void engineScrollCallback(double xoffset, double yoffset);
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
	/* Update and render methods */
	virtual void update() {}
	virtual void render() {}
	/* Called when the Engine is stopping but before the Window has
	 * closed */
	virtual void destroy() {}

	/* Method called to render debugging information */
	void renderDebuggingInfo();

	/* This will end the main game loop */
	void requestClose() { closeRequested = true; }

	/* Setters and getters */
	Window* getWindow() { return window; }
	Settings& getSettings() { return window->getSettings(); }
	Font* getDefaultFont() { return defaultFont; }

	unsigned int getFPS() { return fpsCalculator->getFPS(); }
	long getDelta() { return fpsCalculator->getDelta(); }
	float getDeltaSeconds() { return fpsCalculator->getDeltaSeconds(); }

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
};


#endif /* CORE_BASEENGINE_H_ */
