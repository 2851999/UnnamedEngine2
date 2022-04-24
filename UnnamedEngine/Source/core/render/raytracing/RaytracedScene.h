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

#include "../Renderer.h"
#include "RaytracingPipeline.h"

 /*****************************************************************************
  * The RaytracedScene class handles the raytracing of a group of objects
  *****************************************************************************/

class RaytracedScene {
private:
	/* Structure for storing information required for building BLASs */
	struct BLASInput {
		std::vector<VkAccelerationStructureGeometryKHR>       asGeometry;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> asBuildOffsetInfo;
		VkBuildAccelerationStructureFlagsKHR                  flags{ 0 };
	};

	/* Stores the information about and a buffer containing an acceleration structure*/
	struct AccelKHR {
		VkAccelerationStructureKHR accel = VK_NULL_HANDLE;
		VulkanBuffer* buffer = NULL;
	};

	/* Stores information required for building a BLAS */
	struct BuildAccelerationStructure {
		VkAccelerationStructureBuildGeometryInfoKHR     buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
		VkAccelerationStructureBuildSizesInfoKHR        sizeInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
		const VkAccelerationStructureBuildRangeInfoKHR* rangeInfo;
		AccelKHR                                        as;  // Acceleration structuree to use
		AccelKHR                                        cleanupAS;
	};

	/* Stores device locations of buffers required by the raytracing shader */
	struct ModelData {
		VkDeviceAddress vertexBufferAddress;
		VkDeviceAddress indexBufferAddress;
		VkDeviceAddress matIndexBufferAddress;
		VkDeviceAddress matDataBufferAddress;
		VkDeviceAddress offsetIndicesBufferAddress;
	};

	/* Physical device (Used frequently so keep a pointer here) */
	VulkanDevice* device;

	/* Raytracing hysical device properties */
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtDeviceProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };

	/* The objects contained within this scene */
	std::vector<GameObject3D*> objects;

	/* BLAS instances */
	std::vector<AccelKHR> blas;

	/* TLAS instance */
	AccelKHR tlas;

	/* Storage texture for storing the output of raytracing */
	Texture* storageTexture;

	/* The model data for all the models */
	std::vector<ModelData> sceneModelData;

	/* Buffer storing all the material data in the scene */
	VulkanBuffer* materialDataBuffer;

	/* Pipeline for raytracing */
	RaytracingPipeline* rtPipeline;

	/* Descriptor set and its layout for raytracing */
	DescriptorSetLayout* rtDescriptorSetLayout;
	DescriptorSet*       rtDescriptorSet;

	/* Pipeline/Mesh for drawing the output onto the screen using rasterized rendering */
	GraphicsPipeline* screenRenderPipeline;
	Mesh*             screenRenderMesh;

	/* Creates a BLASInput from a given mesh and subdata (For materials create one BLAS for each SubData for now) 
	  * Currently assume:
	  *	- not separating the vertex data into separate VBO's
	  *	- buffers are not updated (so don't have to create multiple identical ones depending on the maximum number of frames in flight)
	  *	- asssumes vertex position is the first element in the data otherwise need to use offsetof to adjust */
	BLASInput objectToVkGeometryKHR(Mesh* mesh, MeshData::SubData& subData);

	/* Helper for determing whehther a flag has been added to some VkFlags */
	inline bool hasFlag(VkFlags item, VkFlags flag) { return (item & flag) == flag; }

	/* Creates all the BLASs given a list BLASInput's
	 * - Batches BLAS structures into 256MB chunks of memory - avoids potenial problems */
	void buildBLAS(const std::vector<BLASInput>& input, VkBuildAccelerationStructureFlagsKHR flags);

	/* Creates a single BLAS for each element in 'buildAs'
	   - In future this would be faster by building multiple BLAS simultaneuously using a single large scratch buffer */
	void cmdCreateBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkDeviceAddress scratchAddress, VkQueryPool queryPool);

	/* Obtains query results for the compacted size of BLAS structures, creates new BLAS's with the reduced size and copies the existing
	   data to them reducing memory usage */
	void cmdCompactBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkQueryPool queryPool);

	/* Cleans up after the use of 'cmdCompactBLAS' by destroying the old non-compacted structures */
	void destroyNonCompacted(std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs);

	/* Creates and builds the all the required BLAS structures using the above methods */
	void setupAllBLAS();

	/* Converts Matrix4f to VkTransformMatrixKHR while transposing since VkTransformMatrixKHR has a row-major memory layout */
	VkTransformMatrixKHR toTransformMatrixKHR(Matrix4f matrix);

	/* Builds a TLAS given the BLAS it should refer to */
	void buildTLAS(const std::vector<VkAccelerationStructureInstanceKHR>& asInstances, VkBuildAccelerationStructureFlagsKHR flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR, bool update = false, bool motion = false);

	/* Creates a TLAS given the number of BLASs, device address of the buffer holding the VkAccelerationStructureInstanceKHR instances and a scratch buffer
       (that should be destroyed after use) */
	void cmdCreateTLAS(VkCommandBuffer cmdBuf, uint32_t countInstance, VkDeviceAddress instBufferAddr, VulkanBuffer*& scratchBuffer, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion);

	/* Creates and builds the TLAS */
	void setupTLAS();

	/* Method called to initialise for raytracing (obtains physical device properties) */
	void initRaytracing();

	/* Creates the storage texture */
	void createStorageTexture();

	/* Sets up all of the data required by shaders for raytracing the scene */
	void setupModelData();

	/* Returns a MeshData* instance for rendering to the screen */
	MeshData* createScreenMeshData();
public:
	/* Constructor */
	RaytracedScene();

	/* Destructor */
	virtual ~RaytracedScene();

	/* Method for adding an object to this scene */
	inline void add(GameObject3D* object) { objects.push_back(object); }

	/* Method for setting up this scene ready for rendering (should be called after all objects in it are added) */
	void setup(VkShaderModule raygenShader, std::vector<VkShaderModule> missShaders, VkShaderModule closestHitShader);

	/* Performs raytracing (also ensures layout of storage texture is correct for using it in a shader) */
	void raytrace(Camera3D* camera);

	/* Renders the storage texture to the screen using rasterised rendering */
	void renderOutput();
};
