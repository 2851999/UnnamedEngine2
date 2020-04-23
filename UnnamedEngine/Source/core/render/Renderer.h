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
#include "UBO.h"
#include "ShaderInterface.h"
#include "GraphicsPipeline.h"

/*****************************************************************************
 * The Renderer class contains everything need to render a Mesh
 *****************************************************************************/

class Renderer {
private:
	/* Structure for storing info about unloaded shaders */
	struct UnloadedShaderInfo {
		std::string path;
		std::vector<std::string> defines;
	};

	static ShaderInterface* shaderInterface;

	static ShaderBlock_Material shaderMaterialData;
	static ShaderBlock_Skinning shaderSkinningData;

	static std::vector<Camera*> cameras;
	static std::unordered_map<unsigned int, UnloadedShaderInfo> unloadedShaders;
	static std::unordered_map<unsigned int, RenderShader*> loadedRenderShaders;
	static std::unordered_map<unsigned int, GraphicsPipelineLayout*> graphicsPipelineLayouts;
	static Texture* blank;

	/* The current bound graphics pipeline (Used for descriptor set binding) */
	static GraphicsPipeline* currentGraphicsPipeline;

	/* The default render pass used to render to the default framebuffers */
	static RenderPass* defaultRenderPass;
public:
	/* The names of default shaders loaded into the engine */
	static const unsigned int SHADER_MATERIAL;
	static const unsigned int SHADER_SKY_BOX;
	static const unsigned int SHADER_FONT;
	static const unsigned int SHADER_LIGHTING;
	static const unsigned int SHADER_LIGHTING_SKINNING;
	static const unsigned int SHADER_FRAMEBUFFER;
	static const unsigned int SHADER_SHADOW_MAP;
	static const unsigned int SHADER_SHADOW_MAP_SKINNING;

	/* The names of default pipelines created for the engine */
	static const unsigned int GRAPHICS_PIPELINE_MATERIAL;
	static const unsigned int GRAPHICS_PIPELINE_SKY_BOX;
	static const unsigned int GRAPHICS_PIPELINE_FONT;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_BLEND;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_SKINNING;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_MAP;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING;

	/* Methods used to add/remove a camera to use for rendering - the renderer
	 * uses the last camera added when rendering */
	static void addCamera(Camera* camera);
	static void removeCamera();

	/* Returns the current camera for rendering */
	static Camera* getCamera();

	/* Method used to initialise the rendering system */
	static void initialise();

	/* TMethod used to apply the material properties to a shader assuming it is already being used */
	static void useMaterial(RenderData* renderData, unsigned int materialIndex, Material* material);

	/* Method used to stop applying a material (unbinds required textures) */
	static void stopUsingMaterial(Material* material);

	/* Method used to render a Mesh */
	static void render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* shader);

	/* Method use to destroy any objects that were created */
	static void destroy();

	/* Loads and returns an engine shader from the resources */
	static Shader* loadEngineShader(UnloadedShaderInfo& shaderInfo);

	/* Method used to add a RenderShader given a the paths to the shaders */
	static void addRenderShader(unsigned int id, std::string forwardShaderPath, std::vector<std::string> defines = {});

	/* Method used to add a GraphicsPipelineLayout */
	static void addGraphicsPipelineLayout(unsigned int id, GraphicsPipelineLayout* pipelineLayout);

	/* Method to assign the current graphics pipeline */
	static void setCurrentGraphicsPipeline(GraphicsPipeline* pipeline);

	/* Method used to load a render shader from the list of render shaders and store it ready for use */
	static void loadRenderShader(unsigned int id);

	/* Method used to add a RenderShader */
	static void addRenderShader(RenderShader* renderShader);

	/* Returns the ShaderInterface instance */
	static inline ShaderInterface* getShaderInterface() { return shaderInterface; }

	/* Returns the RenderShader with a specific id */
	static RenderShader* getRenderShader(unsigned int id);

	/* Returns the GraphicsPipelineLayout with a specific id */
	static GraphicsPipelineLayout* getGraphicsPipelineLayout(unsigned int id);

	/* Returns the blank texture */
	static inline Texture* getBlankTexture() { return blank; }

	/* Returns the last bound graphics pipeline */
	static inline GraphicsPipeline* getCurrentGraphicsPipeline() { return currentGraphicsPipeline; }

	/* Returns the default render pass */
	static inline RenderPass* getDefaultRenderPass() { return defaultRenderPass; }
};

