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

#include "VulkanFeatures.h"

#include "Vulkan.h"

#include "../Window.h"

 /*****************************************************************************
  * The VulkanFeatures class
  *****************************************************************************/

VkPhysicalDeviceFeatures                         VulkanFeatures::requiredDeviceFeatures        = {};
VkPhysicalDeviceVulkan11Features                 VulkanFeatures::requiredDeviceVK11Features    = {};
VkPhysicalDeviceBufferDeviceAddressFeatures      VulkanFeatures::featuresBufferDeviceAddress   = {};
VkPhysicalDeviceRayTracingPipelineFeaturesKHR    VulkanFeatures::featuresRayTracingPipeline    = {};
VkPhysicalDeviceAccelerationStructureFeaturesKHR VulkanFeatures::featuresAccelerationStructure = {};
VkPhysicalDeviceShaderClockFeaturesKHR           VulkanFeatures::featuresShaderClock           = {};
VkPhysicalDeviceDescriptorIndexingFeatures       VulkanFeatures::featuresDescriptorIndexing    = {};

std::vector<void*> VulkanFeatures::requiredDeviceFeatures2 = {};

void VulkanFeatures::addRequired() {
	//Request anisotropic filtering/geometry shaders (support must be checked in 'checkDeviceSupport')
	requiredDeviceFeatures.samplerAnisotropy = VK_TRUE;
	requiredDeviceFeatures.geometryShader    = VK_TRUE;

	//Setup and add the required features (support must be checked in 'checkDeviceSupport')
	if (Window::getCurrentInstance()->getSettings().videoRaytracing) {
		//Required for using GL_EXT_shader_explicit_arithmetic_types_int64 in shaders
		requiredDeviceFeatures.shaderInt64 = VK_TRUE;

		featuresBufferDeviceAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		featuresBufferDeviceAddress.bufferDeviceAddress = VK_TRUE;
		requiredDeviceFeatures2.push_back(&featuresBufferDeviceAddress);

		featuresRayTracingPipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		featuresRayTracingPipeline.rayTracingPipeline = VK_TRUE;
		requiredDeviceFeatures2.push_back(&featuresRayTracingPipeline);

		featuresAccelerationStructure.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		featuresAccelerationStructure.accelerationStructure = VK_TRUE;
		requiredDeviceFeatures2.push_back(&featuresAccelerationStructure);

		featuresShaderClock.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
		featuresShaderClock.shaderSubgroupClock = VK_TRUE;
		requiredDeviceFeatures2.push_back(&featuresShaderClock);

		featuresDescriptorIndexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		featuresDescriptorIndexing.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		featuresDescriptorIndexing.runtimeDescriptorArray                    = VK_TRUE;
		requiredDeviceFeatures2.push_back(&featuresDescriptorIndexing);
	}
}

bool VulkanFeatures::checkSupport(VkPhysicalDevice& device) {
	//Obtain the supported features
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	bool featuresSupported = supportedFeatures.samplerAnisotropy && supportedFeatures.geometryShader;

	//Setup and query the supported features that require use of pNext
	if (Window::getCurrentInstance()->getSettings().videoRaytracing) {
		VkPhysicalDeviceVulkan11Features                 enabledVK11Features                  = {};
		VkPhysicalDeviceBufferDeviceAddressFeatures      enabledBufferDeviceAddressFeatures   = {};
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR    enabledRayTracingPipelineFeatures    = {};
		VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures = {};
		VkPhysicalDeviceShaderClockFeaturesKHR           enabledShaderClockFeatures           = {};
		VkPhysicalDeviceDescriptorIndexingFeatures       enabledDescriptorIndexingFeatures    = {};

		enabledVK11Features.sType                  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		enabledBufferDeviceAddressFeatures.sType   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		enabledRayTracingPipelineFeatures.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		enabledAccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		enabledShaderClockFeatures.sType           = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
		enabledDescriptorIndexingFeatures.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

		//Required device features
		std::vector<void*> requiredDeviceFeatures = { &enabledVK11Features, &enabledBufferDeviceAddressFeatures, &enabledRayTracingPipelineFeatures, &enabledAccelerationStructureFeatures, &enabledShaderClockFeatures, &enabledDescriptorIndexingFeatures };

		VkPhysicalDeviceFeatures2 supportedFeatures2 = {};
		supportedFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		supportedFeatures2.pNext = VulkanFeatures::setupPNext(requiredDeviceFeatures);
		vkGetPhysicalDeviceFeatures2(device, &supportedFeatures2);

		featuresSupported = featuresSupported && supportedFeatures.shaderInt64 && enabledBufferDeviceAddressFeatures.bufferDeviceAddress && enabledRayTracingPipelineFeatures.rayTracingPipeline && enabledAccelerationStructureFeatures.accelerationStructure && enabledShaderClockFeatures.shaderSubgroupClock && enabledDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing && enabledDescriptorIndexingFeatures.runtimeDescriptorArray;
	}

	return featuresSupported;
}

void* VulkanFeatures::setupPNext(std::vector<void*>& requiredDeviceFeatures2) {
	//Structure to help linking the pNext of features
	struct FeatureHeader {
		VkStructureType sType;
		void* pNext;
	};

	for (unsigned int i = 0; i < requiredDeviceFeatures2.size(); ++i) {
		auto* header = reinterpret_cast<FeatureHeader*>(requiredDeviceFeatures2[i]);
		//Assign the pNext to the next feature in the list provided it is not the last
		header->pNext = i < requiredDeviceFeatures2.size() - 1 ? requiredDeviceFeatures2[i + 1] : nullptr;
	}
	return requiredDeviceFeatures2[0];
}