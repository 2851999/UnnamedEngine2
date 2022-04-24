/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "Texture.h"
#include "UBO.h"
#include "SSBO.h"

class DescriptorSetLayout;

/*****************************************************************************
 * The DescriptorSet class responsible for handeling combinations of Textures
 * and UBOs when rendering
 *****************************************************************************/

class DescriptorSet {
public:
	/* The type of a texture binding
	   STORAGE_IMAGE is currently only used for Vulkan raytracing */
	enum class TextureType {
		TEXTURE_2D, TEXTURE_CUBE, STORAGE_IMAGE
	};
	
	/* Structure for storing information about a texture */
	struct TextureInfo {
		/* The binding location of this texture */
		unsigned int binding;

		/* The texture to be bound here */
		Texture* texture = NULL;

		/* The index of this binding in the textureBindings list */
		unsigned int bindingInfoIndex;

		/* Location within the binding (for Vulkan) */
		unsigned int locationInBinding;
	};

	/* Structure for storing information about a texture binding */
	struct TextureBindingInfo {
		/* The type of this binding*/
		TextureType type;

		/* The binding location */
		unsigned int binding;

		/* The number of textures in this binding */
		unsigned int numTextures;

		/* The actual textures within this binding */
		std::vector<VkDescriptorImageInfo> textures;

		//States in what pipeline stage it should be accessible (Vulkan)
		VkPipelineStageFlags shaderStageFlags;
	};

	/* Structure for storing information about a VkAccelerationStructure */
	struct AccelerationStructureInfo {
		/* The binding location of this structure */
		unsigned int binding;

		/* The acceleration structure */
		VkAccelerationStructureKHR* accelerationStructure;
	};
private:
	/* The layout of this descriptor set */
	DescriptorSetLayout* layout;

	/* The ShaderBuffers within this descriptor set */
	std::vector<ShaderBuffer*> shaderBuffers;

	/* The textures within this descriptor set */
	std::vector<TextureInfo> textures;

	/* The texture bindings within this descriptor set */
	std::vector<TextureBindingInfo> textureBindings;

	/* The acceleration structure bindings within this descriptor set */
	std::vector<AccelerationStructureInfo> asBindings;

	/* The descriptor pool for the allocation of descriptors in Vulkan */
	VkDescriptorPool vulkanDescriptorPool = VK_NULL_HANDLE;

	/* The descriptor sets required in Vulkan (one per swap chain image) */
	std::vector<VkDescriptorSet> vulkanDescriptorSets;

	/* States whether this object is currently within the update queue
       in Vulkan*/
	bool m_isInUpdateQueue = false;

	/* Experimental stuff for Vulkan */
	bool raytracing = false;

	/* Method used to update this descriptor set for Vulkan (This method updates for
	   all internal descriptor sets and as such should not be used during rendering) */
	void updateAllVk();
public:
	/* Constructor */
	DescriptorSet(DescriptorSetLayout* layout, bool raytracing = false);

	/* Destructor */
	virtual ~DescriptorSet();

	/* Method used to create the required structures for Vulkan */
	void setupVk();

	/* Method to setup this descriptor set ready for use preferably run after all properties setup
	   for the first time (to avoid unnecessary calls to update) */
	void setup();

	/* Method used to update this set for a certain frame (for Vulkan) */
	void updateVk(unsigned int frame);

	/* Method used to update this descriptor set */
	void update();

	/* Method used to bind this descriptor set */
	void bind();

	/* Same as above but used for raytracing */
	void bind(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout);

	/* Method used to unbind this descriptor set (For textures in OpenGL) */
	void unbind();

	/* Setters and getters */
	inline void setTexture(unsigned int index, Texture* texture) { textures[index].texture = texture; }
	inline void setAccclerationStructure(unsigned int index, VkAccelerationStructureKHR* accelerationStructure) { asBindings[index].accelerationStructure = accelerationStructure; }

	inline ShaderBuffer* getShaderBuffer(unsigned int index) { return shaderBuffers[index]; }
	inline unsigned int getNumShaderBuffers() { return shaderBuffers.size(); }
	inline Texture* getTexture(unsigned int index) { return textures[index].texture; }

	/* Called when this instance is removed from the update queue */
	void removedFromUpdateQueue() { m_isInUpdateQueue = false; }
	/* Returns whether this instance is within the update queue */
	bool isInUpdateQueue() { return m_isInUpdateQueue; }
};

/*****************************************************************************
 * The DescriptorSetLayout class responsible for defining the structure of a
 * descriptor set
 *****************************************************************************/
class DescriptorSetLayout {
public:
	/* Stucture for storing information about a ShaderBuffer */
	struct BufferInfo {
		ShaderBuffer::Type type;
		unsigned int       size;
		DataUsage          usage;
		unsigned int       binding;
		//States in what shader stage it should be accessible (Vulkan)
		VkShaderStageFlags shaderStageFlags;
	};

	/* Structure for storing information about an acceleration strcutrue*/
	struct ASInfo {
		unsigned int binding;
		VkShaderStageFlags shaderStageFlags;
	};
private:
	/* The set number of this set (corresponds to layout(set) in Vulkan shaders) */
	unsigned int setNumber;

	/* ShaderBuffers required in this layout*/
	std::vector<BufferInfo> shaderBuffers;

	/* Texture bindings required in this layout, specified using their binding number */
	std::vector<DescriptorSet::TextureBindingInfo> textureBindings;

	/* Acceleration structure bindings required in this layout */
	std::vector<ASInfo> asBindings;

	/* The descriptor set layout instance for Vulkan*/
	VkDescriptorSetLayout vulkanDescriptorSetLayout = VK_NULL_HANDLE;

	/* Method used to create the required structures for Vulkan */
	void setupVk();
public:
	/* Constructor */
	DescriptorSetLayout(unsigned int setNumber) : setNumber(setNumber) {}

	/* Destructor */
	virtual ~DescriptorSetLayout();

	/* Method used to setup this layout */
	void setup();

	/* Method to add a texture binding */
	inline void addTextureBinding(DescriptorSet::TextureType type, unsigned int binding, unsigned int numTextures, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) {
		DescriptorSet::TextureBindingInfo bindingInfo;
		bindingInfo.type             = type;
		bindingInfo.binding          = binding;
		bindingInfo.numTextures      = numTextures;
		bindingInfo.shaderStageFlags = shaderStageFlags;
		textureBindings.push_back(bindingInfo);
	}

	/* Methods to add UBOs and Textures to this layout */
	inline void addTexture2D(unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, binding, 1, shaderStageFlags); }
	inline void addTextureCube(unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { addTextureBinding(DescriptorSet::TextureType::TEXTURE_CUBE, binding, 1, shaderStageFlags); }
	inline void addStorageTexture(unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { addTextureBinding(DescriptorSet::TextureType::STORAGE_IMAGE, binding, 1, shaderStageFlags); }
	inline void addShaderBuffer(ShaderBuffer::Type type, unsigned int size, DataUsage usage, unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { shaderBuffers.push_back({ type, size, usage, binding, shaderStageFlags }); }
	inline void addUBO(unsigned int size, DataUsage usage, unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { addShaderBuffer(ShaderBuffer::Type::UBO, size, usage, binding, shaderStageFlags); }
	inline void addSSBO(unsigned int size, DataUsage usage, unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS) { addShaderBuffer(ShaderBuffer::Type::SSBO, size, usage, binding, shaderStageFlags); }
	inline void addAccelerationStructure(unsigned int binding, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR) { asBindings.push_back({ binding, shaderStageFlags }); }

	/* Getters */
	inline unsigned int getSetNumber() { return setNumber; }
	inline std::vector<BufferInfo>& getShaderBuffers() { return shaderBuffers; }
	inline std::vector<DescriptorSet::TextureBindingInfo>& getTextureBindings() { return textureBindings; }
	inline std::vector<ASInfo>& getAccelerationStructureBindings() { return asBindings;  }
	inline VkDescriptorSetLayout& getVkLayout() { return vulkanDescriptorSetLayout; }
};

