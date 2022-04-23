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

#include "../Window.h"

 /*****************************************************************************
  * The VulkanFeatures class handles the required Vulkan device features
  *****************************************************************************/

class VulkanFeatures {
private:
	/* Various structures defining required device features */
	static VkPhysicalDeviceFeatures                         requiredDeviceFeatures;
	static VkPhysicalDeviceVulkan11Features                 requiredDeviceVK11Features;
	static VkPhysicalDeviceBufferDeviceAddressFeatures      featuresBufferDeviceAddress;
	static VkPhysicalDeviceRayTracingPipelineFeaturesKHR    featuresRayTracingPipeline;
	static VkPhysicalDeviceAccelerationStructureFeaturesKHR featuresAccelerationStructureFeatures;

	/* List of the required features */
	static std::vector<void*> requiredDeviceFeatures2;
public:
	/* Adds the default features required */
	static void addRequired();

	/* Checks whether a physical device has the required features */
	static bool checkSupport(VkPhysicalDevice& device);

	/* Returns the required device features */
	static inline VkPhysicalDeviceFeatures& getRequiredDeviceFeatures() { return requiredDeviceFeatures;  }

	/* Returns a list of the required device features (for pNext in VkPhysicalDeviceFeatures2) */
	static inline std::vector<void*>& getRequiredDeviceFeatures2() { return requiredDeviceFeatures2; }

	/* Method to setup pNext on an array of required features - returns the pNext that should be used in a
       VkPhysicalDeviceFeatures2 */
	static void* setupPNext(std::vector<void*>& requiredDeviceFeatures2);
};