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

#include <GL/glew.h>

#include "Shader.h"

#include "../Window.h"
#include "../vulkan/VulkanBuffer.h"
#include "DataUsage.h"

 /*****************************************************************************
  * The ShaderBuffer class is used to manage a UBO/SSBO
  *****************************************************************************/

class ShaderBuffer {
public:
	/* The possible shader buffer types */
	enum class Type {
		UBO,
		SSBO
	};
private:
	/* The target/usage of this buffer for OpenGL/Vulkan */
	GLenum glTarget;
	VkBufferUsageFlagBits vkBufferUsageFlags;
	VkDescriptorType      vkDescriptorType;

	/* The buffer instance for OpenGL */
	GLuint buffer = 0;

	/* The buffer for Vulkan */
	VulkanBufferObject* vulkanBuffer = NULL;

	/* The size of this buffer */
	unsigned int size;

	/* The block binding of this UBO/SSBO (the location used in the shader) */
	unsigned int blockBinding;
public:
	/* Offset used for binding locations when using Vulkan */
	static const unsigned int VULKAN_BINDING_OFFSET = 20;

	/* Constructor */
	ShaderBuffer(Type type, void* data, unsigned int size, DataUsage usage, unsigned int blockBinding);

	/* Destructor */
	virtual ~ShaderBuffer();

	/* Method to bind this buffer for use with OpenGL (binding done by descriptor set in Vulkan) */
	void bindGL();

	/* Method to update the contents of this buffer for a frame - Should be done when rendering (For Vulkan synchronisation) */
	void updateFrame(void* data, unsigned int offset, unsigned int size);

	/* Method to update the contents of this buffer - Should be done when updating (For Vulkan synchronisation) */
	void update(void* data, unsigned int offset, unsigned int size);

	/* Method that returns the descriptor for a particular buffer for Vulkan */
	VkWriteDescriptorSet getVkWriteDescriptorSet(unsigned int frame, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo* bufferInfo);

	/* Method to get a VulkanBuffer for a particular frame */
	VulkanBuffer* getVkBuffer(unsigned int frame) { return vulkanBuffer->getBuffer(frame); }

	/* Returns the binding index */
	unsigned int getBinding() { return blockBinding; }

	/* Various other getters */
	VkDescriptorType getVkDescriptorType() { return vkDescriptorType; }

	/* Converts a ShaderBuffer::Type to a VkDescriptorType */
	static VkDescriptorType convertToVkDescriptorType(Type type);
};


