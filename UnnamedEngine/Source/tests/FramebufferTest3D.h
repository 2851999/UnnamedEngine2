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

#ifndef BASEENGINETEST3D_H_
#define BASEENGINETEST3D_H_

#include "BaseTest3D.h"

#include "../core/render/PostProcessing.h"
#include "../core/render/Renderer.h"

class Test : public BaseTest3D {
private:
	PostProcessor* processor;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void render() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {

}

void Test::onCreated() {
	TextureParameters::DEFAULT_FILTER = GL_LINEAR_MIPMAP_LINEAR;

	GameObject3D* object = new GameObject3D(resourceLoader.loadModel("teapot.obj"), Renderer::getRenderShader("Material"));
	object->update();

	renderScene->add(object);

	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg", 100.0f));
	camera->setFlying(true);

	processor = new PostProcessor("resources/shaders/postprocessing/GrayScaleShader");
}

void Test::onUpdate() {

}

void Test::render() {
	processor->start();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	camera->useView();

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	renderScene->render();

	processor->stop();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	processor->render();
}

void Test::onDestroy() {
	delete processor;
}

#endif /* UTILS_BASEENGINETEST3D_H_ */
