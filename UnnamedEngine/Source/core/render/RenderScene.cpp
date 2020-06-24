/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "RenderScene.h"

#include "../vulkan/Vulkan.h"
#include "RenderPass.h"

#include "../../utils/Logging.h"

#include "../BaseEngine.h"

RenderScene::RenderScene(bool deferred, bool pbr, bool ssr, bool bloom, bool postProcessing, PBREnvironment* pbrEnvironment) : deferred(deferred), pbr(pbr), ssr(ssr), bloom(bloom), postProcessing(postProcessing), pbrEnvironment(pbrEnvironment) {
	//Create the FBO for rendering offscreen
	uint32_t width = Window::getCurrentInstance()->getSettings().windowWidth;
	uint32_t height = Window::getCurrentInstance()->getSettings().windowHeight;

	//Setup for post processing if needed
	if (postProcessing) {
		descriptorSetGammaCorrectionFXAA = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_GAMMA_CORRECTION_FXAA));
		descriptorSetGammaCorrectionFXAA->setup();

		//Assign the default values
		shaderGammaCorrectionFXAAData.inverseTextureSize = Vector2f(1.0f / Window::getCurrentInstance()->getSettings().windowWidth, 1.0f / Window::getCurrentInstance()->getSettings().windowHeight);
		shaderGammaCorrectionFXAAData.gammaCorrect       = false;
		shaderGammaCorrectionFXAAData.exposureIn         = -1;
		shaderGammaCorrectionFXAAData.fxaa               = false;

		descriptorSetGammaCorrectionFXAA->getUBO(0)->update(&shaderGammaCorrectionFXAAData, 0, sizeof(ShaderBlock_GammaCorrectionFXAA));

		//Setup the offscreen render pass
		FBO* fbo = new FBO(width, height, {
			FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
			FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), ! deferred } //When deferred, geometry pass must clear depth buffer, not post process
		});

		postProcessingRenderPass = new RenderPass(fbo);

		//Obtain the pipeline
		pipelineGammaCorrectionFXAA = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_GAMMA_CORRECTION_FXAA), Renderer::getDefaultRenderPass());

		//Setup the screen texture mesh
		MeshData* meshData = createScreenMeshData();
		screenTextureMesh = new Mesh(meshData);
		screenTextureMesh->getMaterial()->setDiffuse(postProcessingRenderPass->getFBO()->getAttachment(0));

		screenTextureMesh->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
	}

	if (pbr)
		//Default ambient light parameter for PBR
		ambientLight = Colour(0.03f, 0.03f, 0.03f);

	//Setup for deferred rendering if needed
	if (deferred) {
		descriptorSetGeometryBuffer = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(pbr ? ShaderInterface::DESCRIPTOR_SET_DEFAULT_BASIC_PBR_DEFERRED_LIGHTING : ShaderInterface::DESCRIPTOR_SET_DEFAULT_DEFERRED_LIGHTING));

		//Setup the geometry render pass
		FBO* fbo;
		if (pbr) {
			fbo = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true }
			});
		} else {
			fbo = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true }
			});
		}

		deferredGeometryRenderPass = new RenderPass(fbo);

		descriptorSetGeometryBuffer->setTexture(0, deferredGeometryRenderPass->getFBO()->getAttachment(0));
		descriptorSetGeometryBuffer->setTexture(1, deferredGeometryRenderPass->getFBO()->getAttachment(1));
		descriptorSetGeometryBuffer->setTexture(2, deferredGeometryRenderPass->getFBO()->getAttachment(2));
		if (pbr)
			descriptorSetGeometryBuffer->setTexture(3, deferredGeometryRenderPass->getFBO()->getAttachment(3));
		descriptorSetGeometryBuffer->setup();

		//Setup the screen texture mesh
		MeshData* meshData = createScreenMeshData();
		deferredRenderingScreenTextureMesh = new Mesh(meshData);
		deferredRenderingScreenTextureMesh->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

		if (bloom) {
			FBO* bloomFBO = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },
				FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), false }
			});

			deferredBloomRenderPass = new RenderPass(bloomFBO);

			FBO* gaussianBlur1FBO = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true }//,
				//FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), false }
			});

			FBO* gaussianBlur2FBO = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true }//,
				//FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), false }
			});

			gaussianBlur1RenderPass = new RenderPass(gaussianBlur1FBO);
			gaussianBlur2RenderPass = new RenderPass(gaussianBlur2FBO);

			pipelineGaussianBlur1 = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_GAUSSIAN_BLUR), gaussianBlur1RenderPass);
			pipelineGaussianBlur2 = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_GAUSSIAN_BLUR), gaussianBlur2RenderPass);

			//Setup the screen texture meshes
			MeshData* meshData1 = createScreenMeshData();
			gaussianBlurBloomScreenTextureMesh1 = new Mesh(meshData1);
			gaussianBlurBloomScreenTextureMesh1->getMaterial()->setDiffuse(deferredBloomRenderPass->getFBO()->getAttachment(1)); //Bright texture

			gaussianBlurBloomScreenTextureMesh1->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

			MeshData* meshData2 = createScreenMeshData();
			gaussianBlurBloomScreenTextureMesh2 = new Mesh(meshData2);
			gaussianBlurBloomScreenTextureMesh2->getMaterial()->setDiffuse(gaussianBlur1RenderPass->getFBO()->getAttachment(0)); //Bright texture

			gaussianBlurBloomScreenTextureMesh2->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

			MeshData* meshData3 = createScreenMeshData();
			gaussianBlurBloomScreenTextureMesh3 = new Mesh(meshData3);
			gaussianBlurBloomScreenTextureMesh3->getMaterial()->setDiffuse(gaussianBlur2RenderPass->getFBO()->getAttachment(0)); //Bright texture

			gaussianBlurBloomScreenTextureMesh3->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

			gaussianBlurData[0].horizontal = true;
			gaussianBlurData[1].horizontal = false;

			descriptorSetsGaussianBlur[0] = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_GAUSSIAN_BLUR));
			descriptorSetsGaussianBlur[0]->getUBO(0)->update(&gaussianBlurData[0], 0, sizeof(ShaderBlock_GaussianBlur));
			descriptorSetsGaussianBlur[0]->setup();

			descriptorSetsGaussianBlur[1] = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_GAUSSIAN_BLUR));
			descriptorSetsGaussianBlur[1]->getUBO(0)->update(&gaussianBlurData[1], 0, sizeof(ShaderBlock_GaussianBlur));
			descriptorSetsGaussianBlur[1]->setup();

			MeshData* meshData4 = createScreenMeshData();
			bloomSSRScreenTextureMesh = new Mesh(meshData4);
			bloomSSRScreenTextureMesh->getMaterial()->setAmbient(((gaussianBlurAmount % 2) == 1) ? gaussianBlur1RenderPass->getFBO()->getAttachment(0) : gaussianBlur2RenderPass->getFBO()->getAttachment(0));
			bloomSSRScreenTextureMesh->getMaterial()->setDiffuse(deferredBloomRenderPass->getFBO()->getAttachment(0));

			bloomSSRScreenTextureMesh->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
		}

		if (ssr) {

			FBO* ssrFBO = new FBO(width, height, {
				FramebufferAttachmentInfo{ new FramebufferAttachment(width, height, FramebufferAttachment::Type::COLOUR_TEXTURE, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), true },//
				//FramebufferAttachmentInfo{ BaseEngine::usingVulkan() ? Vulkan::getSwapChain()->getDepthAttachment() : new FramebufferAttachment(width, height, FramebufferAttachment::Type::DEPTH, TextureParameters(GL_TEXTURE_2D, TextureParameters::Filter::NEAREST, TextureParameters::AddressMode::CLAMP_TO_EDGE)), false }
			});

			deferredPBRSSRRenderPass = new RenderPass(ssrFBO);

			descriptorSetGeometryBufferSSR = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_DEFERRED_PBR_SSR));
			descriptorSetGeometryBufferSSR->setTexture(0, deferredGeometryRenderPass->getFBO()->getAttachment(0));
			descriptorSetGeometryBufferSSR->setTexture(1, deferredGeometryRenderPass->getFBO()->getAttachment(1));
			descriptorSetGeometryBufferSSR->setTexture(2, deferredPBRSSRRenderPass->getFBO()->getAttachment(0)); //Albedo
			descriptorSetGeometryBufferSSR->setTexture(3, deferredGeometryRenderPass->getFBO()->getAttachment(3));
			descriptorSetGeometryBufferSSR->setup();

			pipelineDeferredSSR = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_DEFERRED_PBR_SSR), postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass());
		}

		if (bloom) {
			pipelineDeferredLighting = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(pbrEnvironment ? Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLOOM : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLOOM), deferredBloomRenderPass);
			pipelineDeferredLightingBlend = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(pbrEnvironment ? Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLOOM_BLEND : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLOOM_BLEND), deferredBloomRenderPass);

			pipelineBloomCombine = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_BLOOM), ssr ? deferredPBRSSRRenderPass : (postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass()));
		} else {
			pipelineDeferredLighting = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(pbr ? (pbrEnvironment ? Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING) : Renderer::GRAPHICS_PIPELINE_DEFERRED_LIGHTING), ssr ? deferredPBRSSRRenderPass : (postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass()));
			pipelineDeferredLightingBlend = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(pbr ? (pbrEnvironment ? Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_BLEND : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_BLEND) : Renderer::GRAPHICS_PIPELINE_DEFERRED_LIGHTING_BLEND), ssr ? deferredPBRSSRRenderPass : (postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass()));
		}
	}

	if (pbrEnvironment) {
		descriptorSetPBREnvironment = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(deferred ? ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT : ShaderInterface::DESCRIPTOR_SET_DEFAULT_PBR_ENVIRONMENT_NO_DEFERRED));
		descriptorSetPBREnvironment->setTexture(0, pbrEnvironment->getIrradianceCubemap());
		descriptorSetPBREnvironment->setTexture(1, pbrEnvironment->getPrefilterCubemap());
		descriptorSetPBREnvironment->setTexture(2, pbrEnvironment->getBRDFLUTTexture());
		descriptorSetPBREnvironment->setup();
	}
}

RenderScene::~RenderScene() {
	if (postProcessing) {
		delete descriptorSetGammaCorrectionFXAA;
		delete postProcessingRenderPass;
		delete pipelineGammaCorrectionFXAA;
		delete screenTextureMesh;
	}

	if (deferred) {
		delete pipelineDeferredLighting;
		delete pipelineDeferredLightingBlend;

		delete descriptorSetGeometryBuffer;
		delete deferredGeometryRenderPass;
		delete deferredRenderingScreenTextureMesh;

		if (bloom) {
			delete pipelineBloomCombine;

			delete deferredBloomRenderPass;
			delete gaussianBlur1RenderPass;
			delete gaussianBlur2RenderPass;
			delete pipelineGaussianBlur1;
			delete pipelineGaussianBlur2;
			delete gaussianBlurBloomScreenTextureMesh1;
			delete gaussianBlurBloomScreenTextureMesh2;
			delete gaussianBlurBloomScreenTextureMesh3;
			delete descriptorSetsGaussianBlur[0];
			delete descriptorSetsGaussianBlur[1];
			delete bloomSSRScreenTextureMesh;
		}

		if (ssr) {
			delete deferredPBRSSRRenderPass;
			delete descriptorSetGeometryBufferSSR;
			delete pipelineDeferredSSR;
		}
	}

	if (pbrEnvironment) {
		delete descriptorSetPBREnvironment;
		delete pbrEnvironment;
	}

	//Go through and delete all created objects
	for (DescriptorSet* descriptorSetLightBatch : descriptorSetLightBatches)
		delete descriptorSetLightBatch;
	for (auto& element : objectBatches) {
		for (unsigned int i = 0; i < element.second.objects.size(); ++i)
			delete element.second.objects[i];
		delete element.second.graphicsPipeline;
		if (element.second.graphicsPipelineBlend)
			delete element.second.graphicsPipelineBlend;
	}
	for (unsigned int i = 0; i < lights.size(); ++i)
		delete lights[i];
	lights.clear();
}

void RenderScene::add(GameObject3D* object) {
	//Determine the pipeline needed for rendering
	unsigned int graphicsPipelineID      = 0;
	unsigned int graphicsPipelineBlendID = 0;

	bool skinning = object->getMesh()->hasSkeleton();

	if (object->getRenderShader()->getID() == Renderer::SHADER_TERRAIN) {
		if (deferred)
			graphicsPipelineID = Renderer::GRAPHICS_PIPELINE_DEFERRED_TERRAIN_GEOMETRY;
		else {
			graphicsPipelineID = Renderer::GRAPHICS_PIPELINE_TERRAIN;
			graphicsPipelineBlendID = Renderer::GRAPHICS_PIPELINE_TERRAIN_BLEND;
		}
	} else {
		if (lighting) {
			if (pbr) {
				if (pbrEnvironment) { //IBL
					if (deferred)
						graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY : Renderer::GRAPHICS_PIPELINE_PBR_DEFERRED_LIGHTING_GEOMETRY;
					else {
						graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING : Renderer::GRAPHICS_PIPELINE_PBR_LIGHTING;
						graphicsPipelineBlendID = skinning ? Renderer::GRAPHICS_PIPELINE_PBR_LIGHTING_SKINNING_BLEND : Renderer::GRAPHICS_PIPELINE_PBR_LIGHTING_BLEND;
					}
				} else { //No IBL
					if (deferred)
						graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_SKINNING_GEOMETRY : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_DEFERRED_LIGHTING_GEOMETRY;
					else {
						graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING;
						graphicsPipelineBlendID = skinning ? Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_SKINNING_BLEND : Renderer::GRAPHICS_PIPELINE_BASIC_PBR_LIGHTING_BLEND;
					}
				}
			} else {
				if (deferred)
					graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_DEFERRED_LIGHTING_SKINNING_GEOMETRY : Renderer::GRAPHICS_PIPELINE_DEFERRED_LIGHTING_GEOMETRY;
				else {
					graphicsPipelineID      = skinning ? Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING : Renderer::GRAPHICS_PIPELINE_LIGHTING;
					graphicsPipelineBlendID = skinning ? Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND : Renderer::GRAPHICS_PIPELINE_LIGHTING_BLEND;
				}
			}
		} else
			graphicsPipelineID = Renderer::GRAPHICS_PIPELINE_MATERIAL;
	}

	//Check if a batch already exists for it
	if (objectBatches.find(graphicsPipelineID) != objectBatches.end()) {
		objectBatches[graphicsPipelineID].objects.push_back(object);
	} else {
		ObjectBatch batch;
		batch.skinning = skinning;
		batch.graphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(graphicsPipelineID), deferred ? deferredGeometryRenderPass : (postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass()));
		if (graphicsPipelineBlendID != 0)
			batch.graphicsPipelineBlend = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(graphicsPipelineBlendID), deferred ? deferredGeometryRenderPass : (postProcessing ? postProcessingRenderPass : Renderer::getDefaultRenderPass()));
		batch.objects.push_back(object);

		objectBatches.insert(std::pair<unsigned int, ObjectBatch>(graphicsPipelineID, batch));
	}
}

void RenderScene::addLight(Light* light) {
	lights.push_back(light);

	//Check if require new descriptor set for this
	if ((lights.size() % NUM_LIGHTS_IN_BATCH) == 1) {
		//Add a descriptor set for the new batch
		DescriptorSet* descriptorSetLightBatch = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));

		descriptorSetLightBatch->setup();
		descriptorSetLightBatches.push_back(descriptorSetLightBatch);
	}
	//Check if the light added has a shadow map
	if (light->hasShadowMap()) {
		//Obtain the light index within the current batch
		unsigned int indexInBatch = (lights.size() - 1) % NUM_LIGHTS_IN_BATCH;

		if (light->getType() == Light::TYPE_POINT)
			indexInBatch += 6;

		//Assign the shadow map
		descriptorSetLightBatches[descriptorSetLightBatches.size() - 1]->setTexture(indexInBatch, light->getShadowMapRenderPass()->getFBO()->getAttachment(0));
		//Update the descriptor set
		descriptorSetLightBatches[descriptorSetLightBatches.size() - 1]->update();
	}
}

void RenderScene::renderOffscreen() {
	//Check for lighting
	if (lighting) {
		//Go through and render any shadow maps needed
		for (unsigned int i = 0; i < lights.size(); ++i) {
			if (lights[i]->hasShadowMap()) {
				//Obtain the render pass
				RenderPass* shadowMapRenderPass = lights[i]->getShadowMapRenderPass();

				shadowMapRenderPass->begin();

				//vkCmdSetDepthBias(
				//	Vulkan::getCurrentCommandBuffer(),
				//	1.25f,
				//	0.0f,
				//	1.75f);

				for (auto& batch : objectBatches) {
					if (batch.second.skinning)
						lights[i]->getShadowMapSkinningGraphicsPipeline()->bind();
					else
						lights[i]->getShadowMapGraphicsPipeline()->bind();

					//Use the light's view
					lights[i]->useView();

					for (unsigned int j = 0; j < batch.second.objects.size(); ++j) {
						//if (batch.second.objects[j]->getRenderShader()->getID() != Renderer::SHADER_TERRAIN)
						batch.second.objects[j]->render();
					}
				}

				shadowMapRenderPass->end();
			}
		}
	}

	//Check if deferred rendering
	if (deferred) {
		//Render to the geometry buffer
		deferredGeometryRenderPass->begin();

		for (auto& batch : objectBatches) {
			batch.second.graphicsPipeline->bind();

			((Camera3D*) Renderer::getCamera())->useView();

			for (unsigned int i = 0; i < batch.second.objects.size(); ++i)
				batch.second.objects[i]->render();
		}

		deferredGeometryRenderPass->end();

		if (bloom) {
			//Render the lighting
			deferredBloomRenderPass->begin();

			renderScene();

			deferredBloomRenderPass->end();

			//Perform blur
			bool firstIteration;
			unsigned int currentIndex = 0;
			RenderPass* renderPassCurrent = gaussianBlur1RenderPass;
			GraphicsPipeline* pipelineCurrent = pipelineGaussianBlur1;
			DescriptorSet* descriptorSetCurrent = descriptorSetsGaussianBlur[0];
			Mesh* screenTextureMeshCurrent = gaussianBlurBloomScreenTextureMesh1;

			Matrix4f identityMatrix = Matrix4f().initIdentity();

			for (unsigned int i = 0; i < gaussianBlurAmount; ++i) {

				renderPassCurrent->begin();
				pipelineCurrent->bind();
				descriptorSetCurrent->bind();

				Renderer::render(screenTextureMeshCurrent, identityMatrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

				renderPassCurrent->end();

				currentIndex++;

				if (currentIndex >= 2)
					currentIndex = 0;

				if (currentIndex == 0) {
					renderPassCurrent = gaussianBlur1RenderPass;
					pipelineCurrent = pipelineGaussianBlur1;
					descriptorSetCurrent = descriptorSetsGaussianBlur[0];
					screenTextureMeshCurrent = gaussianBlurBloomScreenTextureMesh3;
				} else if (currentIndex == 1) {
					renderPassCurrent = gaussianBlur2RenderPass;
					pipelineCurrent = pipelineGaussianBlur2;
					descriptorSetCurrent = descriptorSetsGaussianBlur[1];
					screenTextureMeshCurrent = gaussianBlurBloomScreenTextureMesh2;
				}
			}

			if (ssr) {
				deferredPBRSSRRenderPass->begin();

				pipelineBloomCombine->bind();

				Renderer::render(bloomSSRScreenTextureMesh, identityMatrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

				deferredPBRSSRRenderPass->end();
			}
		} else if (ssr) {
			//Render the lighting
			deferredPBRSSRRenderPass->begin();

			renderScene();

			deferredPBRSSRRenderPass->end();
		}
	}

	//Check if post processing
	if (postProcessing) {
		//Render the scene offscreen ready for post processing
		postProcessingRenderPass->begin();

		if (ssr) {
			pipelineDeferredSSR->bind();
			descriptorSetGeometryBufferSSR->bind();
			Matrix4f identityMatrix = Matrix4f().initIdentity();
			Renderer::render(deferredRenderingScreenTextureMesh, identityMatrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
		} else if (bloom) {
			pipelineBloomCombine->bind();
			Matrix4f identityMatrix = Matrix4f().initIdentity();
			Renderer::render(bloomSSRScreenTextureMesh, identityMatrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
		} else {
			renderScene();
		}

		postProcessingRenderPass->end();
	}
}

void RenderScene::renderScene() {
	((Camera3D*) Renderer::getCamera())->useView();

	//Check whether lighting is enabled
	if (lighting) {
		//Ambient light (used for phong shading)
		shaderLightBatchData.ue_lightAmbient = ambientLight;

		//Number of lights in the current batch (assigned later)
		unsigned int uniformNumLights;

		//Current batch number
		unsigned int batchNumber = 0;

		//Go through the each of the light batches
		for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {
			//Calculate the number of lights in this set
			uniformNumLights = utils_maths::min<unsigned int>(NUM_LIGHTS_IN_BATCH, lights.size() - b);

			if (b == NUM_LIGHTS_IN_BATCH)
				//Ambient light applied, so should remove it so not applied more than once
				shaderLightBatchData.ue_lightAmbient = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

			//The index of the light in the current batch (as it will appear in the shader)
			unsigned int lightIndexInBatch;

			//Go through each light in the current batch
			for (unsigned int l = b; (l < b + NUM_LIGHTS_IN_BATCH) && (l < lights.size()); ++l) {
				//Compute the light index as it will be seen in the shader
				lightIndexInBatch = l - b;

				//Assign the data for the current light
				lights[l]->setUniforms(shaderLightBatchData.ue_lights[lightIndexInBatch]);

				if (lights[l]->hasShadowMap())
					//Assign the light space matrix as well
					shaderLightBatchData.ue_lightSpaceMatrix[lightIndexInBatch] = lights[l]->getLightSpaceMatrix();
			}

			//Assign the number of lights in the current batch
			shaderLightBatchData.ue_numLights = uniformNumLights;

			//Update the light batch UBO
			descriptorSetLightBatches[batchNumber]->getUBO(0)->updateFrame(&shaderLightBatchData, 0, sizeof(ShaderBlock_LightBatch));

			batchNumber++;
		}
		if (deferred) {
			batchNumber = 0;

			pipelineDeferredLighting->bind();

			//Bind the PBREnvironment textures if needed
			if (pbrEnvironment)
				descriptorSetPBREnvironment->bind();

			descriptorSetGeometryBuffer->bind();

			//Go through the each of the light batches
			for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {

				if (b == NUM_LIGHTS_IN_BATCH)
					//Start blending the results of other batches
					pipelineDeferredLightingBlend->bind();

				//Bind the descriptor set and render all of the objects
				descriptorSetLightBatches[batchNumber]->bind();

				//Render to the screen
				Matrix4f matrix = Matrix4f().initIdentity();
				Renderer::render(deferredRenderingScreenTextureMesh, matrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

				batchNumber++;
			}
		} else {
			for (auto& batch : objectBatches) {

				batchNumber = 0;

				batch.second.graphicsPipeline->bind();

				//Bind the PBREnvironment textures if needed
				if (pbrEnvironment)
					descriptorSetPBREnvironment->bind();

				//Go through the each of the light batches
				for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {

					if (b == NUM_LIGHTS_IN_BATCH)
						//Start blending the results of other batches
						batch.second.graphicsPipelineBlend->bind();

					//Bind the descriptor set and render all of the objects
					descriptorSetLightBatches[batchNumber]->bind();

					for (unsigned int i = 0; i < batch.second.objects.size(); ++i)
						batch.second.objects[i]->render();

					batchNumber++;
				}
			}
		}
	} else {
		//Go through and render all of the objects
		for (auto& batch : objectBatches) {
			batch.second.graphicsPipeline->bind();

			for (unsigned int i = 0; i < batch.second.objects.size(); ++i)
				batch.second.objects[i]->render();
		}
	}
}

void RenderScene::render() {
	if (postProcessing) {
		pipelineGammaCorrectionFXAA->bind();
		descriptorSetGammaCorrectionFXAA->bind();
		Matrix4f matrix = Matrix4f().initIdentity();
		Renderer::render(screenTextureMesh, matrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));

		if (! BaseEngine::usingVulkan()) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, deferred ? deferredGeometryRenderPass->getFBO()->getGLFBO() : postProcessingRenderPass->getFBO()->getGLFBO());
			//glDrawBuffer(GL_BACK);
			glBlitFramebuffer(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	} else {
		if (deferred && (! BaseEngine::usingVulkan())) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, deferred ? deferredGeometryRenderPass->getFBO()->getGLFBO() : postProcessingRenderPass->getFBO()->getGLFBO());
			//glDrawBuffer(GL_BACK);
			glBlitFramebuffer(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		if (ssr) {
			pipelineDeferredSSR->bind();
			descriptorSetGeometryBufferSSR->bind();
			Matrix4f matrix = Matrix4f().initIdentity();
			Renderer::render(deferredRenderingScreenTextureMesh, matrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
		}  else if (bloom) {
			pipelineBloomCombine->bind();
			Matrix4f identityMatrix = Matrix4f().initIdentity();
			Renderer::render(bloomSSRScreenTextureMesh, identityMatrix, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
		} else {
			renderScene();
		}
	}
}

void RenderScene::setPostProcessingParameters(bool gammaCorrection, bool fxaa, float exposureIn) {
	//Assign the parameters and update the UBO
	shaderGammaCorrectionFXAAData.gammaCorrect = gammaCorrection;
	shaderGammaCorrectionFXAAData.fxaa         = fxaa;
	shaderGammaCorrectionFXAAData.exposureIn   = exposureIn;

	descriptorSetGammaCorrectionFXAA->getUBO(0)->update(&shaderGammaCorrectionFXAAData, 0, sizeof(ShaderBlock_GammaCorrectionFXAA));
}

MeshData* RenderScene::createScreenMeshData() {
	MeshData* meshData = new MeshData(MeshData::DIMENSIONS_2D);
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(-1.0f, -1.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, 1.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
	return meshData;
}