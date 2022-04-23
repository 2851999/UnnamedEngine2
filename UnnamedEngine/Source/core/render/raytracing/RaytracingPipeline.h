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

	//TODO: Cleanup and comment below

	// Push constant structure for the ray tracer
	struct PushConstantRay {
		Vector4f clearColor;
		Vector3f lightPosition;
		float    lightIntensity;
		int      lightType;
	};

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties;

	PushConstantRay pcRay;

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;

	VulkanBuffer* rtSBTBuffer;
	VkStridedDeviceAddressRegionKHR rgenRegion{};
	VkStridedDeviceAddressRegionKHR missRegion{};
	VkStridedDeviceAddressRegionKHR hitRegion{};
	VkStridedDeviceAddressRegionKHR callRegion{};

	template <class integral>
	constexpr bool is_aligned(integral x, size_t a) noexcept {
		return (x & (integral(a) - 1)) == 0;
	}

	/* Used when building the SBT */
	template <class integral>
	constexpr integral align_up(integral x, size_t a) noexcept {
		return integral((x + (integral(a) - 1)) & ~integral(a - 1));
	}

	template <class integral>
	constexpr integral align_down(integral x, size_t a) noexcept {
		return integral(x & ~integral(a - 1));
	}

	/* Create the shader binding table
       - All shaders must be accessible at once when raytracing
	     This allows the corect shader to be selected at runtime */
	void createRtShaderBindingTable();
public:
	/* Constructor */
	RaytracingPipeline(VkPhysicalDeviceRayTracingPipelinePropertiesKHR raytracingProperties, VkShaderModule raygenShader, VkShaderModule missShader, VkShaderModule closestHitShader, DescriptorSetLayout* rtLayout);

	/* Desctructor */
	virtual ~RaytracingPipeline();

	/* Binds the pipeline */
	void bind();

	/* Getters */
	inline VkPipelineLayout& getLayout() { return pipelineLayout; }
	inline const VkStridedDeviceAddressRegionKHR* getRGenRegion() { return &rgenRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getMissRegion() { return &missRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getHitRegion() { return &hitRegion; }
	inline const VkStridedDeviceAddressRegionKHR* getCallRegion() { return &callRegion; }
};
