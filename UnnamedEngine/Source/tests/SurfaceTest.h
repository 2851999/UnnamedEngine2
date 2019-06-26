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

#ifndef TESTS_BASEENGINETEST3D_H_
#define TESTS_BASEENGINETEST3D_H_

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"
#include "../utils/GLUtils.h"
#include "../experimental/Surface.h"

class Test : public BaseTest3D {
private:
	RenderScene3D* scene;
	Surface* surface;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {
	getSettings().videoVSync = true;
	//getSettings().videoMaxFPS = 0;
	//getSettings().windowFullscreen = true;
	//getSettings().videoResolution = VideoResolution::RES_1920x1080;
}

void Test::onCreated() {
	camera->setProjectionMatrix(Matrix4f().initPerspective(80.0f, getSettings().windowAspectRatio, 0.01f, 1000.0f));
	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg"));
	camera->setFlying(true);

	camera->setMovementSpeed(20.0f);

	surface = new Surface(100);
	surface->setScale(10.0f, 1.0f, 10.0f);
	surface->update();

	scene = new RenderScene3D();
	scene->add(surface);
	scene->enableWireframe();

	Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(1.0f, -1.0f, 0.0001f);
	light0->update();
	scene->addLight(light0);
}


void Test::onUpdate() {
	surface->updateHeights(utils_time::getSeconds());
}

void Test::onRender() {
	//renderScene->showDeferredBuffers(); //For deferred rendering need to disable MSAA for this to work
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
//	glCullFace(GL_BACK);
	scene->render();
//	glDisable(GL_CULL_FACE);
}

void Test::onDestroy() {
	delete scene;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
