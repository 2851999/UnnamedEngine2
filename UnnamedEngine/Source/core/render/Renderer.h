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
#include "DataUsage.h"

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

	/* Structure for storing info about unloaded graphics pipelines */
	struct UnloadedGraphicsPipelineLayoutInfo {
		unsigned int                       renderShader;
		GraphicsPipeline::VertexInputData  vertexInputData;
		GraphicsPipeline::ColourBlendState colourBlendState;
		GraphicsPipeline::DepthState       depthState;
		GraphicsPipeline::CullState        cullState;
		bool                               viewportFlippedVk;
	};

	static ShaderInterface* shaderInterface;

	static ShaderBlock_Skinning shaderSkinningData;

	static std::vector<Camera*> cameras;
	static std::unordered_map<unsigned int, UnloadedShaderInfo> unloadedShaders;
	static std::unordered_map<unsigned int, RenderShader*> loadedRenderShaders;
	static std::unordered_map<unsigned int, UnloadedGraphicsPipelineLayoutInfo> unloadedGraphicsPipelineLayouts;
	static std::unordered_map<unsigned int, GraphicsPipelineLayout*> loadedGraphicsPipelineLayouts;
	static std::unordered_map<unsigned int, std::vector<GraphicsPipeline*>> queuePipelines;
	static Texture* blank;
	static Cubemap* blankCubemap;

	/* The current bound graphics pipeline (Used for descriptor set binding) */
	static GraphicsPipeline* currentGraphicsPipeline;

	/* The default render pass used to render to the default framebuffers */
	static RenderPass* defaultRenderPass;
public:
	/* The names of default shaders loaded into the engine */
	static const unsigned int SHADER_MATERIAL                                       = 1;
	static const unsigned int SHADER_SKY_BOX                                        = 2;
	static const unsigned int SHADER_FONT                                           = 3;
	static const unsigned int SHADER_FONT_SDF                                       = 4;
	static const unsigned int SHADER_LIGHTING                                       = 5;
	static const unsigned int SHADER_LIGHTING_SKINNING                              = 6;
	static const unsigned int SHADER_BASIC_PBR_LIGHTING                             = 7;
	static const unsigned int SHADER_BASIC_PBR_LIGHTING_SKINNING                    = 8;
	static const unsigned int SHADER_FRAMEBUFFER                                    = 9;
	static const unsigned int SHADER_SHADOW_MAP                                     = 10;
	static const unsigned int SHADER_SHADOW_MAP_SKINNING                            = 11;
	static const unsigned int SHADER_SHADOW_CUBEMAP                                 = 12;
	static const unsigned int SHADER_SHADOW_CUBEMAP_SKINNING                        = 13;
	static const unsigned int SHADER_GAMMA_CORRECTION_FXAA                          = 14;
	static const unsigned int SHADER_DEFERRED_LIGHTING_GEOMETRY                     = 15;
	static const unsigned int SHADER_DEFERRED_LIGHTING_SKINNING_GEOMETRY            = 16;
	static const unsigned int SHADER_DEFERRED_LIGHTING                              = 17;
	static const unsigned int SHADER_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY           = 18;
	static const unsigned int SHADER_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY  = 19;
	static const unsigned int SHADER_BASIC_PBR_DEFERRED_LIGHTING                    = 20;
	static const unsigned int SHADER_BASIC_PBR_DEFERRED_LIGHTING_BLOOM              = 21;
	static const unsigned int SHADER_DEFERRED_PBR_SSR                               = 22;
	static const unsigned int SHADER_TILEMAP                                        = 23;
	static const unsigned int SHADER_PARTICLE_SYSTEM                                = 24;
	static const unsigned int SHADER_TERRAIN                                        = 25;
	static const unsigned int SHADER_DEFERRED_TERRAIN_GEOMETRY                      = 26;
	static const unsigned int SHADER_PBR_GEN_EQUI_TO_CUBE_MAP                       = 27;
	static const unsigned int SHADER_PBR_GEN_IRRADIANCE_MAP                         = 28;
	static const unsigned int SHADER_PBR_GEN_PREFILTER_MAP                          = 29;
	static const unsigned int SHADER_PBR_GEN_BRDF_INTEGRATION_MAP                   = 30;
	static const unsigned int SHADER_PBR_LIGHTING                                   = 31;
	static const unsigned int SHADER_PBR_LIGHTING_SKINNING                          = 32;
	static const unsigned int SHADER_PBR_DEFERRED_LIGHTING_GEOMETRY                 = 33;
	static const unsigned int SHADER_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY        = 34;
	static const unsigned int SHADER_PBR_DEFERRED_LIGHTING                          = 35;
	static const unsigned int SHADER_PBR_DEFERRED_LIGHTING_BLOOM                    = 36;
	static const unsigned int SHADER_BILLBOARDED_FONT                               = 37;
	static const unsigned int SHADER_BILLBOARDED_FONT_SDF                           = 38;
	static const unsigned int SHADER_GAUSSIAN_BLUR                                  = 39;
	static const unsigned int SHADER_BLOOM                                          = 40;
	static const unsigned int SHADER_TEXTURE_PASSTHROUGH                            = 41;

	/* The names of default pipelines created for the engine */
	static const unsigned int GRAPHICS_PIPELINE_MATERIAL                                      = 1;
	static const unsigned int GRAPHICS_PIPELINE_SKY_BOX                                       = 2;
	static const unsigned int GRAPHICS_PIPELINE_FONT                                          = 3;
	static const unsigned int GRAPHICS_PIPELINE_FONT_SDF                                      = 4;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING                                      = 5;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_BLEND                                = 6;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_SKINNING                             = 7;
	static const unsigned int GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND                       = 8;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING                            = 9;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_BLEND                      = 10;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING                   = 11;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING_BLEND             = 12;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_MAP                                    = 13;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING                           = 14;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_CUBEMAP                                = 15;
	static const unsigned int GRAPHICS_PIPELINE_SHADOW_CUBEMAP_SKINNING                       = 16;
	static const unsigned int GRAPHICS_PIPELINE_GUI                                           = 17;
	static const unsigned int GRAPHICS_PIPELINE_GAMMA_CORRECTION_FXAA                         = 18;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_LIGHTING_GEOMETRY                    = 19;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_LIGHTING_SKINNING_GEOMETRY           = 20;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_LIGHTING                             = 21;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_LIGHTING_BLEND                       = 22;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY          = 23;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY = 24;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING                   = 25;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLEND             = 26;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLOOM             = 27;
	static const unsigned int GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLOOM_BLEND       = 28;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_PBR_SSR                              = 29;
	static const unsigned int GRAPHICS_PIPELINE_SPRITE                                        = 30;
	static const unsigned int GRAPHICS_PIPELINE_TILEMAP                                       = 31;
	static const unsigned int GRAPHICS_PIPELINE_PARTICLE_SYSTEM                               = 32;
	static const unsigned int GRAPHICS_PIPELINE_TERRAIN                                       = 33;
	static const unsigned int GRAPHICS_PIPELINE_TERRAIN_BLEND                                 = 34;
	static const unsigned int GRAPHICS_PIPELINE_DEFERRED_TERRAIN_GEOMETRY                     = 35;
	static const unsigned int GRAPHICS_PIPELINE_PBR_LIGHTING                                  = 36;
	static const unsigned int GRAPHICS_PIPELINE_PBR_LIGHTING_BLEND                            = 37;
	static const unsigned int GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING                         = 38;
	static const unsigned int GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING_BLEND                   = 39;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_GEOMETRY                = 40;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY       = 41;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING                         = 42;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLEND                   = 43;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLOOM                   = 44;
	static const unsigned int GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLOOM_BLEND             = 45;
	static const unsigned int GRAPHICS_PIPELINE_BILLBOARDED_FONT                              = 46;
	static const unsigned int GRAPHICS_PIPELINE_BILLBOARDED_FONT_SDF                          = 47;
	static const unsigned int GRAPHICS_PIPELINE_PBR_GEN_EQUI_TO_CUBE_MAP                      = 48;
	static const unsigned int GRAPHICS_PIPELINE_PBR_GEN_IRRADIANCE_MAP                        = 49;
	static const unsigned int GRAPHICS_PIPELINE_PBR_GEN_PREFILTER_MAP                         = 50;
	static const unsigned int GRAPHICS_PIPELINE_PBR_GEN_BRDF_INTEGRATION_MAP                  = 51;
	static const unsigned int GRAPHICS_PIPELINE_GAUSSIAN_BLUR                                 = 52;
	static const unsigned int GRAPHICS_PIPELINE_BLOOM                                         = 53;
	static const unsigned int GRAPHICS_PIPELINE_TEXTURE_PASSTHROUGH                           = 54;

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

	static void render(RenderData* renderData, Matrix4f& modelMatrix, Material* material);

	/* Method used to render a Mesh */
	static void render(Mesh* mesh, Matrix4f& modelMatrix, RenderShader* shader);

	/* Method use to destroy any objects that were created */
	static void destroy();

	/* Loads and returns an engine shader from the resources */
	static Shader* loadEngineShader(UnloadedShaderInfo& shaderInfo);

	/* Method used to add a RenderShader given a the paths to the shaders */
	static void addRenderShader(unsigned int id, std::string forwardShaderPath, std::vector<std::string> defines = {});

	/* Method used to add a GraphicsPipelineLayout */
	static void addGraphicsPipelineLayout(unsigned int id, unsigned int renderShader, GraphicsPipeline::VertexInputData vertexInputData, GraphicsPipeline::ColourBlendState colourBlendState, GraphicsPipeline::DepthState depthState, GraphicsPipeline::CullState cullState, bool viewportFlippedVk);

	/* Method to assign the current graphics pipeline */
	static void setCurrentGraphicsPipeline(GraphicsPipeline* pipeline);

	/* Method used to load a render shader from the list of render shaders and store it ready for use */
	static void loadRenderShader(unsigned int id);

	/* Method used to add a RenderShader */
	static void addRenderShader(RenderShader* renderShader);

	/* Method used to add a GraphicsPipeline to the list of queued pipelines */
	static void addGraphicsPipelineQueue(unsigned int id, GraphicsPipeline* pipeline);

	/* Method used to remove the last GraphicsPipeline from the list of queued pipelines */
	static void removeGraphicsPipelineQueue(unsigned int id);

	/* Method used to get the current graphics pipeline for queued rendering (last added will be used) */
	static GraphicsPipeline* getGraphicsPipelineQueue(unsigned int id);

	/* Returns the ShaderInterface instance */
	static inline ShaderInterface* getShaderInterface() { return shaderInterface; }

	/* Returns the RenderShader with a specific id */
	static RenderShader* getRenderShader(unsigned int id);

	/* Returns the GraphicsPipelineLayout with a specific id */
	static GraphicsPipelineLayout* getGraphicsPipelineLayout(unsigned int id);

	/* Returns the blank texture/cubemap */
	static inline Texture* getBlankTexture() { return blank; }
	static inline Cubemap* getBlankCubemap() { return blankCubemap; }

	/* Returns the last bound graphics pipeline */
	static inline GraphicsPipeline* getCurrentGraphicsPipeline() { return currentGraphicsPipeline; }

	/* Returns the default render pass */
	static inline RenderPass* getDefaultRenderPass() { return defaultRenderPass; }

	/* Method used to convert 'DataUsage' into the OpenGL equivalent */
	static GLenum convertToGL(DataUsage usage);

	/* Method to compile a shader to SPIRV for use with Vulkan */
	static void compileEngineShaderToSPIRV(unsigned int id, std::string glslangValidatorPath);

	/* Method to compile all possible shaders to SPIRV for use with Vulkan */
	static void compileEngineShadersToSPIRV(std::string glslangValidatorPath);

	/* Method to compile currently loaded engine shaders to SPIRV for use with Vulkan */
	static void compileLoadedEngineShadersToSPIRV(std::string glslValidatorPath);
};
