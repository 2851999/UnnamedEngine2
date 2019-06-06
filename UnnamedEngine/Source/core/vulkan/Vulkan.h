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

#ifndef CORE_VULKAN_VULKAN_H_
#define CORE_VULKAN_VULKAN_H_

#include "../Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsPipeline.h"

#include "../render/VBO.h"
#include "../render/Texture.h"

/*****************************************************************************
 * The Vulkan class manages resources required for Vulkan
 *****************************************************************************/

class Vulkan {
private:
	/* The actual instance handled by this class */
	static VkInstance instance;

	/* Debug messenger */
	static VkDebugUtilsMessengerEXT debugMessenger;

	/* The surface used for rendering to the window */
	static VkSurfaceKHR windowSurface;

	/* The device instance */
	static VulkanDevice* device;

	/* The swap chain */
	static VulkanSwapChain* swapChain;

	/* The render pass */
	static VulkanRenderPass* renderPass;

	/* Vertex and indices data */
	static std::vector<float> vertices;
	static std::vector<unsigned int> indices;

	/* The vertex and index buffers to render */
	static VBO<float>* vertexBuffer;
	static VBO<unsigned int>* indexBuffer;

	/* The graphics pipeline */
	static VulkanGraphicsPipeline* graphicsPipeline;

	/* Command pool */
	static VkCommandPool commandPool;

	/* Command buffers (one for each swap chain image) */
	static std::vector<VkCommandBuffer> commandBuffers;

	/* Synchronisation objects */
	static std::vector<VkSemaphore> imageAvailableSemaphores; //Signals image acquired ready for rendering
	static std::vector<VkSemaphore> renderFinishedSemaphores; //Signals rendering finished, can present
	static std::vector<VkFence> inFlightFences;
	static unsigned int currentFrame;

	/* The descriptor pool (For UBO's) */
	static VkDescriptorPool descriptorPool;

	/* The descriptor set layout (For UBO) */
	static VkDescriptorSetLayout descriptorSetLayout;

	static std::vector<VkDescriptorSet> descriptorSets;

	static std::vector<VulkanBuffer*> uniformBuffers;

	static Texture* texture;
	static VkSampler textureSampler;

	struct UBOData {
		Matrix4f mvpMatrix;
	};

	static UBOData uboData;

	/* Method to create a debug messenger */
	static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	/* Method to destroy a debug messenger */
	static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	/* Callback method for debug messeges */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		    VkDebugUtilsMessageTypeFlagsEXT messageType,
		    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		    void* pUserData);
public:
	/* States whether validation layers are enabled or not */
	static bool ENABLE_VALIDATION_LAYERS;

	/* Method to initialise everything required for Vulkan - returns if this was successful */
	static bool initialise(Window* window);

	/* Method to destroy everything required for Vulkan */
	static void destroy();

	/* Method to create the Vulkan instance (Returns whether the creation was successful */
	static bool createInstance();

	/* Method to destroy the Vulkan instance */
	static void destroyInstance();

	/* Method to create a debug messenger for displaying output for validation layers */
	static void createDebugMessenger();

	/* Method to destroy the debug messenger */
	static void destroyDebugMessenger();

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

	static void createTextureSampler();
	static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	static VkCommandBuffer beginSingleTimeCommands();
	static void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	/* Method to create the synchronisation objects */
	static void createSyncObjects();

	/* Method to destroy the synchronisation objects */
	static void destroySyncObjects();

	/* Method to create the descriptor pool */
	static void createDescriptorPool();

	/* Method to destroy the descriptor pool */
	static void destroyDescriptorPool();

	/* Method to create the descriptor sets */
	static void createDescriptorSets();

	/* Method to create the descriptor set layout */
	static void createDescriptorSetLayout();

	/* Method to destroy the descriptor set layout */
	static void destroyDescriptorSetLayout();

	/* Method to create the uniform buffers */
	static void createUniformBuffers();

	/* Method to destroy the uniform buffers */
	static void destroyUniformBuffers();

	/* Method to update the uniform buffer */
	static void updateUniformBuffer();

	/* Method to start drawing a frame (and recording to the command buffer) */
	static void startDraw();

	/* Method to stop drawing a frame (and recording to the command buffer) */
	static void stopDraw();

	/* Method to draw a frame */
	static void drawFrame();

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
	static inline VkCommandPool& getCommandPool() { return commandPool; }
	static inline VkCommandBuffer& getCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
	static inline unsigned int getCurrentFrame() { return currentFrame; }
};


#endif /* CORE_VULKAN_VULKAN_H_ */
