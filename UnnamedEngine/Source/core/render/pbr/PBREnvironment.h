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

#ifndef CORE_RENDER_PBR_PBRENVIRONMENT_H_
#define CORE_RENDER_PBR_PBRENVIRONMENT_H_


/*****************************************************************************
 * The PBREnvironment class is used to store the textures required for
 * representing an environment
 *****************************************************************************/

#include "../Texture.h"

class PBREnvironment {
private:
	/* The cubemaps and textures required */
	Cubemap* environmentCubemap = NULL;
	Cubemap* irradianceCubemap = NULL;
	Cubemap* prefilterCubemap = NULL;

	Texture* brdfLUTTexture = NULL;
public:
	/* The constructor */
	PBREnvironment(Cubemap* environmentCubemap, Cubemap* irradianceCubemap, Cubemap* prefilterCubemap, Texture* brdfLUTTexture) :
		environmentCubemap(environmentCubemap), irradianceCubemap(irradianceCubemap), prefilterCubemap(prefilterCubemap), brdfLUTTexture(brdfLUTTexture) {}

	/* The destructor */
	virtual ~PBREnvironment() {
		//Delete the textures
		delete environmentCubemap;
		delete irradianceCubemap;
		delete prefilterCubemap;
		delete brdfLUTTexture;
	}

	/* Getters */
	Cubemap* getEnvironmentCubemap() { return environmentCubemap; }
	Cubemap* getIrradianceCubemap() { return irradianceCubemap; }
	Cubemap* getPrefilterCubemap() { return prefilterCubemap; }

	Texture* getBRDFLUTTexture() { return brdfLUTTexture; }

	/* The method used to generate an environment given a path to a equirectangular
	 * environment map texture */
	static PBREnvironment* loadAndGenerate(std::string path);
};


#endif /* CORE_RENDER_PBR_PBRENVIRONMENT_H_ */
