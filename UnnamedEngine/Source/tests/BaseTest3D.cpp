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

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"
#include "../utils/GLUtils.h"

void BaseTest3D::initialise() {
	resourceLoader.setPath("C:/UnnamedEngine/");
	resourceLoader.setPathTextures("textures/");
	resourceLoader.setPathAudio("audio/");
	resourceLoader.setPathModels("models/");

	Settings& settings = getSettings();
	settings.windowTitle = "Test";
	settings.videoMaxAnisotropicSamples = 16;
	settings.videoSamples = 16;
	settings.videoVSync = true;
	settings.videoMaxFPS = 0;

	onInitialise();
}

void BaseTest3D::created() {
	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;

	InputBindings* bindings = new InputBindings();
	bindings->load(resourceLoader.getPath() + "config/Controller.xml", getWindow()->getInputManager());

	camera = new DebugCamera(80.0f, getSettings().windowAspectRatio, 0.1f, 100.0f, bindings);

	physicsScene = new PhysicsScene3D();
	renderScene = new RenderScene3D();

	soundSystem = new SoundSystem();
	soundSystem->createListener(camera);

	Renderer::addCamera(camera);

	getWindow()->disableCursor();

	onCreated();
}

void BaseTest3D::update() {
	onUpdate();

	camera->update(getDeltaSeconds());
	physicsScene->update(getDeltaSeconds());

	soundSystem->update();
}

void BaseTest3D::render() {
	utils_gl::setupSimple3DView(true);

	renderScene->render();

	camera->useView(); //In case of deferred rendering forward rendered objects should be rendered after the deferred

	onRender();
}

void BaseTest3D::destroy() {
	onDestroy();

	delete camera;
	delete physicsScene;
	delete renderScene;
}

void BaseTest3D::onKeyPressed(int key) {
	if (key == GLFW_KEY_ESCAPE)
		requestClose();
}
