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

#include "../../utils/Logging.h"

RenderScene::RenderScene() {
	//Obtain the render pipelines
	pipelineMaterial              = Renderer::getGraphicsPipeline(Renderer::GRAPHICS_PIPELINE_MATERIAL);
	pipelineLighting              = Renderer::getGraphicsPipeline(Renderer::GRAPHICS_PIPELINE_LIGHTING);
	pipelineLightingBlend         = Renderer::getGraphicsPipeline(Renderer::GRAPHICS_PIPELINE_LIGHTING_BLEND);
	pipelineLightingSkinning      = Renderer::getGraphicsPipeline(Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING);
	pipelineLightingSkinningBlend = Renderer::getGraphicsPipeline(Renderer::GRAPHICS_PIPELINE_LIGHTING_SKINNING_BLEND);
}

RenderScene::~RenderScene() {
	//Go through and delete all created objects
	for (DescriptorSet* descriptorSetLightBatch : descriptorSetLightBatches)
		delete descriptorSetLightBatch;
	for (unsigned int i = 0; i < objects.size(); ++i)
		delete objects[i];
	objects.clear();
	for (unsigned int i = 0; i < skinnedObjects.size(); ++i)
		delete skinnedObjects[i];
	skinnedObjects.clear();
	for (unsigned int i = 0; i < lights.size(); ++i)
		delete lights[i];
	lights.clear();
}

void RenderScene::add(GameObject3D* object) {
	//Add the object to the scene
	if (! object->getMesh()->hasSkeleton())
		objects.push_back(object);
	else
		skinnedObjects.push_back(object);
}

void RenderScene::addLight(Light* light) {
	lights.push_back(light);

	//Check if require new descriptor set for this
	if (lights.size() % NUM_LIGHTS_IN_BATCH == 1) {
		//Add a descriptor set for the new batch
		DescriptorSet* descriptorSetLightBatch = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_LIGHT_BATCH));
		descriptorSetLightBatch->setup();
		descriptorSetLightBatches.push_back(descriptorSetLightBatch);
	}
}

void RenderScene::render() {
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
			}

			//Assign the number of lights in the current batch
			shaderLightBatchData.ue_numLights = uniformNumLights;

			//Update the light batch UBO
			descriptorSetLightBatches[batchNumber]->getUBO(0)->updateFrame(&shaderLightBatchData, 0, sizeof(ShaderBlock_LightBatch));

			batchNumber++;
		}
		if (objects.size() > 0) {

			batchNumber = 0;

			pipelineLighting->bind();

			//Go through the each of the light batches
			for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {

				if (b == NUM_LIGHTS_IN_BATCH)
					//Start blending the results of other batches
					pipelineLightingBlend->bind();

				//Bind the descriptor set and render all of the objects
				descriptorSetLightBatches[batchNumber]->bind();

				for (unsigned int i = 0; i < objects.size(); ++i)
					objects[i]->render();

				batchNumber++;
			}
		}

		if (skinnedObjects.size() > 0) {

			batchNumber = 0;

			pipelineLightingSkinning->bind();

			//Go through the each of the light batches
			for (unsigned int b = 0; b < lights.size(); b += NUM_LIGHTS_IN_BATCH) {
				if (b == NUM_LIGHTS_IN_BATCH)
					//Start blending the results of other batches
					pipelineLightingSkinningBlend->bind();

				//Bind the descriptor set and render all of the objects
				descriptorSetLightBatches[batchNumber]->bind();

				for (unsigned int i = 0; i < skinnedObjects.size(); ++i)
					skinnedObjects[i]->render();

				batchNumber++;
			}
		}
	} else {
		//Use the material pipeline
		pipelineMaterial->bind();

		//Go through and render all of the objects
		for (unsigned int i = 0; i < objects.size(); ++i)
			objects[i]->render();
	}
}