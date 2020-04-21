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

#include "VulkanDevice.h"

#include "Vulkan.h"
#include "VulkanExtensions.h"
#include "VulkanValidationLayers.h"
#include "../../utils/Logging.h"

#include <set>

/*****************************************************************************
 * The VulkanDevice class
 *****************************************************************************/

VulkanDevice::VulkanDevice(VkPhysicalDevice& physicalDevice) {
	this->physicalDevice = physicalDevice;

	//Obtain the device queue families
	this->queueFamilies = findQueueFamilies(physicalDevice);

	//Create the logical device
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilies.graphicsFamilyIndex, queueFamilies.presentFamilyIndex }; //Likely these are identical, so only one one in set

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		//Setup the queue create info
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	//Request anisotropic filtering (support must be checked in 'isDeviceSuitable')
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	//Obtain the required validation layers and device extensions
	std::vector<const char*>& validationLayers = VulkanValidationLayers::getLayers();
	std::vector<const char*>& deviceExtensions = VulkanExtensions::getDeviceExtentions();

	//Info for logical device creation
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos       = queueCreateInfos.data();
	createInfo.pEnabledFeatures 	   = &deviceFeatures;
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	//Not required in recent Vulkan implementations (as no distinction between instance and device validation layers)
	if (Window::getCurrentInstance()->getSettings().debugVkValidationLayersEnabled) {
		createInfo.enabledLayerCount   = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else
		createInfo.enabledLayerCount   = 0;
	//---------------------------------------------------------------------------------------------------------------

	//Create the logical device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
		Logger::log("Failed to create a logical device", "VulkanDevice", LogType::Error);

	//Obtain the device queues requested
	vkGetDeviceQueue(logicalDevice, queueFamilies.graphicsFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilies.presentFamilyIndex,  0, &presentQueue );
}

VulkanDevice::~VulkanDevice() {
	//Device queues also cleaned up when device is destroyed

	//Destroy the device
	vkDestroyDevice(logicalDevice, nullptr);
}

std::string VulkanDevice::listLimits() {
	//Obtain the device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	//Obtain all of the properties and return them as a string
	return
		"maxImageDimension1D: " + utils_string::str(deviceProperties.limits.maxImageDimension1D) + "\n" +
		"maxImageDimension2D: " + utils_string::str(deviceProperties.limits.maxImageDimension2D) + "\n" +
		"maxImageDimension3D: " + utils_string::str(deviceProperties.limits.maxImageDimension3D) + "\n" +
		"maxImageDimensionCube: " + utils_string::str(deviceProperties.limits.maxImageDimensionCube) + "\n" +
		"maxImageArrayLayers: " + utils_string::str(deviceProperties.limits.maxImageArrayLayers) + "\n" +
		"maxTexelBufferElements: " + utils_string::str(deviceProperties.limits.maxTexelBufferElements) + "\n" +
		"maxUniformBufferRange: " + utils_string::str(deviceProperties.limits.maxUniformBufferRange) + "\n" +
		"maxStorageBufferRange: " + utils_string::str(deviceProperties.limits.maxStorageBufferRange) + "\n" +
		"maxPushConstantsSize: " + utils_string::str(deviceProperties.limits.maxPushConstantsSize) + "\n" +
		"maxMemoryAllocationCount: " + utils_string::str(deviceProperties.limits.maxMemoryAllocationCount) + "\n" +
		"maxSamplerAllocationCount: " + utils_string::str(deviceProperties.limits.maxSamplerAllocationCount) + "\n" +
		"bufferImageGranularity: " + utils_string::str(deviceProperties.limits.bufferImageGranularity) + "\n" +
		"sparseAddressSpaceSize: " + utils_string::str(deviceProperties.limits.sparseAddressSpaceSize) + "\n" +
		"maxBoundDescriptorSets: " + utils_string::str(deviceProperties.limits.maxBoundDescriptorSets) + "\n" +
		"maxPerStageDescriptorSamplers: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorSamplers) + "\n" +
		"maxPerStageDescriptorUniformBuffers: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorUniformBuffers) + "\n" +
		"maxPerStageDescriptorStorageBuffers: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorStorageBuffers) + "\n" +
		"maxPerStageDescriptorSampledImages: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorSampledImages) + "\n" +
		"maxPerStageDescriptorStorageImages: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorStorageImages) + "\n" +
		"maxPerStageDescriptorInputAttachments: " + utils_string::str(deviceProperties.limits.maxPerStageDescriptorInputAttachments) + "\n" +
		"maxPerStageResources: " + utils_string::str(deviceProperties.limits.maxPerStageResources) + "\n" +
		"maxDescriptorSetSamplers: " + utils_string::str(deviceProperties.limits.maxDescriptorSetSamplers) + "\n" +
		"maxDescriptorSetUniformBuffers: " + utils_string::str(deviceProperties.limits.maxDescriptorSetUniformBuffers) + "\n" +
		"maxDescriptorSetUniformBuffersDynamic: " + utils_string::str(deviceProperties.limits.maxDescriptorSetUniformBuffersDynamic) + "\n" +
		"maxDescriptorSetStorageBuffers: " + utils_string::str(deviceProperties.limits.maxDescriptorSetStorageBuffers) + "\n" +
		"maxDescriptorSetStorageBuffersDynamic: " + utils_string::str(deviceProperties.limits.maxDescriptorSetStorageBuffersDynamic) + "\n" +
		"maxDescriptorSetSampledImages: " + utils_string::str(deviceProperties.limits.maxDescriptorSetSampledImages) + "\n" +
		"maxDescriptorSetStorageImages: " + utils_string::str(deviceProperties.limits.maxDescriptorSetStorageImages) + "\n" +
		"maxDescriptorSetInputAttachments: " + utils_string::str(deviceProperties.limits.maxDescriptorSetInputAttachments) + "\n" +
		"maxVertexInputAttributes: " + utils_string::str(deviceProperties.limits.maxVertexInputAttributes) + "\n" +
		"maxVertexInputBindings: " + utils_string::str(deviceProperties.limits.maxVertexInputBindings) + "\n" +
		"maxVertexInputAttributeOffset: " + utils_string::str(deviceProperties.limits.maxVertexInputAttributeOffset) + "\n" +
		"maxVertexInputBindingStride: " + utils_string::str(deviceProperties.limits.maxVertexInputBindingStride) + "\n" +
		"maxVertexOutputComponents: " + utils_string::str(deviceProperties.limits.maxVertexOutputComponents) + "\n" +
		"maxTessellationGenerationLevel: " + utils_string::str(deviceProperties.limits.maxTessellationGenerationLevel) + "\n" +
		"maxTessellationPatchSize: " + utils_string::str(deviceProperties.limits.maxTessellationPatchSize) + "\n" +
		"maxTessellationControlPerVertexInputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationControlPerVertexInputComponents) + "\n" +
		"maxTessellationControlPerVertexOutputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationControlPerVertexOutputComponents) + "\n" +
		"maxTessellationControlPerPatchOutputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationControlPerPatchOutputComponents) + "\n" +
		"maxTessellationControlTotalOutputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationControlTotalOutputComponents) + "\n" +
		"maxTessellationEvaluationInputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationEvaluationInputComponents) + "\n" +
		"maxTessellationEvaluationOutputComponents: " + utils_string::str(deviceProperties.limits.maxTessellationEvaluationOutputComponents) + "\n" +
		"maxGeometryShaderInvocations: " + utils_string::str(deviceProperties.limits.maxGeometryShaderInvocations) + "\n" +
		"maxGeometryInputComponents: " + utils_string::str(deviceProperties.limits.maxGeometryInputComponents) + "\n" +
		"maxGeometryOutputComponents: " + utils_string::str(deviceProperties.limits.maxGeometryOutputComponents) + "\n" +
		"maxGeometryOutputVertices: " + utils_string::str(deviceProperties.limits.maxGeometryOutputVertices) + "\n" +
		"maxGeometryTotalOutputComponents: " + utils_string::str(deviceProperties.limits.maxGeometryTotalOutputComponents) + "\n" +
		"maxFragmentInputComponents: " + utils_string::str(deviceProperties.limits.maxFragmentInputComponents) + "\n" +
		"maxFragmentOutputAttachments: " + utils_string::str(deviceProperties.limits.maxFragmentOutputAttachments) + "\n" +
		"maxFragmentDualSrcAttachments: " + utils_string::str(deviceProperties.limits.maxFragmentDualSrcAttachments) + "\n" +
		"maxFragmentCombinedOutputResources: " + utils_string::str(deviceProperties.limits.maxFragmentCombinedOutputResources) + "\n" +
		"maxComputeSharedMemorySize: " + utils_string::str(deviceProperties.limits.maxComputeSharedMemorySize) + "\n" +
		"maxComputeWorkGroupCount[0]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupCount[0]) + "\n" +
		"maxComputeWorkGroupCount[1]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupCount[1]) + "\n" +
		"maxComputeWorkGroupCount[2]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupCount[2]) + "\n" +
		"maxComputeWorkGroupInvocations: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupInvocations) + "\n" +
		"maxComputeWorkGroupSize[0]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupSize[0]) + "\n" +
		"maxComputeWorkGroupSize[1]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupSize[1]) + "\n" +
		"maxComputeWorkGroupSize[2]: " + utils_string::str(deviceProperties.limits.maxComputeWorkGroupSize[2]) + "\n" +
		"subPixelPrecisionBits: " + utils_string::str(deviceProperties.limits.subPixelPrecisionBits) + "\n" +
		"subTexelPrecisionBits: " + utils_string::str(deviceProperties.limits.subTexelPrecisionBits) + "\n" +
		"mipmapPrecisionBits: " + utils_string::str(deviceProperties.limits.mipmapPrecisionBits) + "\n" +
		"maxDrawIndexedIndexValue: " + utils_string::str(deviceProperties.limits.maxDrawIndexedIndexValue) + "\n" +
		"maxDrawIndirectCount: " + utils_string::str(deviceProperties.limits.maxDrawIndirectCount) + "\n" +
		"maxSamplerLodBias: " + utils_string::str(deviceProperties.limits.maxSamplerLodBias) + "\n" +
		"maxSamplerAnisotropy: " + utils_string::str(deviceProperties.limits.maxSamplerAnisotropy) + "\n" +
		"maxViewports: " + utils_string::str(deviceProperties.limits.maxViewports) + "\n" +
		"maxViewportDimensions[0]: " + utils_string::str(deviceProperties.limits.maxViewportDimensions[0]) + "\n" +
		"maxViewportDimensions[1]: " + utils_string::str(deviceProperties.limits.maxViewportDimensions[1]) + "\n" +
		"viewportBoundsRange[0]: " + utils_string::str(deviceProperties.limits.viewportBoundsRange[0]) + "\n" +
		"viewportBoundsRange[1]: " + utils_string::str(deviceProperties.limits.viewportBoundsRange[1]) + "\n" +
		"viewportSubPixelBits: " + utils_string::str(deviceProperties.limits.viewportSubPixelBits) + "\n" +
		"minMemoryMapAlignment: " + utils_string::str(deviceProperties.limits.minMemoryMapAlignment) + "\n" +
		"minTexelBufferOffsetAlignment: " + utils_string::str(deviceProperties.limits.minTexelBufferOffsetAlignment) + "\n" +
		"minUniformBufferOffsetAlignment: " + utils_string::str(deviceProperties.limits.minUniformBufferOffsetAlignment) + "\n" +
		"minStorageBufferOffsetAlignment: " + utils_string::str(deviceProperties.limits.minStorageBufferOffsetAlignment) + "\n" +
		"minTexelOffset: " + utils_string::str(deviceProperties.limits.minTexelOffset) + "\n" +
		"maxTexelOffset: " + utils_string::str(deviceProperties.limits.maxTexelOffset) + "\n" +
		"minTexelGatherOffset: " + utils_string::str(deviceProperties.limits.minTexelGatherOffset) + "\n" +
		"maxTexelGatherOffset: " + utils_string::str(deviceProperties.limits.maxTexelGatherOffset) + "\n" +
		"minInterpolationOffset: " + utils_string::str(deviceProperties.limits.minInterpolationOffset) + "\n" +
		"maxInterpolationOffset: " + utils_string::str(deviceProperties.limits.maxInterpolationOffset) + "\n" +
		"subPixelInterpolationOffsetBits: " + utils_string::str(deviceProperties.limits.subPixelInterpolationOffsetBits) + "\n" +
		"maxFramebufferWidth: " + utils_string::str(deviceProperties.limits.maxFramebufferWidth) + "\n" +
		"maxFramebufferHeight: " + utils_string::str(deviceProperties.limits.maxFramebufferHeight) + "\n" +
		"maxFramebufferLayers: " + utils_string::str(deviceProperties.limits.maxFramebufferLayers) + "\n" +
		"framebufferColorSampleCounts: " + utils_string::str(deviceProperties.limits.framebufferColorSampleCounts) + "\n" +
		"framebufferDepthSampleCounts: " + utils_string::str(deviceProperties.limits.framebufferDepthSampleCounts) + "\n" +
		"framebufferStencilSampleCounts: " + utils_string::str(deviceProperties.limits.framebufferStencilSampleCounts) + "\n" +
		"framebufferNoAttachmentsSampleCounts: " + utils_string::str(deviceProperties.limits.framebufferNoAttachmentsSampleCounts) + "\n" +
		"maxColorAttachments: " + utils_string::str(deviceProperties.limits.maxColorAttachments) + "\n" +
		"sampledImageColorSampleCounts: " + utils_string::str(deviceProperties.limits.sampledImageColorSampleCounts) + "\n" +
		"sampledImageIntegerSampleCounts: " + utils_string::str(deviceProperties.limits.sampledImageIntegerSampleCounts) + "\n" +
		"sampledImageDepthSampleCounts: " + utils_string::str(deviceProperties.limits.sampledImageDepthSampleCounts) + "\n" +
		"sampledImageStencilSampleCounts: " + utils_string::str(deviceProperties.limits.sampledImageStencilSampleCounts) + "\n" +
		"storageImageSampleCounts: " + utils_string::str(deviceProperties.limits.storageImageSampleCounts) + "\n" +
		"maxSampleMaskWords: " + utils_string::str(deviceProperties.limits.maxSampleMaskWords) + "\n" +
		"timestampComputeAndGraphics: " + utils_string::str(deviceProperties.limits.timestampComputeAndGraphics) + "\n" +
		"timestampPeriod: " + utils_string::str(deviceProperties.limits.timestampPeriod) + "\n" +
		"maxClipDistances: " + utils_string::str(deviceProperties.limits.maxClipDistances) + "\n" +
		"maxCullDistances: " + utils_string::str(deviceProperties.limits.maxCullDistances) + "\n" +
		"maxCombinedClipAndCullDistances: " + utils_string::str(deviceProperties.limits.maxCombinedClipAndCullDistances) + "\n" +
		"discreteQueuePriorities: " + utils_string::str(deviceProperties.limits.discreteQueuePriorities) + "\n" +
		"pointSizeRange[0]: " + utils_string::str(deviceProperties.limits.pointSizeRange[0]) + "\n" +
		"pointSizeRange[1]: " + utils_string::str(deviceProperties.limits.pointSizeRange[1]) + "\n" +
		"lineWidthRange[0]: " + utils_string::str(deviceProperties.limits.lineWidthRange[2]) + "\n" +
		"lineWidthRange[1]: " + utils_string::str(deviceProperties.limits.lineWidthRange[2]) + "\n" +
		"pointSizeGranularity: " + utils_string::str(deviceProperties.limits.pointSizeGranularity) + "\n" +
		"lineWidthGranularity: " + utils_string::str(deviceProperties.limits.lineWidthGranularity) + "\n" +
		"strictLines: " + utils_string::str(deviceProperties.limits.strictLines) + "\n" +
		"standardSampleLocations: " + utils_string::str(deviceProperties.limits.standardSampleLocations) + "\n" +
		"optimalBufferCopyOffsetAlignment: " + utils_string::str(deviceProperties.limits.optimalBufferCopyOffsetAlignment) + "\n" +
		"optimalBufferCopyRowPitchAlignment: " + utils_string::str(deviceProperties.limits.optimalBufferCopyRowPitchAlignment) + "\n" +
		"nonCoherentAtomSize: " + utils_string::str(deviceProperties.limits.nonCoherentAtomSize) + "\n";
}

std::string VulkanDevice::listSupportedExtensions() {
	//Get the available extensions for the device
	uint32_t deviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

	std::vector<VkExtensionProperties> availbleDeviceExtensions(deviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availbleDeviceExtensions.data());

	std::string output;
	for (unsigned int i = 0; i < deviceExtensionCount; ++i) {
		output += utils_string::str(availbleDeviceExtensions[i].extensionName); // +"        " + utils_string::str(availbleDeviceExtensions[i].specVersion);
		if (i < deviceExtensionCount - 1)
			output += "\n";
	}

	return output;
}

VulkanDevice* VulkanDevice::create() {
	//The device instance to return
	VulkanDevice* deviceInstance = NULL;

	//Obtain a list of physical devices
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(Vulkan::getInstance(), &physicalDeviceCount, nullptr);

	//Check there are devices available
	if (physicalDeviceCount > 0) {
		VkPhysicalDevice chosenPhysicalDevice = VK_NULL_HANDLE;

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(Vulkan::getInstance(), &physicalDeviceCount, physicalDevices.data());

		//Go through the devices and pick the first suitable one
		for (const auto& physicalDevice : physicalDevices) {
			if (isDeviceSuitable(physicalDevice)) {
				chosenPhysicalDevice = physicalDevice;
				break;
			}
		}

		//Check a physical device was found
		if (chosenPhysicalDevice != VK_NULL_HANDLE)
			//Create the device
			deviceInstance = new VulkanDevice(chosenPhysicalDevice);
		else
			Logger::log("Failed to find a suitable physical device", "VulkanDevice", LogType::Error);
	} else
		Logger::log("Failed to find any physical devices", "VulkanDevice", LogType::Error);

	//Return the device instance
	return deviceInstance;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) {
	//Obtain the device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//Obtain supported queue families
	VulkanDeviceQueueFamilies queueFamilies = findQueueFamilies(device);

	//Check extensions are supported
	bool extensionsSupported = VulkanExtensions::checkSupport(device);

	//Check swap chain support of the device
	bool swapChainSupportAdequate = false;

	if (extensionsSupported) {
		VulkanDeviceSwapChainSupportDetails swapChainSupportDetails = querySwapChainSupport(device);
		swapChainSupportAdequate = (! swapChainSupportDetails.formats.empty()) && (! swapChainSupportDetails.presentModes.empty());
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	//Return the result
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && queueFamilies.isComplete() && extensionsSupported && swapChainSupportAdequate && supportedFeatures.samplerAnisotropy;
}

VulkanDeviceQueueFamilies VulkanDevice::findQueueFamilies(VkPhysicalDevice device) {
	VulkanDeviceQueueFamilies queueFamilies;

	//Obtain queue families supported by the device
	uint32_t availableQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &availableQueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> availableQueueFamilies(availableQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &availableQueueFamilyCount, availableQueueFamilies.data());

	//Go through the available queue families for this device
	for (uint32_t i = 0; i < availableQueueFamilyCount; ++i) {
		//Check for present support (rendering to a surface)
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Vulkan::getWindowSurface(), &presentSupport);

		//Check and assign queue family with present/graphics support
		if (availableQueueFamilies[i].queueCount > 0 && presentSupport)
			queueFamilies.asssignPresentFamily(i);
		if (availableQueueFamilies[i].queueCount > 0 && (availableQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			queueFamilies.assignGraphicsFamily(i);

		//Stop if all required families have been found
		if (queueFamilies.isComplete())
			break;
	}

	//Return the found info
	return queueFamilies;
}

VulkanDeviceSwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) {
	VulkanDeviceSwapChainSupportDetails details;

	//The window surface being used
	VkSurfaceKHR windowSurface = Vulkan::getWindowSurface();

	//Obtain the surface capabilities of the device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface, &details.capabilities);

	//Obtain the supported surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, nullptr);
	if (formatCount > 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &formatCount, details.formats.data());
	}

	//Obtain the supported present modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, nullptr);
	if (presentModeCount > 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModeCount, details.presentModes.data());
	}

	//Return the support details
	return details;
}
