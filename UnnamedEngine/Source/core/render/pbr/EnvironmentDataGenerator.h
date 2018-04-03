/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#ifndef CORE_RENDER_PBR_ENVIRONMENTDATAGENERATOR_H_
#define CORE_RENDER_PBR_ENVIRONMENTDATAGENERATOR_H_

#include "../Texture.h"
#include "../../Matrix.h"

/*****************************************************************************
 * The EnvironmentDataGenerator class is used to generate the cubemaps and
 * textures used for a PBR environment
 *****************************************************************************/

class EnvironmentDataGenerator {
private:
	/* The projection and view matrices used in rendering a cubemap's individual faces */
	const Matrix4f captureProjection = Matrix4f().initPerspective(90.0f, 1.0f, 0.1f, 10.0f);
	const Matrix4f captureViews[6] = {
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f,  0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f,  0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, -1.0f,  0.0f))
	};

	/* The output textures */
	Cubemap* environmentCubemap = NULL;
	Cubemap* irradianceCubemap = NULL;
	Cubemap* prefilterCubemap = NULL;

	Texture* brdfLUTTexture = NULL;
public:
	/* The constructor */
	EnvironmentDataGenerator() {}

	/* The method used to generate the data given a path to a equirectangular
	 * environment map texture */
	void loadAndGenerate(std::string path);

	/* Getters */
	Cubemap* getEnvironmentCubemap() { return environmentCubemap; }
	Cubemap* getIrradianceCubemap() { return irradianceCubemap; }
	Cubemap* getPrefilterCubemap() { return prefilterCubemap; }

	Texture* getBRDFLUTTexture() { return brdfLUTTexture; }
};



#endif /* CORE_RENDER_PBR_ENVIRONMENTDATAGENERATOR_H_ */
