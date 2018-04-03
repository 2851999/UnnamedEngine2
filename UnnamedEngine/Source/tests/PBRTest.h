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
#include "../core/render/pbr/EnvironmentDataGenerator.h"

class Test : public BaseTest3D {
private:
	std::vector<GameObject3D*> spheres;

	Texture* albedo;
	Texture* normal;
	Texture* metallic;
	Texture* roughness;
	Texture* ao;

	RenderShader* pbrRenderShader;
	Cubemap* environmentMap;
	Cubemap* irradianceMap;
	Cubemap* prefilterMap;
	Texture* brdfLUTTexture;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onDestroy() override;
};

void Test::onInitialise() {
//	getSettings().videoVSync = false;
//	getSettings().videoMaxFPS = 0;
}

void Test::onCreated() {

	//MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "SimpleSphere.obj");

	//unsigned int envMap, irMap, prefilMap, brdfLUTMap;

	EnvironmentDataGenerator generator;
	generator.loadAndGenerate(resourceLoader.getAbsPathTextures() + "PBR/Theatre-Center_2k.hdr");
	//EquiToCube::generateCubemapAndIrradiance(resourceLoader.getAbsPathTextures() + "PBR/Theatre-Center_2k.hdr", envMap, irMap, prefilMap, brdfLUTMap);

	environmentMap = generator.getEnvironmentCubemap();
	irradianceMap = generator.getIrradianceCubemap();
	prefilterMap = generator.getPrefilterCubemap();
	brdfLUTTexture = generator.getBRDFLUTTexture();

	//Cubemap* environmentMap = EquiToCube::generateCubemap(resourceLoader.getAbsPathTextures() + "PBR/WinterForest_Ref.hdr");
	//Cubemap* irradianceMap = EquiToCube::generateIrradianceMap(environmentMap);

//	std::cout << environmentMap->getHandle() << std::endl;
//	std::cout << irradianceMap->getHandle() << std::endl;

	camera->setSkyBox(new SkyBox(environmentMap));
	//camera->setSkyBox(new SkyBox(resourceLoader.getAbsPathTextures() + "skybox2/", ".jpg", 100.0f));
	camera->setFlying(true);

	pbrRenderShader = new RenderShader("PBRShader", Renderer::loadEngineShader("pbr/PBRShader"), NULL);
	Renderer::prepareForwardShader("PBRShader", pbrRenderShader->getForwardShader());

	std::string path = "C:/UnnamedEngine/textures/PBR/";
//	albedo = Texture::loadTexture(path + "tile-basecolour.png");
//	normal = Texture::loadTexture(path + "tile-normal.png");
//	metallic = Texture::loadTexture(path + "tile-metalness.png");
//	roughness = Texture::loadTexture(path + "tile-roughness.png");
//	ao = Texture::loadTexture(path + "streakedmetal-metalness.png");

//	albedo = Texture::loadTexture(path + "granite/albedo.jpg");
//	normal = Texture::loadTexture(path + "granite/normal.jpg");
//	metallic = Texture::loadTexture(path + "granite/metalness.png");
//	roughness = Texture::loadTexture(path + "granite/roughness.jpg");
//	ao = Texture::loadTexture(path + "granite/ao.jpg");

//	albedo = Texture::loadTexture(path + "streakedmetal-albedo.png");
//	//normal = Texture::loadTexture(path + "normal.png");
//	metallic = Texture::loadTexture(path + "streakedmetal-metalness.png");
//	roughness = Texture::loadTexture(path + "streakedmetal-roughness.png");
//	ao = Texture::loadTexture(path + "streakedmetal-metalness.png");

	albedo = Texture::loadTexture(path + "bamboo-wood-semigloss-albedo.png");
	normal = Texture::loadTexture(path + "bamboo-wood-semigloss-normal.png");
	metallic = Texture::loadTexture(path + "bamboo-wood-semigloss-metal.png");
	roughness = Texture::loadTexture(path + "bamboo-wood-semigloss-roughness.png");
	ao = Texture::loadTexture(path + "bamboo-wood-semigloss-ao.png");

	for (int i = 0; i < 16; i++) {
		GameObject3D* sphere = new GameObject3D(resourceLoader.loadModel("", "SimpleSphere.obj"), pbrRenderShader);
		sphere->getMesh()->getMaterial(1)->normalMap = normal;

		//resourceLoader.loadModel("", "SimpleSphere.model") //Normals not smooth????
		//MeshLoader::loadModel("resources/objects/", "plain_sphere.model")

		int x = i % 4;
		int y = (int) (i / 4.0f);
		sphere->setPosition(x * 2, y * 2, -0.5f);
		//sphere->setScale(0.25f, 0.25f, 0.25f);

		sphere->update();

		spheres.push_back(sphere);
	}

	//renderScene->add(sphere);

	//Light* light0 = (new Light(Light::TYPE_DIRECTIONAL, Vector3f(), false))->setDirection(0, -1.0f, 0.0001f); //->setDiffuseColour(Colour(200.0f, 200.0f, 200.0f));
	//Light* light1 = (new Light(Light::TYPE_POINT, Vector3f(0.0f, 1.0f, 0.0f), false))->setDiffuseColour(Colour::RED);
	//light0->update();
	//light1->update();
	//renderScene->addLight(light0);
	//renderScene->addLight(light1);

	camera->setMovementSpeed(5.0f);
}

void Test::onUpdate() {

}

void Test::onRender() {
	Shader* shader = pbrRenderShader->getShader();
	shader->use();

	shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

	shader->setUniformi("Albedo", Renderer::bindTexture(albedo));
	shader->setUniformi("Metallic", Renderer::bindTexture(metallic));
	shader->setUniformi("Roughness", Renderer::bindTexture(roughness));
	shader->setUniformi("AO", Renderer::bindTexture(ao));
	shader->setUniformi("IrradianceMap", Renderer::bindTexture(irradianceMap));
	shader->setUniformi("PrefilterMap", Renderer::bindTexture(prefilterMap));
	shader->setUniformi("BRDFLUT", Renderer::bindTexture(brdfLUTTexture));

	for (unsigned int i = 0; i < spheres.size(); i++) {

		Matrix4f modelMatrix = spheres[i]->getModelMatrix();

		shader->setUniformMatrix4("ModelMatrix", modelMatrix);
		shader->setUniformMatrix3("NormalMatrix", modelMatrix.to3x3().inverse().transpose());

		spheres[i]->render();
	}

	Renderer::unbindTexture();
	Renderer::unbindTexture();
	Renderer::unbindTexture();
	Renderer::unbindTexture();
	Renderer::unbindTexture();
	Renderer::unbindTexture();
	Renderer::unbindTexture();

	shader->stopUsing();
}

void Test::onDestroy() {

}

#endif /* TESTS_BASEENGINETEST3D_H_ */
