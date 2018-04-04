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

/*****************************************************************************
 * The Renderer class contains everything need to render a Mesh
 *****************************************************************************/

class Renderer {
private:
	static std::vector<Camera*> cameras;
	static std::vector<Texture*> boundTextures;
	static std::unordered_map<std::string, RenderShader*> renderShaders;
	static Texture* blank;

	/* This mesh is used to render a FramebufferTexture to the screen - useful
	 * for post processing effects */
	static MeshRenderData* screenTextureMesh;

	/* The size of the boundTextures array, at the moment saveTextures() is
	 * called */
	static unsigned int boundTexturesOldSize;
public:
	/* The names of default shaders loaded into the engine */
	static const std::string SHADER_MATERIAL;
	static const std::string SHADER_SKY_BOX;
	static const std::string SHADER_FONT;
	static const std::string SHADER_BILLBOARD;
	static const std::string SHADER_PARTICLE;
	static const std::string SHADER_LIGHTING;
	static const std::string SHADER_FRAMEBUFFER;
	static const std::string SHADER_ENVIRONMENT_MAP;
	static const std::string SHADER_SHADOW_MAP;
	static const std::string SHADER_BILLBOARDED_FONT;
	static const std::string SHADER_TERRAIN;
	static const std::string SHADER_PLAIN_TEXTURE;
	static const std::string SHADER_DEFERRED_LIGHTING;
	static const std::string SHADER_PBR_EQUI_TO_CUBE;
	static const std::string SHADER_PBR_IRRADIANCE;
	static const std::string SHADER_PBR_PREFILTER;
	static const std::string SHADER_PBR_BRDF;

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
	static inline void saveTextures() {
		boundTexturesOldSize = boundTextures.size();
	}

	static inline unsigned int getNumBoundTextures() {
		return boundTextures.size();
	}

	/* Releases extra textures so that the boundTextures size is the same
	 * as it was when saveTextures() was called */
	static inline void releaseNewTextures() {
		while (boundTextures.size() > boundTexturesOldSize)
			unbindTexture();
		boundTexturesOldSize = boundTextures.size();
	}

	/* Method used to initialise the rendering system */
	static void initialise();

	/* The method used to apply the material properties to a shader assuming it is already being used */
	static void setMaterialUniforms(Shader* shader, std::string shaderName, Material* material);

	/* Method used to render a Mesh */
	static void render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* shader);

	/* Method used to render a FramebufferTexture */
	static void render(FramebufferTexture* texture, Shader* shader = NULL);

	/* Method use to destroy any objects that were created */
	static void destroy();

	/* Loads and returns an engine shader from the resources */
	static Shader* loadEngineShader(std::string path);

	/* Method used to prepare a shader by adding its required uniforms provided the id is recognised */
	static void prepareForwardShader(std::string id, Shader* shader);

	/* Method used to prepare a shader by adding its required uniforms provided the id is recognised */
	static void prepareDeferredGeomShader(std::string id, Shader* shader);

	/* Method used to add a RenderShader given a Shader - this method will also setup the shader
	 * providing the id is recognised */
	static void addRenderShader(std::string id, Shader* forwardShader, Shader* deferredGeomShader = NULL);

	/* Method used to add a RenderShader */
	static void addRenderShader(RenderShader* renderShader);

	/* Returns the RenderShader with a specific id */
	static RenderShader* getRenderShader(std::string id);

	/* Returns the blank texture */
	static inline Texture* getBlankTexture() { return blank; }

	/* Returns the screen texture mesh */
	static inline MeshRenderData* getScreenTextureMesh() { return screenTextureMesh; }
};

#endif /* CORE_RENDER_RENDERER_H_ */
