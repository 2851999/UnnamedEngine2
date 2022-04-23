/*****************************************************************************
 *
 *   Copyright 2022 Joel Davies
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

#include "RaytracingPipeline.h"

#include "../Renderer.h"
#include "../../vulkan/Vulkan.h"
#include "../../vulkan/VulkanExtensions.h"
#include "../../../utils/VulkanUtils.h"

#include <cassert>

 /*****************************************************************************
  * The RaytracingPipeline class
  *****************************************************************************/

RaytracingPipeline::RaytracingPipeline(VkPhysicalDeviceRayTracingPipelinePropertiesKHR raytracingProperties, VkShaderModule raygenShader, VkShaderModule missShader, VkShaderModule closestHitShader, DescriptorSetLayout* rtLayout) : rtProperties(raytracingProperties) {
	//enum StageIndices {
	//	eRaygen,
	//	eMiss,
	//	eClosestHit,
	//	eShaderGroupCount
	//};

	////All stages
	//std::vector<VkPipelineShaderStageCreateInfo> stages{};
	//stages.resize(eShaderGroupCount);
	//VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	//stage.pName = "main";  //All the same entry point
	////Raygen
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rgen.spv"));
	//stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	//stages[eRaygen] = stage;
	////Miss
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rmiss.spv"));
	//stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	//stages[eMiss] = stage;
	////The second miss shader is invoked when a shadow ray misses the geometry. It simply indicates that no occlusion has been found
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytraceShadow.rmiss.spv"));
	//stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	//stages[eMiss] = stage;
	////Hit Group - Closest Hit
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rchit.spv"));
	//stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	//stages[eClosestHit] = stage;

	enum StageIndices {
		eRaygen,
		eMiss,
		eClosestHit,
		eShaderGroupCount
	};

	//All stages
	std::vector<VkPipelineShaderStageCreateInfo> stages{};
	stages.resize(eShaderGroupCount);
	//Raygen
	stages[eRaygen] = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_RAYGEN_BIT_KHR, raygenShader, "main");
	//Miss
	stages[eMiss] = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_MISS_BIT_KHR, missShader, "main");
	//Hit Group - Closest Hit
	stages[eClosestHit] = utils_vulkan::initPipelineShaderStageCreateInfo(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, closestHitShader, "main");

	//Shader groups
	VkRayTracingShaderGroupCreateInfoKHR group{ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
	group.anyHitShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = VK_SHADER_UNUSED_KHR;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.intersectionShader = VK_SHADER_UNUSED_KHR;

	//Raygen
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eRaygen;
	shaderGroups.push_back(group);

	//Miss
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss;
	shaderGroups.push_back(group);

	//closest hit shader
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = eClosestHit;
	shaderGroups.push_back(group);

	////Push constants
	//VkPushConstantRange pushConstant{ VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR, 0, sizeof(PushConstantRay) };

	//Descriptor sets for the pipeline - one for the camera (set 0) and anoher for the TLAS/storage image
	//Needs to be in order of set numbers
	std::vector<VkDescriptorSetLayout> rtDescSetLayouts = { Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_CAMERA)->getVkLayout(), rtLayout->getVkLayout()};

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = utils_vulkan::initPipelineLayoutCreateInfo(static_cast<uint32_t>(rtDescSetLayouts.size()), rtDescSetLayouts.data());
	//pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	//pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;


	//Create the pipeline layout
	vkCreatePipelineLayout(Vulkan::getDevice()->getLogical(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

	//Assign the shader stages and recursion depth info
	VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
	rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());  //Stages are shaders
	rayPipelineInfo.pStages = stages.data();

	//In this case shaderGroups == 3 - one raygen group, one miss shader group, one hit group
	rayPipelineInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
	rayPipelineInfo.pGroups = shaderGroups.data();

	//Ray depth cannot exceed rtProperties.maxRayRecursionDepth
	rayPipelineInfo.maxPipelineRayRecursionDepth = 1;
	rayPipelineInfo.layout = pipelineLayout;

	//Create the raytracing pipeline
	VulkanExtensions::vkCreateRayTracingPipelinesKHR(Vulkan::getDevice()->getLogical(), {}, {}, 1, &rayPipelineInfo, nullptr, &pipeline);

	createRtShaderBindingTable();
}

void RaytracingPipeline::createRtShaderBindingTable() {
	//Number of each shader
	uint32_t missCount{ 1 };
	uint32_t hitCount{ 1 };
	auto     handleCount = 1 + missCount + hitCount; //Always only 1 raygen shader
	uint32_t handleSize = rtProperties.shaderGroupHandleSize;

	//Need to use align_up here as there is no guarentee the alignment corresponds to the handle or group size
	//Hardware with a smaller handle size than alignment can end up interleaving some shaderRecordEXT data without
	//additional memory usage
	uint32_t handleSizeAligned = align_up(handleSize, rtProperties.shaderGroupHandleAlignment);

	rgenRegion.stride = align_up(handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	rgenRegion.size = rgenRegion.stride;  //This member of pRayGenShaderBindingTable must be equal to its stride member
	missRegion.stride = handleSizeAligned;
	missRegion.size = align_up(missCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	hitRegion.stride = handleSizeAligned;
	hitRegion.size = align_up(hitCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);

	//Obtain the shader group handles
	uint32_t dataSize = handleCount * handleSize;
	std::vector<uint8_t> handles(dataSize);
	auto result = VulkanExtensions::vkGetRayTracingShaderGroupHandlesKHR(Vulkan::getDevice()->getLogical(), pipeline, 0, handleCount, dataSize, handles.data());
	assert(result == VK_SUCCESS);

	//Allocate a buffer for storing the SBT.
	VkDeviceSize sbtSize = rgenRegion.size + missRegion.size + hitRegion.size + callRegion.size;
	rtSBTBuffer = new VulkanBuffer(sbtSize, Vulkan::getDevice(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);

	//Find the SBT addresses of each group (requires VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT in the above)
	VkDeviceAddress sbtAddress = Vulkan::getBufferDeviceAddress(rtSBTBuffer->getInstance());
	rgenRegion.deviceAddress = sbtAddress;
	missRegion.deviceAddress = sbtAddress + rgenRegion.size;
	hitRegion.deviceAddress = sbtAddress + rgenRegion.size + missRegion.size;

	//Returns pointer to the previously retrieved handle (used for copying data into the SBT buffer)
	auto getHandle = [&](int i) { return handles.data() + i * handleSize; };

	//Map the SBT buffer to write the data
	void* pSBTBufferVoid;
	vkMapMemory(Vulkan::getDevice()->getLogical(), rtSBTBuffer->getDeviceMemory(), 0, sbtSize, 0, &pSBTBufferVoid); //Data is now mapped

	auto* pSBTBuffer = reinterpret_cast<uint8_t*>(pSBTBufferVoid);
	uint8_t* pData{ nullptr };
	uint32_t handleIdx{ 0 };

	//Raygen
	pData = pSBTBuffer;
	memcpy(pData, getHandle(handleIdx++), handleSize);

	//Miss
	pData = pSBTBuffer + rgenRegion.size;
	for (uint32_t c = 0; c < missCount; ++c) {
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += missRegion.stride;
	}

	//Hit
	pData = pSBTBuffer + rgenRegion.size + missRegion.size;
	for (uint32_t c = 0; c < hitCount; ++c) {
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += hitRegion.stride;
	}

	//Finished so unmap
	vkUnmapMemory(Vulkan::getDevice()->getLogical(), rtSBTBuffer->getDeviceMemory());
}

RaytracingPipeline::~RaytracingPipeline() {
	delete rtSBTBuffer;
	vkDestroyPipeline(Vulkan::getDevice()->getLogical(), pipeline, nullptr);
	vkDestroyPipelineLayout(Vulkan::getDevice()->getLogical(), pipelineLayout, nullptr);
}

void RaytracingPipeline::bind() {
	//Bind the pipeline
	vkCmdBindPipeline(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline);
}