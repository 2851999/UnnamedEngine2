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

#ifndef CORE_RENDER_RENDERER_H_
#define CORE_RENDER_RENDERER_H_

#include <unordered_map>
#include "Camera.h"
#include "FBO.h"
#include "UBO.h"
#include "ShaderInterface.h"

/*****************************************************************************
 * The Renderer class contains everything need to render a Mesh
 *****************************************************************************/

class Renderer {
private:
	static ShaderInterface* shaderInterface;

	static ShaderBlock_Core     shaderCoreData;
	static ShaderBlock_Material shaderMaterialData;
	static ShaderBlock_Skinning shaderSkinningData;

	static std::vector<Camera*> cameras;
	static std::vector<Texture*> boundTextures;
	static std::unordered_map<unsigned int, std::vector<std::string>> renderShaderPaths;
	static std::unordered_map<unsigned int, RenderShader*> loadedRenderShaders;
	static Texture* blank;

	/* This mesh is used to render a FramebufferTexture to the screen - useful
	 * for post processing effects */
	static MeshRenderData* screenTextureMesh;

	/* Stores the sizes of the boundTextures array, at the moment saveTextures() is
	 * called */
	static std::vector<unsigned int> boundTexturesOldSize;

	/* The current graphics state being used */
	static GraphicsState* currentGraphicsState;

	/* States whether graphics states should be ignored */
	static bool shouldIgnoreGraphicsStates;

	/* Assigns texture uniforms for a material */
	static void assignMatTexture(Shader* shader, std::string type, Texture* texture);
public:
	/* The names of default shaders loaded into the engine */
	static const unsigned int SHADER_MATERIAL;
	static const unsigned int SHADER_SKY_BOX;
	static const unsigned int SHADER_FONT;
	static const unsigned int SHADER_BILLBOARD;
	static const unsigned int SHADER_PARTICLE;
	static const unsigned int SHADER_LIGHTING;
	static const unsigned int SHADER_FRAMEBUFFER;
	static const unsigned int SHADER_ENVIRONMENT_MAP;
	static const unsigned int SHADER_SHADOW_MAP;
	static const unsigned int SHADER_SHADOW_CUBEMAP;
	static const unsigned int SHADER_BILLBOARDED_FONT;
	static const unsigned int SHADER_TERRAIN;
	static const unsigned int SHADER_PLAIN_TEXTURE;
	static const unsigned int SHADER_DEFERRED_LIGHTING;
	static const unsigned int SHADER_TILEMAP;
	static const unsigned int SHADER_VULKAN;
	static const unsigned int SHADER_VULKAN_LIGHTING;

	static const unsigned int SHADER_PBR_EQUI_TO_CUBE_GEN;
	static const unsigned int SHADER_PBR_IRRADIANCE_MAP_GEN;
	static const unsigned int SHADER_PBR_PREFILTER_MAP_GEN;
	static const unsigned int SHADER_PBR_BRDF_INTEGRATION_MAP_GEN;
	static const unsigned int SHADER_PBR_LIGHTING;
	static const unsigned int SHADER_PBR_DEFERRED_LIGHTING;

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

	/* Used to ignore the graphics states */
	static inline void ignoreGraphicsStates(bool value) { shouldIgnoreGraphicsStates = value; }

	/* TMethod used to apply the material properties to a shader assuming it is already being used */
	static void useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material, UBO* materialUBO);

	/* Method used to stop applying a material (unbinds required textures) */
	static void stopUsingMaterial(Material* material);

	/* Method used to apply a graphics state */
	static void useGraphicsState(GraphicsState* state);

	/* Method used to render a Mesh */
	static void render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* shader);

	/* Method used to render a FramebufferStore */
	static void render(FramebufferStore* texture, Shader* shader = NULL, std::string textureUniform = "Texture");

	/* Method use to destroy any objects that were created */
	static void destroy();

	/* Loads and returns an engine shader from the resources */
	static Shader* loadEngineShader(std::string path);

	/* Method used to prepare a shader by adding its required uniforms provided the id is recognised */
	static void prepareForwardShader(unsigned int id, Shader* shader);

	/* Method used to prepare a shader by adding its required uniforms provided the id is recognised */
	static void prepareDeferredGeomShader(unsigned int id, Shader* shader);

	/* Method used to add a RenderShader given a the paths to the shaders */
	static void addRenderShader(unsigned int id, std::string forwardShaderPath, std::string deferredGeomShaderPath = "");

	/* Method used to load a render shader from the list of render shaders and store it ready for use */
	static void loadRenderShader(unsigned int id);

	/* Assigns the graphics state for a particular shader based on its ID */
	static void assignGraphicsState(GraphicsState* state, unsigned int shaderID);

	/* Method used to add a RenderShader */
	static void addRenderShader(RenderShader* renderShader);

	/* Returns the ShaderInterface instance */
	static inline ShaderInterface* getShaderInterface() { return shaderInterface; }

	/* Returns the RenderShader with a specific id */
	static RenderShader* getRenderShader(unsigned int id);

	/* Returns the blank texture */
	static inline Texture* getBlankTexture() { return blank; }

	/* Returns the screen texture mesh */
	static inline MeshRenderData* getScreenTextureMesh() { return screenTextureMesh; }

	/* Returns a reference to the data for the core UBO */
	static inline ShaderBlock_Core& getShaderBlock_Core() { return shaderCoreData; }
};

#endif /* CORE_RENDER_RENDERER_H_ */
