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

#ifndef CORE_VULKAN_VULKANSHADER_H_
#define CORE_VULKAN_VULKANSHADER_H_

#include "../render/UBO.h"

/*****************************************************************************
 * The VulkanShader class
 *****************************************************************************/

class VulkanShader {
private:
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;

	/* Method to create a shader module */
	static VkShaderModule createShaderModule(VulkanDevice* device, const std::vector<char>& code);

	/* Method to read a file */
	static std::vector<char> readFile(const std::string& fileName);
public:
	/* Constructor */
	VulkanShader(std::string path);

	/* Destructor */
	virtual ~VulkanShader();

	/* Getters */
	VkShaderModule& getVertexShaderModule() { return vertexShaderModule; }
	VkShaderModule& getFragmentShaderModule() { return fragmentShaderModule; }
};

#endif /* CORE_VULKAN_VULKANSHADER_H_ */
