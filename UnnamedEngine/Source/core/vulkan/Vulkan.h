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
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "../render/VBO.h"
#include "../render/UBO.h"
#include "../render/Texture.h"
#include "../render/Mesh.h"
#include "VulkanGraphicsPipeline.h"

class DescriptorSet;

/*****************************************************************************
 * The Vulkan class manages resources required for Vulkan
 *****************************************************************************/

class Vulkan {
private:
	/* The actual instance handled by this class */
	static VkInstance instance;

	/* The surface used for rendering to the window */
	static VkSurfaceKHR windowSurface;

	/* The device instance */
	static VulkanDevice* device;

	/* The swap chain */
	static VulkanSwapChain* swapChain;

	/* The render pass */
	static VulkanRenderPass* renderPass;

	/* Command pool */
	static VkCommandPool commandPool;

	/* Command buffers (one for each swap chain image) */
	static std::vector<VkCommandBuffer> commandBuffers;

	/* Synchronisation objects */
	static std::vector<VkSemaphore> imageAvailableSemaphores; //Signals image acquired ready for rendering
	static std::vector<VkSemaphore> renderFinishedSemaphores; //Signals rendering finished, can present
	static std::vector<VkFence> inFlightFences;
	static unsigned int currentFrame;

	/* The current bound graphics pipeline (Used for descriptor set binding) */
	static VulkanGraphicsPipeline* currentGraphicsPipeline;

	/* Structure for storing info about a requested descriptor set update */
	struct DescriptorSetUpdateInfo {
		//The descriptor set instance to be updated
		DescriptorSet* set;

		//The next frame to update (different to the current to avoid synchronisation problems)
		unsigned int nextUpdateFrame;

		//Number of updates left to perform
		unsigned int updatesLeft;
	};

	/* List of descriptor set updates to perform, will work on assumption
	   all descriptor sets require same number of updates based on the number
	   of swap chain images */
	static std::vector<DescriptorSetUpdateInfo> descriptorSetUpdateQueue;

	/* Method to update a descriptor set for the current frame (Returns whether the set has been fully updated) */
	static bool updateDescriptorSetFrame(DescriptorSetUpdateInfo& info);

	/* Method to update the descriptor sets in the update queue */
	static void updateDescriptorSetQueue();

	/* Structure for storing info about a requested UBO update */
	struct UBOUpdateInfo {
		//The UBO instance to be updated
		UBO* ubo;

		//The information needed to perform the update each frame
		void*        data;
		unsigned int offset;
		unsigned int size;

		//The next frame to update (different to the current to avoid synchronisation problems)
		unsigned int nextUpdateFrame;

		//Number of updates left to perform
		unsigned int updatesLeft;
	};

	/* List of UBO updates to perform */
	static std::vector<UBOUpdateInfo> uboUpdateQueue;

	/* Method to update a UBO for the current frame (Returns whether the set has been fully updated) */
	static bool updateUBOFrame(UBOUpdateInfo& info);

	/* Method to update UBOs in the update queue*/
	static void updateUBOQueue();
public:
	/* Method to initialise everything required for Vulkan - returns if this was successful */
	static bool initialise(Window* window);

	/* Method to destroy everything required for Vulkan */
	static void destroy();

	/* Method to create the Vulkan instance (Returns whether the creation was successful */
	static bool createInstance();

	/* Method to destroy the Vulkan instance */
	static void destroyInstance();

	/* Method to create the window surface (Returns whether the creation was successful) */
	static bool createWindowSurface(Window* window);

	/* Method to destroy the window surface */
	static void destroyWindowSurface();

	/* Method to create the command pool */
	static void createCommandPool();

	/* Method to destroy the command pool */
	static void destroyCommandPool();

	/* Method to create the command buffers */
	static void createCommandBuffers(); //(Destroyed with command pool)

	static void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	static VkImageView createImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layerCount);
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount, VkCommandBuffer commandBuffer = VK_NULL_HANDLE); //Pass VK_NULL_HANDLE for command buffer to let this method create one
	static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	static VkCommandBuffer beginSingleTimeCommands();
	static void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	/* Method to create the synchronisation objects */
	static void createSyncObjects();

	/* Method to destroy the synchronisation objects */
	static void destroySyncObjects();

	/* Method to update for rendering this Vulkan (Should be called after update of game loop, before rendering) */
	static void update();

	/* Method to start drawing a frame (and recording to the command buffer) */
	static void startDraw();

	/* Method to stop drawing a frame (and recording to the command buffer) */
	static void stopDraw();

	/* Method to bind a graphics pipeline for rendering */
	static void bindGraphicsPipeline(VulkanGraphicsPipeline* pipeline);

	/* Method to update a descriptor set */
	static void updateDescriptorSet(DescriptorSet* set);

	/* Method to update a UBO */
	static void updateUBO(UBO* ubo, void* data, unsigned int offset, unsigned int size);

	/* Method to obtain the maximum number of samples supported that is closest to a requested number */
	static VkSampleCountFlagBits getMaxUsableSampleCount(unsigned int targetSamples);

	/* Method to locate an image format based on what is available (used for depth image) */
	static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	/* Method to find a suitable format for a depth image */
	static inline VkFormat findDepthFormat() {
		return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	/* Returns whether a depth format has a stencil component (used with above) */
	static inline bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	/* Locates a particular kind of graphics memory for a buffer */
	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	/* Creates a buffer */
	static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	/* Copies a buffer from one place to another */
	static void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool, VkQueue& graphicsQueue);

	/* Waits for device to be finished working */
	static inline void waitDeviceIdle() { vkDeviceWaitIdle(device->getLogical()); }

	/* Getters */
	static inline VkInstance& getInstance() { return instance; }
	static inline VkSurfaceKHR& getWindowSurface() { return windowSurface; }
	static inline VulkanDevice* getDevice() { return device; }
	static inline VulkanSwapChain* getSwapChain() { return swapChain; }
	static inline VulkanRenderPass* getRenderPass() { return renderPass; }
	static inline VkCommandPool& getCommandPool() { return commandPool; }
	static inline VkCommandBuffer& getCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
	static inline unsigned int getCurrentFrame() { return currentFrame; }
	static inline unsigned int getNextFrame() { return (currentFrame + 1) % swapChain->getImageCount(); }
	static inline VulkanGraphicsPipeline* getCurrentGraphicsPipeline() { return currentGraphicsPipeline; }
};


