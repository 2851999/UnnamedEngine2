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

#include "../core/terrain/CDLODTerrain.h"
#include "../experimental/terrain/HeightMapGenerator.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

class Test : public BaseTest3D {
private:
	CDLODTerrain* terrain;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {
	getSettings().videoVSync = false;
	getSettings().videoMaxFPS = 0;
}

void Test::onCreated() {
	camera->setViewMatrix(Matrix4f().initPerspective(110.0f, getSettings().windowAspectRatio, 0.1f, 1000.0f));
	camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg", 100.0f));
	camera->setFlying(true);

	camera->setMovementSpeed(5.0f);

	terrain = new CDLODTerrain(resourceLoader.getAbsPathTextures() + "heightmap.jpg");
//	HeightMapGenerator generator;
//	terrain = new CDLODTerrain(new CDLODHeightMap(generator.generate(256, 256), 1, 256, 256, GL_RED));
//	stbi_write_bmp((resourceLoader.getAbsPathTextures() + "heightmapgen.bmp").c_str(), 256, 256, 1, generator.generate(256, 256));
}

void Test::onUpdate() {

}

void Test::onRender() {
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	terrain->render();
	glDisable(GL_CULL_FACE);
}

void Test::onDestroy() {
	delete terrain;
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
