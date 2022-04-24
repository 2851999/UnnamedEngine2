/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "../Window.h"

/*****************************************************************************
 * The VulkanExtensions class handles the names of required Vulkan extensions
 *****************************************************************************/

class VulkanExtensions {
private:
	/* Stores all the required extensions */
	static std::vector<const char*> requiredExtensions;

	/* Stores all the required device extensions */
	static std::vector<const char*> requiredDeviceExtensions;

	/* Stores the extensions required for raytracing */
	static std::vector<const char*> requiredRaytracingExtensions;

	/* Various Vulkan extension methods */
	static PFN_vkGetBufferDeviceAddressKHR loaded_vkGetBufferDeviceAddressKHR;
	static PFN_vkCreateAccelerationStructureKHR loaded_vkCreateAccelerationStructureKHR;
	static PFN_vkDestroyAccelerationStructureKHR loaded_vkDestroyAccelerationStructureKHR;
	static PFN_vkGetAccelerationStructureBuildSizesKHR loaded_vkGetAccelerationStructureBuildSizesKHR;
	static PFN_vkGetAccelerationStructureDeviceAddressKHR loaded_vkGetAccelerationStructureDeviceAddressKHR;
	static PFN_vkCmdBuildAccelerationStructuresKHR loaded_vkCmdBuildAccelerationStructuresKHR;
	static PFN_vkBuildAccelerationStructuresKHR loaded_vkBuildAccelerationStructuresKHR;
	static PFN_vkCmdTraceRaysKHR loaded_vkCmdTraceRaysKHR;
	static PFN_vkGetRayTracingShaderGroupHandlesKHR loaded_vkGetRayTracingShaderGroupHandlesKHR;
	static PFN_vkCreateRayTracingPipelinesKHR loaded_vkCreateRayTracingPipelinesKHR;
	static PFN_vkCmdCopyAccelerationStructureKHR loaded_vkCmdCopyAccelerationStructureKHR;
	static PFN_vkCmdWriteAccelerationStructuresPropertiesKHR loaded_vkCmdWriteAccelerationStructuresPropertiesKHR;
public:
	/* Adds the default extensions required */
	static void addRequired();

	/* Adds an extension to the list of requested ones */
	static void addRequiredExtension(const char* extension);

	/* Obtains required extension methods for use */
	static void loadExtensionMethods();

	/* Various Vulkan extension methods */
	static inline VkDeviceAddress vkGetBufferDeviceAddressKHR(VkDevice device, const VkBufferDeviceAddressInfo* pInfo) { return loaded_vkGetBufferDeviceAddressKHR(device, pInfo); }
	static inline VkResult vkCreateAccelerationStructureKHR(VkDevice device, const VkAccelerationStructureCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure) { return loaded_vkCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure); }
	static inline void vkDestroyAccelerationStructureKHR(VkDevice device, VkAccelerationStructureKHR accelerationStructure, const VkAllocationCallbacks* pAllocator) { loaded_vkDestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator); }
	static inline void vkGetAccelerationStructureBuildSizesKHR(VkDevice device, VkAccelerationStructureBuildTypeKHR buildType, const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) { loaded_vkGetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo); }
	static inline VkDeviceAddress vkGetAccelerationStructureDeviceAddressKHR(VkDevice device, const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) { return loaded_vkGetAccelerationStructureDeviceAddressKHR(device, pInfo); }
	static inline void vkCmdBuildAccelerationStructuresKHR(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) { loaded_vkCmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos); }
	static inline VkResult vkBuildAccelerationStructuresKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) { return loaded_vkBuildAccelerationStructuresKHR(device, deferredOperation, infoCount, pInfos, ppBuildRangeInfos); }
	static inline void vkCmdTraceRaysKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint32_t width, uint32_t height, uint32_t depth) { loaded_vkCmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth); }
	static inline VkResult vkGetRayTracingShaderGroupHandlesKHR(VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData) { return loaded_vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData); }
	static inline VkResult vkCreateRayTracingPipelinesKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkRayTracingPipelineCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { return loaded_vkCreateRayTracingPipelinesKHR(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines); }
	static inline void vkCmdCopyAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo) { loaded_vkCmdCopyAccelerationStructureKHR(commandBuffer, pInfo); }
	static inline void vkCmdWriteAccelerationStructuresPropertiesKHR(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery) { return loaded_vkCmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery); }

	/* Returns whether the device extensions required are supported by a particular device */
	static bool checkSupport(VkPhysicalDevice device);

	/* Returns a list of the required extensions */
	static std::vector<const char*>& getRequiredExtentions() { return requiredExtensions; }

	/* Returns a list of the required device extensions */
	static std::vector<const char*>& getRequiredDeviceExtentions() { return requiredDeviceExtensions; }
};

