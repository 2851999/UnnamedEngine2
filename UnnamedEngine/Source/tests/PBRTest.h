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

//	Texture* albedo;
//	Texture* normal;
//	Texture* metallic;
//	Texture* roughness;
//	Texture* ao;

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
	MeshLoader::loadDiffuseTexturesAsSRGB = true;

	//MeshLoader::convertToEngineModel(resourceLoader.getAbsPathModels(), "SimpleSphere.obj");

	EnvironmentDataGenerator generator;
	generator.loadAndGenerate(resourceLoader.getAbsPathTextures() + "PBR/Newport_Loft_Ref.hdr");
	//EquiToCube::generateCubemapAndIrradiance(resourceLoader.getAbsPathTextures() + "PBR/Theatre-Center_2k.hdr", envMap, irMap, prefilMap, brdfLUTMap);

	environmentMap = generator.getEnvironmentCubemap();
	irradianceMap = generator.getIrradianceCubemap();
	prefilterMap = generator.getPrefilterCubemap();
	brdfLUTTexture = generator.getBRDFLUTTexture();

	camera->setSkyBox(new SkyBox(environmentMap));
	camera->setFlying(true);

	pbrRenderShader = Renderer::getRenderShader(Renderer::SHADER_PBR_LIGHTING);

	std::string path = "C:/UnnamedEngine/textures/PBR/";

	for (int i = 0; i < 1; i++) {
		GameObject3D* sphere = new GameObject3D(resourceLoader.loadModel("SimpleSphere/", "SimpleSphere.obj", true), pbrRenderShader);
		//GameObject3D* sphere = new GameObject3D(resourceLoader.loadModel("crytek-sponza/", "sponza.obj", true), pbrRenderShader);
		//sphere->setScale(0.15f, 0.15f, 0.15f);
		//Material* material = sphere->getMesh()->getMaterial(1);

		//material->setAO(1.0f);

		//resourceLoader.loadModel("", "SimpleSphere.model") //Normals not smooth????
		//MeshLoader::loadModel("resources/objects/", "plain_sphere.model")

		int x = i % 4;
		int y = (int) (i / 4.0f);

		sphere->setPosition(x * 2, y * 2, -0.5f);
		//sphere->setScale(0.25f, 0.25f, 0.25f);

//		material->setAlbedo(Colour(0.5f, 0.0f, 0.0f));
//		material->setMetalness(x * (1.0f / 3.0f));
//		material->setRoughness(utils_maths::clamp(y * (1.0f / 3.0f), 0.05f, 1.0f));

		sphere->update();

		spheres.push_back(sphere);
	}

	camera->setMovementSpeed(5.0f);
}

void Test::onUpdate() {

}

void Test::onRender() {
	Shader* shader = pbrRenderShader->getShader();
	shader->use();

	shader->setUniformVector3("CameraPosition", ((Camera3D*) Renderer::getCamera())->getPosition());

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

	shader->stopUsing();
}

void Test::onDestroy() {

}

#endif /* TESTS_BASEENGINETEST3D_H_ */
