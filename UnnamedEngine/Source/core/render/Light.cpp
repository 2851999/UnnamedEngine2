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

#include "Light.h"

#include "FBO.h"
#include "RenderPass.h"

/*****************************************************************************
 * The Light class
 *****************************************************************************/

Light::Light(unsigned int type, Vector3f position, bool castShadows) : type(type) {
	setPosition(position);

	if (castShadows) {
		if (type == TYPE_DIRECTIONAL) {
			//Create FBO
			FBO* fbo = new FBO(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, {
					//new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::COLOUR_TEXTURE),
					new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::DEPTH_TEXTURE)
				});

			//Create the shadow map render pass
			shadowMapRenderPass = new RenderPass(fbo);

			//Create the graphics pipelines
			shadowMapGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_MAP), shadowMapRenderPass);
			shadowMapSkinningGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING), shadowMapRenderPass);

			setProjectionMatrix(Matrix4f().initOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f));
		} else if (type == TYPE_POINT) {
			//Create FBO
			FBO* fbo = new FBO(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, {
				//new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::COLOUR_TEXTURE),
				new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::DEPTH_CUBEMAP)
			});

			//Create the shadow map render pass
			shadowMapRenderPass = new RenderPass(fbo);

			//Create the graphics pipelines
			shadowMapGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_CUBEMAP), shadowMapRenderPass);
			shadowMapSkinningGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_CUBEMAP_SKINNING), shadowMapRenderPass);

			//Setup the shadow transforms for each face of the cubemap
			for (unsigned int i = 0; i < 6; ++i)
				lightShadowTransforms.push_back(Matrix4f());

			//Setup the projection matrix for the shadow map rendering
			setProjectionMatrix(Matrix4f().initPerspective(90.0f, (float) SHADOW_MAP_SIZE / (float) SHADOW_MAP_SIZE, 1.0f, 25.0f));

			//Create the shadow cubemap descriptor set
			shadowCubemapDescriptorSet = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_SHADOW_CUBEMAP));
			shadowCubemapDescriptorSet->setup();
		} else if (type == TYPE_SPOT) {
			//Create FBO
			FBO* fbo = new FBO(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, {
					//new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::COLOUR_TEXTURE),
					new FramebufferAttachment(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, FramebufferAttachment::Type::DEPTH_TEXTURE)
				});

			//Create the shadow map render pass
			shadowMapRenderPass = new RenderPass(fbo);

			//Create the graphics pipeline
			shadowMapGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_MAP), shadowMapRenderPass);
			shadowMapSkinningGraphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_SHADOW_MAP_SKINNING), shadowMapRenderPass);

			//Setup the projection matrix for the shadow map rendering
			setProjectionMatrix(Matrix4f().initPerspective(90.0f, (float) SHADOW_MAP_SIZE / (float) SHADOW_MAP_SIZE, 1.0f, 25.0f));
		}
		//Update this light (in case it is static)
		update();
	}
}

Light::~Light() {
	if (hasShadowMap()) {
		delete shadowMapRenderPass;
		delete shadowMapGraphicsPipeline;
		delete shadowMapSkinningGraphicsPipeline;
		if (shadowCubemapDescriptorSet)
			delete shadowCubemapDescriptorSet;
	}
}

void Light::update() {
	//Check the type of light this is
	if (type == TYPE_DIRECTIONAL && shadowMapRenderPass) {
		Vector3f right = direction.cross(Vector3f(0.0f, 1.0f, 0.0f)).normalise();
		Vector3f up = right.cross(direction).normalise();

		getViewMatrix().initLookAt(direction * -5, (direction * 5) + direction, up);

		lightProjectionView = getProjectionMatrix() * getViewMatrix();

		//Update the frustum
		frustum.update(lightProjectionView);

		setViewMatrix(lightProjectionView); //Shortcut

		//Update the UBO
		updateUBO();
	} else if (type == TYPE_POINT && shadowCubemapDescriptorSet) { //SHOULD ONLY DO IF HAVE DEPTH BUFFER
		//The position of the point light
		Vector3f pos = getPosition();

		shadowCubemapData.lightPos = Vector4f(pos, 0.0f);

		//Assign each of the transform matrices
		shadowCubemapData.shadowMatrices[0] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));
		shadowCubemapData.shadowMatrices[1] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));
		shadowCubemapData.shadowMatrices[2] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
		shadowCubemapData.shadowMatrices[3] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
		shadowCubemapData.shadowMatrices[4] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f));
		shadowCubemapData.shadowMatrices[5] = getProjectionMatrix() * Matrix4f().initLookAt(pos, pos + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));

		//Update the descriptor set
		shadowCubemapDescriptorSet->getUBO(0)->updateFrame(&shadowCubemapData, 0, sizeof(ShaderBlock_ShadowCubemap));
	} else if (type == TYPE_SPOT && shadowMapRenderPass) {
		//The position of the spot light
		Vector3f pos = getPosition();

		Vector3f right = direction.cross(Vector3f(0.0f, 1.0f, 0.0f)).normalise();
		Vector3f up = right.cross(direction).normalise();

		getViewMatrix().initLookAt(pos, pos + direction, up);

		lightProjectionView = getProjectionMatrix() * getViewMatrix();

		//Update the frustum
		frustum.update(lightProjectionView);

		setViewMatrix(lightProjectionView); //Shortcut

		//Update the UBO
		updateUBO();
	}
}

void Light::useView() {
	//Bind the shadow cubemap if needed
	if (shadowCubemapDescriptorSet)
		shadowCubemapDescriptorSet->bind();
	else
		Camera::useView();
}

void Light::setUniforms(ShaderStruct_Light& lightData) {
	lightData.type = type;
	lightData.position = Vector4f(getPosition(), 0.0f);
	lightData.direction = Vector4f(direction, 0.0f);
	lightData.diffuseColour = Vector4f(diffuseColour, 0.0f);
	lightData.specularColour = Vector4f(specularColour, 0.0f);
	lightData.constant = constant;
	lightData.linear = linear;
	lightData.quadratic = quadratic;
	lightData.innerCutoff = innerCutoff;
	lightData.outerCutoff = outerCutoff;
	lightData.useShadowMap = hasShadowMap();
}

