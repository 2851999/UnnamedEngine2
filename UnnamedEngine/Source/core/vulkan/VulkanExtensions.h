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
public:
	/* Adds the default extensions required */
	static void addRequired();

	/* Adds an extension to the list of requested ones */
	static void addRequiredExtension(const char* extension);

	/* Returns whether the device extensions required are supported by a particular device */
	static bool checkSupport(VkPhysicalDevice device);

	/* Returns a list of the required extensions */
	static std::vector<const char*>& getRequiredExtentions() { return requiredExtensions; }

	/* Returns a list of the required device extensions */
	static std::vector<const char*>& getRequiredDeviceExtentions() { return requiredDeviceExtensions; }
};

