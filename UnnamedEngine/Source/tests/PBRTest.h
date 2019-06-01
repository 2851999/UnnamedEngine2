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
#include "../core/render/pbr/PBREnvironment.h"

class Test : public BaseTest3D {
private:
//	Texture* albedo;
//	Texture* normal;
//	Texture* metallic;
//	Texture* roughness;
//	Texture* ao;

	RenderShader* pbrRenderShader;
	PBREnvironment* environment;

	Light* light0;

	GameObject3D* mit1;

	bool deferred = true;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;

	virtual void onKeyPressed(int key) override;
};

void Test::onInitialise() {
	getSettings().videoVSync = false;
	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = deferred ? 0 : 16;
	getSettings().videoResolution = VideoResolution::RES_1440p;
	getSettings().videoRefreshRate = 144;
	getSettings().windowFullscreen = true;

	Logger::startFileOutput("C:/UnnamedEngine/logs.txt");
}

void Test::onCreated() {
	Logger::stopFileOutput();

//	GLint num;
//	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &num);
//	std::cout << num << std::endl;

//	GLint num;
//	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &num);
//	std::cout << num << std::endl;
	MeshLoader::loadDiffuseTexturesAsSRGB = true;

	//MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "SimpleSphere.obj");

	environment = PBREnvironment::loadAndGenerate(resourceLoader.getAbsPathTextures() + "PBR/Milkyway_small.hdr"); //Milkyway_small
	//EquiToCube::generateCubemapAndIrradiance(resourceLoader.getAbsPathTextures() + "PBR/Theatre-Center_2k.hdr", envMap, irMap, prefilMap, brdfLUTMap);

	camera->setSkyBox(new SkyBox(environment->getEnvironmentCubemap()));
	camera->setFlying(true);

	pbrRenderShader = Renderer::getRenderShader(Renderer::SHADER_PBR_LIGHTING);
	renderScene->enablePBR();
	renderScene->setPBREnvironment(environment);
	if (deferred)
		renderScene->enableDeferred(); //Should be enabled after PBR so the correct buffers are setup

	light0 = (new Light(Light::TYPE_POINT, Vector3f(0.5f, 2.0f, 2.0f), true))->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));

	//camera->setProjectionMatrix(light0->getLightProjectionMatrix());

	//Light* light1 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, -1.0f, 0.0001f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f));
	//light1->update();
	renderScene->addLight(light0);
//	renderScene->addLight(light1);
//	renderScene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));
//	renderScene->addLight((new Light(Light::TYPE_SPOT, Vector3f(0.0f, 3.0f, 0.0f), false))->setDirection(0.0f, -1.0f, 0.0f)->setInnerCutoff(25.0f)->setOuterCutoff(35.0f)->setDiffuseColour(Colour(23.47f, 21.31f, 20.79f))); //Sphere appears off-centre
//	renderScene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));

	//std::string path = "C:/UnnamedEngine/textures/PBR/";

//	GameObject3D* plane = new GameObject3D(resourceLoader.loadModel("plane/", "plane2.model"), pbrRenderShader);
//	plane->setPosition(0.0f, -2.0f, 0.0f);
//	plane->update();
//	renderScene->add(plane);

	for (int i = 0; i < 16; i++) {
		GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj"), pbrRenderShader);
		Material* material = sphere->getMesh()->getMaterial(1);

		int x = i % 4;
		int y = (int) (i / 4.0f);

		sphere->setPosition(x * 2, y * 2, -0.5f);

		material->setAlbedo(Colour(0.5f, 0.0f, 0.0f));
		material->setMetalness(x * (1.0f / 3.0f));
		material->setRoughness(utils_maths::clamp(y * (1.0f / 3.0f), 0.05f, 1.0f));

		sphere->update();

		renderScene->add(sphere);
	}

	//resourceLoader.loadModel("", "SimpleSphere.model") //Normals not smooth????
	//MeshLoader::loadModel("resources/objects/", "plain_sphere.model")

	//GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("SimpleSphere/", "SimpleSphere.obj"), pbrRenderShader);
	GameObject3D* sphere = new GameObject3D(resourceLoader.loadPBRModel("crytek-sponza/", "sponza.obj"), pbrRenderShader);
	sphere->setScale(0.15f, 0.15f, 0.15f);
	sphere->update();
	renderScene->add(sphere);

	GameObject3D* sphere2 = new GameObject3D(resourceLoader.loadPBRModel("SimpleSphere/", "plane.obj"), pbrRenderShader);
	sphere2->setPosition(10.0f, 0.8f, 0.0f);
	sphere2->update();
	renderScene->add(sphere2);

	//mitsuba-sphere.obj
	mit1 = new GameObject3D(resourceLoader.loadPBRModel("Sphere-Bot Basic/", "bot.dae"), pbrRenderShader);
	mit1->getMesh()->getSkeleton()->startAnimation("");

	//std::cout << mit1->getMesh()->getMaterial(2)->diffuseTexture->getPath() << std::endl;

	mit1->getMesh()->getMaterial(2)->shininessTexture = Texture::loadTexture(resourceLoader.getAbsPathModels() + "Sphere-Bot Basic/Sphere_Bot_rough.jpg");
	mit1->getMesh()->getMaterial(2)->normalMap = Texture::loadTexture(resourceLoader.getAbsPathModels() + "Sphere-Bot Basic/Sphere_Bot_nmap_1.jpg");

	//mit1->setScale(0.5f, 0.5f, 0.5f);
	mit1->setPosition(10.0f, 1.0f, 0.0f);
	mit1->update();
	renderScene->add(mit1);

//	GameObject3D* testObject = new GameObject3D(resourceLoader.loadPBRModel("pbr/", "Cerberus_LP.FBX"), pbrRenderShader);
//	testObject->setScale(0.05f, 0.05f, 0.05f);
//	testObject->setPosition(0.0f, -2.0f, 0.0f);
//	testObject->update();
//	Material* mat = testObject->getMesh()->getMaterial(0);
//
//	mat->setAlbedo(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_A.tga", TextureParameters().setSRGB(true)));
//	mat->setMetalness(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_M.tga"));
//	mat->setRoughness(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_R.tga"));
//	mat->setNormalMap(Texture::loadTexture(resourceLoader.getAbsPathModels() + "pbr/Textures/Cerberus_N.tga"));
//	mat->setAlbedo(Colour(1.0f, 1.0f, 1.0f));
//	mat->setRoughness(1.0f);
//	mat->setMetalness(1.0f);
//	mat->setAO(1.0f);

	//testObject->getMesh()->getMaterials()[1] = mat;

//	renderScene->add(testObject);

	camera->setMovementSpeed(5.0f);
}

void Test::onUpdate() {
	if (Keyboard::isPressed(GLFW_KEY_UP))
		light0->getTransform()->translate(0.0f, 0.0f, -0.008f * getDelta());
	else if (Keyboard::isPressed(GLFW_KEY_DOWN))
		light0->getTransform()->translate(0.0f, 0.0f, 0.008f * getDelta());
	if (Keyboard::isPressed(GLFW_KEY_LEFT))
		light0->getTransform()->translate(-0.008f * getDelta(), 0.0f, 0.0f);
	else if (Keyboard::isPressed(GLFW_KEY_RIGHT))
		light0->getTransform()->translate(0.008f * getDelta(), 0.0f, 0.0f);
	light0->update();

	mit1->getMesh()->updateAnimation(getDeltaSeconds());

	//camera->setViewMatrix(light0->getLightShadowTransform(1));
}

void Test::onRender() {
	renderScene->showDeferredBuffers(); //For deferred rendering need to disable MSAA for this to work
}

void Test::onDestroy() {

}

void Test::onKeyPressed(int key) {
	BaseTest3D::onKeyPressed(key);
	if (key == GLFW_KEY_Q)
		renderScene->addLight((new Light(Light::TYPE_POINT, Vector3f(2.0f, 2.0f, 0.0f), false))->setDiffuseColour(Colour(23.47f, 0.0f, 0.0f)));
	else if (key == GLFW_KEY_1)
		renderScene->setExposure(0.25f);
	else if (key == GLFW_KEY_2)
		renderScene->setExposure(0.5f);
	else if (key == GLFW_KEY_3)
		renderScene->setExposure(1.0f);
	else if (key == GLFW_KEY_4)
		renderScene->setExposure(2.0f);
	else if (key == GLFW_KEY_5)
		renderScene->setExposure(4.0f);
	else if (key == GLFW_KEY_6)
		renderScene->setExposure(8.0f);
}

#endif /* TESTS_BASEENGINETEST3D_H_ */
