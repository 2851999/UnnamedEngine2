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

class DescriptorSetLayout;

/*****************************************************************************
 * The DescriptorSet class responsible for handeling combinations of Textures
 * and UBOs when rendering
 *****************************************************************************/

class DescriptorSet {
public:
	/* The type of a texture binding */
	enum class TextureType {
		TEXTURE_2D, TEXTURE_CUBE
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
	};
private:
	/* The layout of this descriptor set */
	DescriptorSetLayout* layout;

	/* The UBOs within this descriptor set */
	std::vector<UBO*> ubos;

	/* The textures within this descriptor set */
	std::vector<TextureInfo> textures;

	/* The texture bindings within this descriptor set */
	std::vector<TextureBindingInfo> textureBindings;

	/* The descriptor pool for the allocation of descriptors in Vulkan */
	VkDescriptorPool vulkanDescriptorPool = VK_NULL_HANDLE;

	/* The descriptor sets required in Vulkan (one per swap chain image) */
	std::vector<VkDescriptorSet> vulkanDescriptorSets;

	/* States whether this object is currently within the update queue
       in Vulkan*/
	bool m_isInUpdateQueue = false;

	/* Method used to update this descriptor set for Vulkan (This method updates for
	   all internal descriptor sets and as such should not be used during rendering) */
	void updateAllVk();
public:
	/* Constructor */
	DescriptorSet(DescriptorSetLayout* layout);

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

	/* Method used to unbind this descriptor set (For textures in OpenGL) */
	void unbind();

	/* Setters and getters */
	inline void setTexture(unsigned int index, Texture* texture) { textures[index].texture = texture; }

	inline UBO* getUBO(unsigned int index) { return ubos[index]; }
	inline unsigned int getNumUBOs() { return ubos.size(); }
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
	/* Stucture for storing information about a UBO */
	struct UBOInfo {
		unsigned int size;
		DataUsage    usage;
		unsigned int binding;
	};
private:
	/* The set number of this set (corresponds to layout(set) in Vulkan shaders) */
	unsigned int setNumber;

	/* UBOs required in this layout*/
	std::vector<UBOInfo> ubos;

	/* Texture bindings required in this layout, specified using their binding number */
	std::vector<DescriptorSet::TextureBindingInfo> textureBindings;

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
	inline void addTextureBinding(DescriptorSet::TextureType type, unsigned int binding, unsigned int numTextures) {
		DescriptorSet::TextureBindingInfo bindingInfo;
		bindingInfo.type = type;
		bindingInfo.binding = binding;
		bindingInfo.numTextures = numTextures;
		textureBindings.push_back(bindingInfo);
	}

	/* Methods to add UBOs and Textures to this layout */
	inline void addTexture2D(unsigned int binding) { addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, binding, 1); }
	inline void addTextureCube(unsigned int binding) { addTextureBinding(DescriptorSet::TextureType::TEXTURE_CUBE, binding, 1); }
	inline void addUBO(unsigned int size, DataUsage usage, unsigned int binding) { ubos.push_back({ size, usage, binding }); }

	/* Getters */
	inline unsigned int getSetNumber() { return setNumber; }
	inline std::vector<UBOInfo>& getUBOs() { return ubos; }
	inline std::vector<DescriptorSet::TextureBindingInfo>& getTextureBindings() { return textureBindings; }
	inline VkDescriptorSetLayout& getVkLayout() { return vulkanDescriptorSetLayout; }
};

