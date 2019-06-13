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

#include "VulkanShader.h"

#include "../../utils/Logging.h"

#include "Vulkan.h"

#include <fstream>

/*****************************************************************************
 * The VulkanShader class
 *****************************************************************************/

VulkanShader::VulkanShader(std::string path) {
	//Create the graohics pipeline
	auto vertShaderCode = readFile(path + "_vert.spv");
	auto fragShaderCode = readFile(path + "_frag.spv");

	//Can destroy after pipeline creation
	vertexShaderModule   = createShaderModule(Vulkan::getDevice(), vertShaderCode);
	fragmentShaderModule = createShaderModule(Vulkan::getDevice(), fragShaderCode);
}

VulkanShader::~VulkanShader() {
	//Destroy the shader modules
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), vertexShaderModule, nullptr);
	vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), fragmentShaderModule, nullptr);
}

VkShaderModule VulkanShader::createShaderModule(VulkanDevice* device, const std::vector<char>& code) {
	//Assign the creation info
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

	//Load the shader module
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->getLogical(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		Logger::log("Failed to create shader module", "VulkanGraphicsPipeline", LogType::Error);

	return shaderModule;
}

std::vector<char> VulkanShader::readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary); //Read at end as can tell size of file from position

	if (! file.is_open())
		Logger::log("Failed to open file " + fileName, "VulkanGraphicsPipeline", LogType::Error);

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
