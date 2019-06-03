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

#include "VulkanValidationLayers.h"

#include "../Window.h"
#include <cstdint>

/*****************************************************************************
 * The VulkanValidationLayers class
 *****************************************************************************/

std::vector<const char*> VulkanValidationLayers::validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
};

bool VulkanValidationLayers::checkSupport() {
	//Obtain the supported layers
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//Go through the required layers and ensure all of the required layers are supported
	for (const char* layerName : validationLayers) {
		//States whether the layer was found in the list of available layers
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				//Have found the required layer in the list of supported layers
				layerFound = true;
				break;
			}
		}

		if (! layerFound)
			return false; //Found a layer that isn't supported
	}

	return true;
}
