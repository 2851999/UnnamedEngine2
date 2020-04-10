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

#include "Texture.h"
#include "UBO.h"

class DescriptorSetLayout;

 /*****************************************************************************
  * The DescriptorSet class responsible for handeling combinations of Textures
  * and UBOs when rendering
  *****************************************************************************/

class DescriptorSet {
public:
    /* Structure for storing information about a texture */
    struct TextureInfo {
        /* The binding location of this texture */
        unsigned int binding;
        /* The texture to be bound here */
        Texture* texture = NULL;
    };
private:
    /* The UBOs within this descriptor set */
    std::vector<UBO*> ubos;

    /* The textures within this descriptor set */
    std::vector<TextureInfo> textures;

    /* The descriptor pool for the allocation of descriptors in Vulkan */
    VkDescriptorPool vulkanDescriptorPool = VK_NULL_HANDLE;

    /* The descriptor sets required in Vulkan (one per swap chain image) */
    std::vector<VkDescriptorSet> vulkanDescriptorSets;

    /* Method used to update this descriptor set for Vulkan (This method updates for
       all internal descriptor sets and as such should not be used during rendering) */
    void updateAllVulkan();
public:
    /* Constructor */
    DescriptorSet() {}

    /* Destructor */
    virtual ~DescriptorSet();

    /* Method used to create the required structures for Vulkan */
    void setupVulkan(DescriptorSetLayout* layout);

    /* Method used to bind this descriptor set */
    void bind();

    /* Method used to unbind this descriptor set (For textures in OpenGL) */
    void unbind();
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
        GLenum       usage;
        unsigned int binding;
    };
private:
    /* UBOs required in this layout*/
    std::vector<UBOInfo> ubos;

    /* Textures required in this layout, specified using their binding number */
    std::vector<unsigned int> textureBindings;

    /* The descriptor set layout instance for Vulkan*/
    VkDescriptorSetLayout vulkanDescriptorSetLayout = VK_NULL_HANDLE;
public:
    /* Constructor */
    DescriptorSetLayout() {}

    /* Destructor */
    virtual ~DescriptorSetLayout();

    /* Method used to create the required structures for Vulkan */
    void setupVulkan();

    /* Returns the VkDescriptorSetLayout corresponding to this layout for Vulkan */
    inline VkDescriptorSetLayout getVkLayout() { return vulkanDescriptorSetLayout; }
};