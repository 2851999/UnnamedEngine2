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
	//Quick fix for no pipeline being bound to get primitive topology  from
	GraphicsPipeline* pipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_MATERIAL), Renderer::getDefaultRenderPass());
	pipeline->bind();

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

	//Get the shaders
	RenderShader* renderShader1 = Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_EQUI_TO_CUBE_MAP);
	RenderShader* renderShader2 = Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_IRRADIANCE_MAP);
	RenderShader* renderShader3 = Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_PREFILTER_MAP);
	RenderShader* renderShader4 = Renderer::getRenderShader(Renderer::SHADER_PBR_GEN_BRDF_INTEGRATION_MAP);

	Shader* shader1 = renderShader1->getShader();
	Shader* shader2 = renderShader2->getShader();
	Shader* shader3 = renderShader3->getShader();
	Shader* shader4 = renderShader4->getShader();

	//Create meshes to render a cubemap and 2D texture
	MeshData* cubeMeshData = MeshBuilder::createCube(1.0f, 1.0f, 1.0f);
	MeshData* quadMeshData = MeshBuilder::createQuad(Vector2f(-1.0f, -1.0f), Vector2f(1.0f, -1.0f), Vector2f(1.0f, 1.0f), Vector2f(-1.0f, 1.0f), NULL);

	MeshRenderData* cubeMesh = new MeshRenderData(cubeMeshData, renderShader1);
	MeshRenderData* quadMesh = new MeshRenderData(quadMeshData, renderShader4);
	std::vector<Material*> materials;
	cubeMesh->setup(cubeMeshData, materials);
	quadMesh->setup(quadMeshData, materials);

	//Load the texture for the equirectangular map ensuring it's the right way up
	Texture::setFlipVerticallyOnLoad(true);
	Texture* texture = Texture::loadTexturef(path, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true));
	Texture::setFlipVerticallyOnLoad(false);

	//---------------------------------- RENDER ENVIRONMENT CUBEMAP FROM EQUIRECTANGULAR MAP ----------------------------------

	FramebufferAttachment* environmentCubemap = new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::COLOUR_CUBEMAP);
	environmentCubemap->getParameters().setMinFilter(TextureParameters::Filter::LINEAR_MIPMAP_LINEAR);
	//environmentCubemap->getParameters().setMinFilter(TextureParameters::Filter::LINEAR);
	environmentCubemap->getParameters().setMagFilter(TextureParameters::Filter::LINEAR);
	environmentCubemap->getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
	//environmentCubemap->getParameters().preventGenerateMipMaps(); //MUST NOT HAVE OTHERWISE CUBEMAP INCOMPLETE

	FBO* fboEquiToCubemap = new FBO(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, {
		FramebufferAttachmentInfo{ environmentCubemap, true, false }//,
		//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
	});

	RenderPass* renderPassEquiToCubeMap = new RenderPass(fboEquiToCubemap);
	GraphicsPipeline* pipelineEquiToCubeMap = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_EQUI_TO_CUBE_MAP), renderPassEquiToCubeMap, ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE);
	DescriptorSet* descriptorSetEquiToCubeMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_EQUI_TO_CUBE_MAP));
	descriptorSetEquiToCubeMap->setTexture(0, texture);

	ShaderBlock_PBRGenEnvMap genEnvMapData;
	for (unsigned int i = 0; i < 6; ++i)
		genEnvMapData.projectionViewMatrices[i] = captureProjection * captureViews[i];
	descriptorSetEquiToCubeMap->getUBO(0)->updateFrame(&genEnvMapData, 0, sizeof(ShaderBlock_PBRGenEnvMap));

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
	environmentCubemap->bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//glClearColor(0.0, 0.0, 0.0, 1.0);

	//---------------------------------- RENDER IRRADIANCE CUBEMAP BY CONVOLUTING THE ENVIRONMENT MAP ----------------------------------

	FramebufferAttachment* irradianceCubemap = new FramebufferAttachment(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, FramebufferAttachment::Type::COLOUR_CUBEMAP);
	irradianceCubemap->getParameters().setFilter(TextureParameters::Filter::LINEAR);
	irradianceCubemap->getParameters().setAddressMode(TextureParameters::AddressMode::CLAMP_TO_EDGE);
	//environmentCubemap->getParameters().preventGenerateMipMaps(); //MUST NOT HAVE OTHERWISE CUBEMAP INCOMPLETE

	FBO* fboIrradianceMap = new FBO(IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, {
		FramebufferAttachmentInfo{ irradianceCubemap, true, false }//,
		//FramebufferAttachmentInfo{ new FramebufferAttachment(ENVIRONMENT_MAP_SIZE, ENVIRONMENT_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP), true }
	});

	RenderPass* renderPassIrradianceMap = new RenderPass(fboIrradianceMap);
	GraphicsPipeline* pipelineIrradianceMap = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PBR_GEN_IRRADIANCE_MAP), renderPassEquiToCubeMap, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
	DescriptorSet* descriptorSetIrradianceMap = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_GEN_IRRADIANCE_MAP));
	descriptorSetIrradianceMap->setTexture(0, environmentCubemap);

	descriptorSetIrradianceMap->getUBO(0)->updateFrame(&genEnvMapData, 0, sizeof(ShaderBlock_PBRGenEnvMap));

	descriptorSetIrradianceMap->setup();

	renderPassIrradianceMap->begin();
	pipelineIrradianceMap->bind();
	descriptorSetIrradianceMap->bind();

	cubeMesh->render();

	renderPassIrradianceMap->end();

	//--------------------- RENDER PREFILTER CUBEMAP BY CONVOLUTING THE ENVIRONMENT MAP (SPLIT SUM APPROXIMATION) ---------------------

	//Create and setup the FBO and RBO for rendering
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, maxSize, maxSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	ShaderBlock_GenPBREnvMap envMapGenData;
	UBO* envMapGenUBO = new UBO(NULL, sizeof(ShaderBlock_GenPBREnvMap), DataUsage::DYNAMIC, ShaderInterface::BLOCK_PBR_ENV_MAP_GEN);
	envMapGenUBO->bindGL();

	TextureParameters prefilMapParameters = TextureParameters(GL_TEXTURE_CUBE_MAP, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true);
	prefilMapParameters.setMinFilter(TextureParameters::Filter::LINEAR_MIPMAP_LINEAR);
	prefilMapParameters.preventGenerateMipMaps();
	Cubemap* prefilterCubemap = Cubemap::createCubemap(PREFILTER_MAP_SIZE, GL_RGB16F, GL_RGB, GL_FLOAT, prefilMapParameters);

	ShaderBlock_GenPBRPrefilterMap prefilterMapGenData;
	UBO* prefilterMapGenUBO = new UBO(NULL, sizeof(ShaderBlock_GenPBRPrefilterMap), DataUsage::DYNAMIC, ShaderInterface::BLOCK_PBR_PREFILTER_MAP_GEN);
	prefilterMapGenUBO->bindGL();

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, Window::getCurrentInstance()->getSettings().videoMaxAnisotropicSamples);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); //Allocate required memory

	prefilterCubemap->unbind();

	shader3->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentCubemap->getHandle());
	shader3->setUniformi("EnvMap", 0);
	envMapGenData.projection = captureProjection;
	prefilterMapGenData.envMapSize = ENVIRONMENT_MAP_SIZE;

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	const unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; mip++) {
		//Resize framebuffer based on number of levels
		unsigned int mipWidth = PREFILTER_MAP_SIZE * pow(0.5, mip);
		unsigned int mipHeight = mipWidth;

		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float) mip / (float) (maxMipLevels - 1);
		prefilterMapGenData.roughness = roughness;
		prefilterMapGenUBO->update(&prefilterMapGenData, 0, sizeof(ShaderBlock_GenPBRPrefilterMap));
		for (unsigned int i = 0; i < 6; i++) {
			envMapGenData.view = captureViews[i];
			envMapGenUBO->update(&envMapGenData, 0, sizeof(ShaderBlock_GenPBREnvMap));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterCubemap->getHandle(), mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cubeMesh->render();
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	shader3->stopUsing();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-------------------------------------------------- RENDER BDRF INTEGRATION MAP --------------------------------------------------

	Texture* brdfLUTTexture = new Texture(TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::LINEAR, TextureParameters::AddressMode::CLAMP_TO_EDGE, true));
	brdfLUTTexture->bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, 0);

	brdfLUTTexture->applyParameters(false);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture->getHandle(), 0);

	glViewport(0, 0, BRDF_LUT_TEXTURE_SIZE, BRDF_LUT_TEXTURE_SIZE);
	shader4->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadMesh->render();

	brdfLUTTexture->unbind();

	shader4->stopUsing();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//---------------------------------------------------------------------------------------------------------------------------------

	//Reset the normal view port
	glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);

	//Delete the resources created
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);

	delete cubeMesh;
	delete quadMesh;

	delete pipeline;

	return new PBREnvironment(environmentCubemap, irradianceCubemap, prefilterCubemap, brdfLUTTexture);
}
