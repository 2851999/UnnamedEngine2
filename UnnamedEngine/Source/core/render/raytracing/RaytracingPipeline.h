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

#pragma once

#include "../../Window.h"

#include "../DescriptorSet.h"
#include "../../vulkan/VulkanBuffer.h"

 /*****************************************************************************
  * The RaytracingPipeline class handles raytracing pipeline (Vulkan only)
  *****************************************************************************/

class RaytracingPipeline {
private:
	/* The pipeline layout */
	VkPipelineLayout pipelineLayout;

	/* The pipeline */
	VkPipeline pipeline = VK_NULL_HANDLE;

	/* Stores the raytracing capabilities of the physical device */
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties;

	/* Shader used with this pipeline */
	Shader* rtShader;

	/* Number of particular kinds of raytracing shader */
	uint32_t numRaygenShaders     = 0;
	uint32_t numMissShaders       = 0;
	uint32_t numClosestHitShaders = 0;

	/* Buffer for storing the shader binding table*/
	VulkanBuffer* rtSBTBuffer;

	/* Regions of the SBT used when raytracing */
	VkStridedDeviceAddressRegionKHR rgenRegion{};
	VkStridedDeviceAddressRegionKHR missRegion{};
	VkStridedDeviceAddressRegionKHR hitRegion{};
	VkStridedDeviceAddressRegionKHR callRegion{};

	/* Used when building the SBT */
	template <class T>
	constexpr T align_up(T x, size_t a) noexcept {
		return T((x + (T(a) - 1)) & ~T(a - 1));
	}

	/* Create the shader binding table
       - All shaders must be accessible at once when raytracing
	     This allows the corect shader to be selected at runtime */
	void createRtShaderBindingTable();

	/* Counts the number of each kind of shader module in the shader ready for building the SBT 
	   NOTE: All shader modules should be added to the shader in the order raygen, miss, closestHit */
	void countShaderTypes();
public:
	/* Push constants for the shaders */
	struct RTPushConstants {
		int frame;
	};

	/* Constructor */
	RaytracingPipeline(VkPhysicalDeviceRayTracingPipelinePropertiesKHR raytracingProperties, Shader* rtShader, DescriptorSetLayout* rtLayout);

	/* Desctructor */
	virtual ~RaytracingPipeline();

	/* Binds the pipeline */
	void bind(const RTPushConstants* pushConstants);

	/* Getters */
	inline VkPipelineLayout& getLayout() { return pipelineLayout; }
	inline const VkStridedDeviceAddressRegionKHR* getRGenRegion() { return &rgenRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getMissRegion() { return &missRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getHitRegion() { return &hitRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getCallRegion() { return &callRegion; }
};
