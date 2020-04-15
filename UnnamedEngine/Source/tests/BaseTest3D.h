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

#include "../core/BaseEngine.h"
#include "../core/ResourceLoader.h"
#include "../core/audio/SoundSystem.h"
#include "../core/physics/PhysicsScene.h"
#include "../core/render/RenderScene.h"
#include "../utils/DebugCamera.h"
#include "../utils/DebugProfiler.h"

class BaseTest3D : public BaseEngine {
protected:
	DebugCamera* camera;
	RenderScene3D* renderScene;
	PhysicsScene3D* physicsScene;
	SoundSystem* soundSystem;
	DebugProfiler profiler;

	ResourceLoader resourceLoader;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;

	virtual void onInitialise() {}
	virtual void onCreated() {}
	virtual void onUpdate() {}
	virtual void onRender() {}
	virtual void onDestroy() {}

	/* Method to start profiling */
	inline void startProfile(float timeSeconds) { profiler.start(timeSeconds, profilerCallback); }

	/* Callback for the profiler */
	static void profilerCallback(DebugProfiler::Result result);

	/* Input methods */
	virtual void onKeyPressed(int key) override;
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

