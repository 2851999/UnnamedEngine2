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

#include "PBREnvironment.h"

#include "../../Matrix.h"
#include "../Renderer.h"
#include "../../Window.h"
#include "../../../utils/Utils.h"
#include "../../BaseEngine.h"

 /*****************************************************************************
  * The PBREnvironment class
  *****************************************************************************/

PBREnvironment* PBREnvironment::loadAndGenerate(std::string path) {
	VkCommandBuffer vulkanCommandBuffer;
	if (BaseEngine::usingVulkan()) {
		vulkanCommandBuffer = Vulkan::beginSingleTimeCommands();
		Vulkan::setOverrideCommandBuffer(vulkanCommandBuffer);
	}

	//For now these are the sizes of the maps (for cubemaps this is the size of each side)
	const unsigned int ENVIRONMENT_MAP_SIZE  = 512;
	const unsigned int IRRADIANCE_MAP_SIZE   = 32;
	const unsigned int PREFILTER_MAP_SIZE    = 128;
	const unsigned int BRDF_LUT_TEXTURE_SIZE = 512;

	//Calculate the max size for allocating the required data
	unsigned int maxSize = utils_maths::max(ENVIRONMENT_MAP_SIZE, utils_maths::max(IRRADIANCE_MAP_SIZE, utils_maths::max(PREFILTER_MAP_SIZE, BRDF_LUT_TEXTURE_SIZE)));

	/* The projection and view matrices used in rendering a cubemap's individual faces */
	const Matrix4f captureProjection = Matrix4f().initPerspective(90.0f, 1.0f, 0.1f, 10.0f);
	const Matrix4f captureViews[6] ={
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f, 0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f,  1.0f,  0.0f), Vector3f(0.0f,  0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f,  0.0f), Vector3f(0.0f,  0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f,  0.0f,  1.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f,  0.0f, -1.0f), Vector3f(0.0f, -1.0f,  0.0f))
	};

	//Create meshes to render a cubemap and 2D texture
	MeshData* cubeMeshData = MeshBuilder::createCube(1.0f, 1.0f, 1.0f);
	MeshData* quadMeshData = MeshBuilder::createQuad(Vector2f(-1.0f, -1.0f), Vector2f(1.0f, -1.0f), Vector2f(1.0f, 1.0f), Vector2f(-1.0f, 1.0f), NULL);

	MeshRenderData* cubeMesh = new MeshRenderData(cubeMeshData, Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_EQUI_TO_CUBE_MAP));
	MeshRenderData* quadMesh = new MeshRenderData(quadMeshData, Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_BRDF_INTEGRATION_MAP));
	std::vector<Material*> materials;
	cubeMesh->setup(cubeMeshData, materials);
	quadMesh->setup(quadMeshData, materials);

	//Load the texture for the equirectangular map ensuring it's the right way up
	Texture::setFlipVerticallyOnLoad(true);
	Texture* texture = Texture::loadTexturef(path, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, false)); //Vulkan has no float SRGB format so false to make both similar
	Texture::setFlipVerticallyOnLoad(false);

	//---------------------------------- RENDER ENVIRONMENT CUBEMAP FROM EQUIRECTANGULAR MAP ----------------------------------
	TextureParameters environmentCubemapTextureParameters = TextureParameters();
	//environmentCubemapTextureParameters.setMinFilter(TextureParameters::Filter::LINEAR_MIPMAP_LINEAR);
	environmentCubemapTextureParameters.setMinFilter(TextureParameters::Filter::LINEAR);
	environmentCubemapTextureParameters.setMagFilter(TextureParameters::Filter::LINEAR);
	environmentCubemapTextureParameters.setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
	environmentCubemapTextureParameters.setSRGB(true);

	FramebufferAttachment* environmentCubemap = new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::COLOUR_CUBEMAP, environmentCubemapTextureParameters, 1);
	//environmentCubemap->getParameters().preventGenerateMipMaps(); //MUST NOT HAVE OTHERWISE CUBEMAP INCOMPLETE

	FBO* fboEquiToCubemap = new FBO(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, {
		FramebufferAttachmentInfo{ environmentCubemap, true, -1, false }//,
		//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
	});

	RenderPass* renderPassEquiToCubeMap = new RenderPass(fboEquiToCubemap);
	GraphicsPipeline* pipelineEquiToCubeMap = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_EQUI_TO_CUBE_MAP), renderPassEquiToCubeMap, ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE);
	DescriptorSet* descriptorSetEquiToCubeMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP));
	descriptorSetEquiToCubeMap->setTexture(0, texture);

	ShaderBlock_PBRGenEnvMap genEnvMapData;
	for (unsigned int i = 0; i < 6; ++i)
		genEnvMapData.projectionViewMatrices[i] = captureProjection * captureViews[i];
	descriptorSetEquiToCubeMap->getShaderBuffer(0)->updateFrame(&genEnvMapData, 0, sizeof(ShaderBlock_PBRGenEnvMap));

	descriptorSetEquiToCubeMap->setup();

	renderPassEquiToCubeMap->begin();
	pipelineEquiToCubeMap->bind();
	descriptorSetEquiToCubeMap->bind();

	//Create the environment cubemap
	//TextureParameters envMapParameters = TextureParameters(GL_TEXTURE_CUBE_MAP, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true);
	//envMapParameters.setMinFilter(TextureParameters::Filter::LINEAR_MIPMAP_LINEAR);
	//envMapParameters.preventGenerateMipMaps();
	//Cubemap* environmentCubemap = Cubemap::createCubemap(ENVIRONMENT_MAP_SIZE, GL_RGB16F, GL_RGB, GL_FLOAT, envMapParameters);

	cubeMesh->render();

	//glClearColor(1.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);

	renderPassEquiToCubeMap->end();

	//Generate mip map as now assigned the texture
	//environmentCubemap->bind();
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//glClearColor(0.0, 0.0, 0.0, 1.0);

	//---------------------------------- RENDER IRRADIANCE CUBEMAP BY CONVOLUTING THE ENVIRONMENT MAP ----------------------------------

	FramebufferAttachment* irradianceCubemap = new FramebufferAttachment(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, FramebufferAttachment::Type::COLOUR_CUBEMAP, TextureParameters(GL_TEXTURE_CUBE_MAP, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true), 1);
	//environmentCubemap->getParameters().preventGenerateMipMaps(); //MUST NOT HAVE OTHERWISE CUBEMAP INCOMPLETE

	FBO* fboIrradianceMap = new FBO(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, {
		FramebufferAttachmentInfo{ irradianceCubemap, true, -1, false }//,
		//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
	});

	RenderPass* renderPassIrradianceMap = new RenderPass(fboIrradianceMap);
	GraphicsPipeline* pipelineIrradianceMap = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_IRRADIANCE_MAP), renderPassIrradianceMap, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
	DescriptorSet* descriptorSetIrradianceMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP));
	descriptorSetIrradianceMap->setTexture(0, environmentCubemap);

	descriptorSetIrradianceMap->getShaderBuffer(0)->updateFrame(&genEnvMapData, 0, sizeof(ShaderBlock_PBRGenEnvMap));

	descriptorSetIrradianceMap->setup();

	renderPassIrradianceMap->begin();
	pipelineIrradianceMap->bind();
	descriptorSetIrradianceMap->bind();

	cubeMesh->render();

	renderPassIrradianceMap->end();

	//--------------------- RENDER PREFILTER CUBEMAP BY CONVOLUTING THE ENVIRONMENT MAP (SPLIT SUM APPROXIMATION) ---------------------

	const unsigned int maxMipLevels = 5;

	FramebufferAttachment* prefilterCubemap = new FramebufferAttachment(PREFILTER_MAP_SIZE, PREFILTER_MAP_SIZE, FramebufferAttachment::Type::COLOUR_CUBEMAP, TextureParameters(GL_TEXTURE_CUBE_MAP, TextureParameters::Filter::LINEAR_MIPMAP_LINEAR, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true), 1, maxMipLevels);

	std::vector<ShaderBlock_PBRGenPrefilterMap> prefilterMapGenDatas;
	prefilterMapGenDatas.resize(maxMipLevels);
	std::vector<RenderPass*> renderPassesPrefilterMap;
	std::vector<GraphicsPipeline*> pipelinesPrefilterMap;
	std::vector<DescriptorSet*> descriptorSetsPrefilterMap;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
		unsigned int mipSize = PREFILTER_MAP_SIZE * pow(0.5, mip);

		FBO* fboPrefilterMap = new FBO(mipSize, mipSize, {
			FramebufferAttachmentInfo{ prefilterCubemap, true, static_cast<int>(mip), false }//,
			//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
		});

		RenderPass* renderPassPrefilterMap = new RenderPass(fboPrefilterMap);
		GraphicsPipeline* pipelinePrefilterMap = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_PREFILTER_MAP), renderPassPrefilterMap, mipSize, mipSize);

		prefilterMapGenDatas[mip].envMapSize = ENVIRONMENT_MAP_SIZE;
		prefilterMapGenDatas[mip].roughness = ((float) mip) / ((float) (maxMipLevels - 1));

		DescriptorSet* descriptorSetPrefilterMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_PREFILTER_MAP));
		descriptorSetPrefilterMap->getShaderBuffer(0)->updateFrame(&prefilterMapGenDatas[mip], 0, sizeof(ShaderBlock_PBRGenPrefilterMap));
		descriptorSetPrefilterMap->setup();

		renderPassesPrefilterMap.push_back(renderPassPrefilterMap);
		pipelinesPrefilterMap.push_back(pipelinePrefilterMap);
		descriptorSetsPrefilterMap.push_back(descriptorSetPrefilterMap);
	}

	DescriptorSet* descriptorSetPrefilterMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP));
	descriptorSetPrefilterMap->setTexture(0, environmentCubemap);

	for (unsigned int i = 0; i < maxMipLevels; ++i) {
		renderPassesPrefilterMap[i]->begin();

		pipelinesPrefilterMap[i]->bind();
		descriptorSetIrradianceMap->bind();
		descriptorSetsPrefilterMap[i]->bind();

		cubeMesh->render();

		renderPassesPrefilterMap[i]->end();
	}

	//-------------------------------------------------- RENDER BDRF INTEGRATION MAP --------------------------------------------------

	//Texture* brdfLUTTexture = new Texture(TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true));

	FramebufferAttachment* brdfLUTTexture = new FramebufferAttachment(BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE));

	FBO* fboBRDFLUTTexture = new FBO(BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE, {
		FramebufferAttachmentInfo{ brdfLUTTexture, true, -1, false }//,
		//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
	});

	RenderPass* renderPassBRDFLUTTexture = new RenderPass(fboBRDFLUTTexture);
	GraphicsPipeline* pipelineBRDFLUTTexture = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_BRDF_INTEGRATION_MAP), renderPassBRDFLUTTexture, BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE);

	renderPassBRDFLUTTexture->begin();
	pipelineBRDFLUTTexture->bind();

	quadMesh->render();

	renderPassBRDFLUTTexture->end();

	if (BaseEngine::usingVulkan()) {
		Vulkan::endSingleTimeCommands(vulkanCommandBuffer);
		Vulkan::setOverrideCommandBuffer(VK_NULL_HANDLE);
	}

	//---------------------------------------------------------------------------------------------------------------------------------

	delete cubeMesh;
	delete quadMesh;

	delete renderPassEquiToCubeMap;
	delete pipelineEquiToCubeMap;
	delete descriptorSetEquiToCubeMap;
	delete renderPassIrradianceMap;
	delete pipelineIrradianceMap;
	delete descriptorSetIrradianceMap;
	for (unsigned int i = 0; i < renderPassesPrefilterMap.size(); ++i)
		delete renderPassesPrefilterMap[i];
	for (unsigned int i = 0; i < pipelinesPrefilterMap.size(); ++i)
		delete pipelinesPrefilterMap[i];
	for (unsigned int i = 0; i < descriptorSetsPrefilterMap.size(); ++i)
		delete descriptorSetsPrefilterMap[i];
	delete descriptorSetPrefilterMap;
	delete renderPassBRDFLUTTexture;
	delete pipelineBRDFLUTTexture;

	return new PBREnvironment(environmentCubemap, irradianceCubemap, prefilterCubemap, brdfLUTTexture);
}
