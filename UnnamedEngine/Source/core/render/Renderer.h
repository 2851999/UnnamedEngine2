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

#include <unordered_map>
#include "Camera.h"
#include "FBO.h"
#include "UBO.h"
#include "ShaderInterface.h"
#include "RenderPipeline.h"

/*****************************************************************************
 * The Renderer class contains everything need to render a Mesh
 *****************************************************************************/

class Renderer {
private:
	static ShaderInterface* shaderInterface;

	static ShaderBlock_Material shaderMaterialData;
	static ShaderBlock_Skinning shaderSkinningData;

	static std::vector<Camera*> cameras;
	static std::vector<Texture*> boundTextures;
	static std::unordered_map<unsigned int, std::string> renderShaderPaths;
	static std::unordered_map<unsigned int, RenderShader*> loadedRenderShaders;
	static std::unordered_map<unsigned int, RenderPipeline*> renderPipelines;
	static Texture* blank;

	/* Stores the sizes of the boundTextures array, at the moment saveTextures() is
	 * called */
	static std::vector<unsigned int> boundTexturesOldSize;

	/* Assigns texture uniforms for a material */
	static void assignMatTexture(Shader* shader, std::string type, Texture* texture);
public:
	/* The names of default shaders loaded into the engine */
	static const unsigned int SHADER_MATERIAL;
	static const unsigned int SHADER_SKY_BOX;
	static const unsigned int SHADER_FONT;
	static const unsigned int SHADER_VULKAN_LIGHTING;

	/* The names of default pipelines created for the engine */
	static const unsigned int PIPELINE_MATERIAL;
	static const unsigned int PIPELINE_SKY_BOX;
	static const unsigned int PIPELINE_FONT;
	static const unsigned int PIPELINE_LIGHTING;
	static const unsigned int PIPELINE_LIGHTING_BLEND;

	/* Methods used to add/remove a camera to use for rendering - the renderer
	 * uses the last camera added when rendering */
	static void addCamera(Camera* camera);
	static void removeCamera();

	/* Returns the current camera for rendering */
	static Camera* getCamera();

	/* Methods used to bind/unbind a texture */
	static GLuint bindTexture(Texture* texture);
	static void unbindTexture();
	/* Unbinds all textures */
	static inline void unbindTextures() {
		while (boundTextures.size() > 0)
			unbindTexture();
	}

	/* Used to store the current number of boundTextures so new textures
	 * can be released later */
	static void saveTextures();

	/* Releases extra textures so that the boundTextures size is the same
	 * as it was when saveTextures() was last called */
	static void releaseNewTextures();

	static unsigned int getNumBoundTextures() {
		return boundTextures.size();
	}

	/* Method used to initialise the rendering system */
	static void initialise();

	/* TMethod used to apply the material properties to a shader assuming it is already being used */
	static void useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material);

	/* Method used to stop applying a material (unbinds required textures) */
	static void stopUsingMaterial(Material* material);

	/* Method called at the start of the frame (to update per frame data) */
	static void preRender();

	/* Method used to render a Mesh */
	static void render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* shader);

	/* Method use to destroy any objects that were created */
	static void destroy();

	/* Loads and returns an engine shader from the resources */
	static Shader* loadEngineShader(std::string path);

	/* Method used to add a RenderShader given a the paths to the shaders */
	static void addRenderShader(unsigned int id, std::string forwardShaderPath);

	/* Method used to add a RenderPipeline */
	static void addPipeline(unsigned int id, RenderPipeline* pipeline);

	/* Method used to load a render shader from the list of render shaders and store it ready for use */
	static void loadRenderShader(unsigned int id);

	/* Method used to add a RenderShader */
	static void addRenderShader(RenderShader* renderShader);

	/* Returns the ShaderInterface instance */
	static inline ShaderInterface* getShaderInterface() { return shaderInterface; }

	/* Returns the RenderShader with a specific id */
	static RenderShader* getRenderShader(unsigned int id);

	/* Returns the RenderPipeline with a specific id */
	static RenderPipeline* getPipeline(unsigned int id);

	/* Returns the blank texture */
	static inline Texture* getBlankTexture() { return blank; }
};

