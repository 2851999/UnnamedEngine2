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

#include <cassert>

#include "BaseTest3D.h"

#include "../core/render/Renderer.h"
#include "../utils/GLUtils.h"

//Made folowing
//https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/
//and with reference to
//https://github.com/SaschaWillems/Vulkan/blob/master/examples/raytracingbasic/raytracingbasic.cpp

class Test : public BaseTest3D {
private:
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
	PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
	PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;

	struct ModelInstance {
		Matrix4f transform;
		uint32_t modelIndex;
	};

	std::vector<GameObject3D*> modelObjects;
	std::vector<ModelInstance> modelInstances;

	GameObject3D* model1;
	GameObject3D* model2;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };

	void initRaytracing();
	uint64_t getBufferDeviceAddress(VkBuffer buffer);

	/* Structure containing the information required to build an acceleration structure geometry */
	struct BLASInput {
		std::vector<VkAccelerationStructureGeometryKHR>       asGeometry;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> asBuildOffsetInfo;
		VkBuildAccelerationStructureFlagsKHR                  flags{ 0 };
	};

	BLASInput objectToVkGeometryKHR(Mesh* mesh);
	void createBottomLevelAS();

	struct AccelKHR {
		VkAccelerationStructureKHR accel  = VK_NULL_HANDLE;
		VulkanBuffer*              buffer = NULL;
	};

	struct BuildAccelerationStructure {
		VkAccelerationStructureBuildGeometryInfoKHR     buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
		VkAccelerationStructureBuildSizesInfoKHR        sizeInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
		const VkAccelerationStructureBuildRangeInfoKHR* rangeInfo;
		AccelKHR                                        as;  // Acceleration structuree to use
		AccelKHR                                        cleanupAS;
	};

	bool hasFlag(VkFlags item, VkFlags flag) { return (item & flag) == flag; }

	std::vector<AccelKHR> blas;
	AccelKHR              tlas;

	void buildBLAS(const std::vector<BLASInput>& input, VkBuildAccelerationStructureFlagsKHR flags);

	void cmdCreateBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkDeviceAddress scratchAddress, VkQueryPool queryPool);

	void cmdCompactBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkQueryPool queryPool);

	void destroyNonCompacted(std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs);

	void createTopLevelAS();

	VkTransformMatrixKHR toTransformMatrixKHR(Matrix4f matrix);

	void buildTLAS(const std::vector<VkAccelerationStructureInstanceKHR>& asInstances, VkBuildAccelerationStructureFlagsKHR flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR, bool update = false, bool motion = false);

	void cmdCreateTLAS(VkCommandBuffer cmdBuf, uint32_t countInstance, VkDeviceAddress instBufferAddr, VulkanBuffer*& scratchBuffer, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion);

	DescriptorSet* raytracingDescriptorSet;

	struct StorageImage {
		VkDeviceMemory memory;
		VkImage image;
		VkImageView view;
		VkFormat format;
	} storageImage;

	void createStorageImage();

	// Push constant structure for the ray tracer
	struct PushConstantRay {
		Vector4f clearColor;
		Vector3f lightPosition;
		float    lightIntensity;
		int      lightType;
	};

	void createRtPipeline();

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;

	PushConstantRay pcRay;

	VkPipelineLayout rtPipelineLayout;
	VkPipeline rtPipeline;

	VulkanBuffer* rtSBTBuffer;
	VkStridedDeviceAddressRegionKHR rgenRegion{};
	VkStridedDeviceAddressRegionKHR missRegion{};
	VkStridedDeviceAddressRegionKHR hitRegion{};
	VkStridedDeviceAddressRegionKHR callRegion{};

	template <class integral>
	constexpr bool is_aligned(integral x, size_t a) noexcept {
		return (x & (integral(a) - 1)) == 0;
	}

	/* Used when building the SBT */
	template <class integral>
	constexpr integral align_up(integral x, size_t a) noexcept {
		return integral((x + (integral(a) - 1)) & ~integral(a - 1));
	}

	template <class integral>
	constexpr integral align_down(integral x, size_t a) noexcept {
		return integral(x & ~integral(a - 1));
	}

	void createRtShaderBindingTable();

	void raytrace();

	GraphicsPipeline* materialPipeline;
	GraphicsPipeline* screenRenderPipeline;

	Mesh* screenTextureMesh;

	MeshData* createScreenMeshData();

	Texture* storageTexture;
public:
	virtual void onInitialise() override;
	virtual void onCreated() override;
	virtual void onUpdate() override;
	virtual void onRenderOffscreen() override;
	virtual void onRender() override;
	virtual void onDestroy() override;

	virtual void onKeyPressed(int key) override;
};

void Test::onInitialise() {
	getSettings().videoVSync = true;
	getSettings().videoMaxFPS = 0;
	getSettings().videoSamples = 0;
	getSettings().videoVulkan = true;
	getSettings().videoRaytracing = true;
	getSettings().debugShowInformation = false;
	getSettings().debugVkValidationLayersEnabled = true;
}

void Test::initRaytracing() {
	//Various extension methods needed for raytracing
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkGetBufferDeviceAddressKHR"));
	vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCmdBuildAccelerationStructuresKHR"));
	vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkBuildAccelerationStructuresKHR"));
	vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCreateAccelerationStructureKHR"));
	vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkDestroyAccelerationStructureKHR"));
	vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkGetAccelerationStructureBuildSizesKHR"));
	vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkGetAccelerationStructureDeviceAddressKHR"));
	vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCmdTraceRaysKHR"));
	vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkGetRayTracingShaderGroupHandlesKHR"));
	vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCreateRayTracingPipelinesKHR"));
	vkCmdCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCmdCopyAccelerationStructureKHR"));
	vkCmdWriteAccelerationStructuresPropertiesKHR = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(Vulkan::getDevice()->getLogical(), "vkCmdWriteAccelerationStructuresPropertiesKHR"));

	//Obtain physical device properies for raytracing capabilities
	VkPhysicalDeviceProperties2 prop2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	prop2.pNext = &rtProperties;
	vkGetPhysicalDeviceProperties2(Vulkan::getDevice()->getPhysical(), &prop2);

	std::cout << "Max recursion depth: " + utils_string::str(rtProperties.maxRayRecursionDepth) << std::endl;
}

/* Returns the device address of a vulkan buffer*/
uint64_t Test::getBufferDeviceAddress(VkBuffer buffer) {
	VkBufferDeviceAddressInfoKHR bufferDeviceAI{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	bufferDeviceAI.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(Vulkan::getDevice()->getLogical(), &bufferDeviceAI);
}

/* Generates a BLASInput struct for a mesh 
  *
  * Currently assume:
  *	- not separating the vertex data into separate VBO's
  *	- buffers are not updated (so don't have to create multiple identical ones depending on the maximum number of frames in flight)
  *	- asssumes vertex position is the first element in the data otherwise need to use offsetof to adjust
  */
Test::BLASInput Test::objectToVkGeometryKHR(Mesh* mesh) {
	//Obtain the number of indices and triangels making up the mesh
	uint32_t numIndices   = mesh->getData()->getNumIndices();
	uint32_t numTriangles = numIndices / 3;

	//BLAS builder requires raw device addresses
	VkDeviceAddress vertexAddress = getBufferDeviceAddress(mesh->getRenderData()->getVBOOthers()->getVkCurrentBuffer()->getInstance());
	VkDeviceAddress indexAddress  = getBufferDeviceAddress(mesh->getRenderData()->getIBO()->getVkCurrentBuffer()->getInstance());

	//Describe the vertex buffer - as above, here assume all data is in the 'others' vbo in the mesh render data
	//but keep the same stride so it works when other data is loaded as well
	VkAccelerationStructureGeometryTrianglesDataKHR triangles{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR };
	triangles.vertexFormat             = VK_FORMAT_R32G32B32_SFLOAT; //vec3 vertex position data
	triangles.vertexData.deviceAddress = vertexAddress;
	triangles.vertexStride             = mesh->getRenderData()->getVBOOthers()->getVkBindingDescription().stride;

	//Describe the index buffer
	triangles.indexType               = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress = indexAddress;

	//Leaving out the transform indicates the identity
	//triangles.transformData = {};
	triangles.maxVertex = numIndices; //Number of vertices

	//Define the above data data as containing opaque triangles
	VkAccelerationStructureGeometryKHR asGeom{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	asGeom.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags              = VK_GEOMETRY_OPAQUE_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	//For now the entire array will be used to build the BLAS
	VkAccelerationStructureBuildRangeInfoKHR offset;
	offset.firstVertex     = 0;
	offset.primitiveCount  = numTriangles; //Number of triangles
	offset.primitiveOffset = 0;
	offset.transformOffset = 0;

	//Create the BLASInput - can add more geometry in each BLAS, only one for now
	//TODO: add more for handling multiple materials
	BLASInput input{};
	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);

	return input;
}

/* Creates the BLAS using the above method */
void Test::createBottomLevelAS() {
	//BLAS objects for the scene
	std::vector<BLASInput> allBLAS;
	//Create the BLASInput instances - assume one BLAS per model for now
	allBLAS.reserve(modelObjects.size());
	for (GameObject3D* model : modelObjects)
		allBLAS.emplace_back(objectToVkGeometryKHR(model->getMesh()));

	//Create the BLAS instances using the above input
	buildBLAS(allBLAS, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR); //| VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
}

/* Creates all the BLAS's given BLASInput's
 *  - Batches BLAS structures into 256MB chunks of memory - avoids potenial problems */
void Test::buildBLAS(const std::vector<BLASInput>& input, VkBuildAccelerationStructureFlagsKHR flags) {
	uint32_t     numBlas = static_cast<uint32_t>(input.size());
	VkDeviceSize asTotalSize{ 0 };     //Memory size of all allocated BLAS
	uint32_t     numCompactions{ 0 };  //Nb of BLAS requesting compaction
	VkDeviceSize maxScratchSize{ 0 };  //Largest scratch size

	//Preparing the information required for the acceleration structure build commands
	std::vector<BuildAccelerationStructure> buildAs(numBlas);
	for (uint32_t idx = 0; idx < numBlas; ++idx) {
		//Partially fill the VkAccelerationStructureBuildGeometryInfoKHR for querying the build sizes
		//Other information will be filled in the cmdCreateBLAS
		buildAs[idx].buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildAs[idx].buildInfo.mode          = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildAs[idx].buildInfo.flags         = input[idx].flags | flags;
		buildAs[idx].buildInfo.geometryCount = static_cast<uint32_t>(input[idx].asGeometry.size());
		buildAs[idx].buildInfo.pGeometries   = input[idx].asGeometry.data();

		//Build range info
		buildAs[idx].rangeInfo = input[idx].asBuildOffsetInfo.data();

		//Find sizes to create scratch buffer and acceleration structures - need to find the largest
		//of the added geometries in the BLAS to ensure there is enough room
		std::vector<uint32_t> maxPrimCount(input[idx].asBuildOffsetInfo.size());
		for (uint32_t tt = 0; tt < input[idx].asBuildOffsetInfo.size(); tt++)
			maxPrimCount[tt] = input[idx].asBuildOffsetInfo[tt].primitiveCount;  //Number of primitives/triangles

		//Obtain the required size for the current BLAS
		vkGetAccelerationStructureBuildSizesKHR(Vulkan::getDevice()->getLogical(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildAs[idx].buildInfo, maxPrimCount.data(), &buildAs[idx].sizeInfo);

		//Size needed for the acceleration structure
		asTotalSize    += buildAs[idx].sizeInfo.accelerationStructureSize;
		//Scratch size for the largest BLAS (allows same buffer to be used for multiple)
		maxScratchSize  = std::max(maxScratchSize, buildAs[idx].sizeInfo.buildScratchSize);
		numCompactions += hasFlag(buildAs[idx].buildInfo.flags, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
	}

	//Create the scratch buffer and obtain its device address
	VulkanBuffer* scratchBuffer = new VulkanBuffer(maxScratchSize, Vulkan::getDevice(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	VkDeviceAddress scratchAddress = getBufferDeviceAddress(scratchBuffer->getInstance());

	//Allocate a query pool for storing the needed size for every BLAS compaction.
	VkQueryPool queryPool{ VK_NULL_HANDLE };

	//Allocate a query pool if compaction is requested (used for obtaining
	//the actual size taken up by each BLAS)
	if (numCompactions > 0) {
		//Don't allow mix of on/off compaction
		assert(numCompactions == numBlas);

		//By default vkGetAccelerationStructureBuildSizesKHR gives worst case sizes in the VkAccelerationStructureBuildSizesInfoKHR struct
		//byt here can query the real space needed (can save up to 50% memory usage)

		//Allocate the query pool
		VkQueryPoolCreateInfo qpCreateInfo{ VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
		qpCreateInfo.queryCount = numBlas;
		qpCreateInfo.queryType  = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
		vkCreateQueryPool(Vulkan::getDevice()->getLogical(), &qpCreateInfo, nullptr, &queryPool);
	}

	//Batching creation/compaction of BLAS to allow staying in restricted amount of memory
	std::vector<uint32_t> indices;                    //Indices of the BLAS to create
	VkDeviceSize          batchSize{ 0 };             //Size of the current batch
	VkDeviceSize          batchLimit{ 256'000'000 };  //Maximum size of a batch - 256 MB

	//Go through each BLAS
	for (uint32_t idx = 0; idx < numBlas; ++idx) {
		indices.push_back(idx);

		//Add the current BLAS size to the batch size
		batchSize += buildAs[idx].sizeInfo.accelerationStructureSize;
		//Check if over the limit for this batch, or that we are already on the last one
		if (batchSize >= batchLimit || idx == numBlas - 1) {
			//Create a BLAS accleration structure, build it and if requested
			//query its size for compaction
			VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();
			cmdCreateBLAS(cmdBuf, indices, buildAs, scratchAddress, queryPool);
			Vulkan::endSingleTimeCommands(cmdBuf);

			//Check if requested compaction
			if (queryPool) {
				//Compact the BLAS
				VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();
				cmdCompactBLAS(cmdBuf, indices, buildAs, queryPool);
				Vulkan::endSingleTimeCommands(cmdBuf);

				//Destroy the non-compacted version
				destroyNonCompacted(indices, buildAs);
			}

			//Reset for the next batch
			batchSize = 0;
			indices.clear();
		}
	}

	//Store all the created acceleration structures
	for (auto& b : buildAs)
		blas.emplace_back(b.as);

	//Cleanup
	if (queryPool)
		vkDestroyQueryPool(Vulkan::getDevice()->getLogical(), queryPool, nullptr);
	delete scratchBuffer;
}

/* Creates a BLAS for each element in 'buildAs'
   In future this would be faster by building multiple BLAS simultaneuously uisng the same large
   scratch buffer */
void Test::cmdCreateBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkDeviceAddress scratchAddress, VkQueryPool queryPool) {
	//Reset the query pool for querying the compaction size
	if (queryPool)
		vkResetQueryPool(Vulkan::getDevice()->getLogical(), queryPool, 0, static_cast<uint32_t>(indices.size()));
	uint32_t queryCount{ 0 };

	//Go through each BLAS to create
	for (const auto& idx : indices) {
		//Setup the data required for the creation of the BLAS
		VkAccelerationStructureCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		createInfo.size = buildAs[idx].sizeInfo.accelerationStructureSize;  //Will be used to allocate memory

		//Create a buffer to store the acceleration structure and assign it in the create info
		buildAs[idx].as.buffer = new VulkanBuffer(createInfo.size, Vulkan::getDevice(), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
		createInfo.buffer = buildAs[idx].as.buffer->getInstance();

		//Create the BLAS
		std::cout << "BLAS" << std::endl;
		vkCreateAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), &createInfo, nullptr, &buildAs[idx].as.accel);

		//Setup the additional build info required for building the BLAS (the rest is done in buildBLAS)
		buildAs[idx].buildInfo.dstAccelerationStructure = buildAs[idx].as.accel; //Where to store the acceleration structure
		buildAs[idx].buildInfo.scratchData.deviceAddress = scratchAddress;       //Scratch buffer

		//Build the BLAS
		vkCmdBuildAccelerationStructuresKHR(cmdBuf, 1, &buildAs[idx].buildInfo, &buildAs[idx].rangeInfo);

		//Since the scratch buffer is reused across builds need a memory barrier to ensure each build has finished before starting the next one.
		VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);

		//Pass the AS info ready to query the actual memory required by the BLAS - this will be used for compaction
		//requires the AS - &buildAs[idx].buildInfo.dstAccelerationStructure = buildAs[idx].as.accel above
		if (queryPool)
			vkCmdWriteAccelerationStructuresPropertiesKHR(cmdBuf, 1, &buildAs[idx].buildInfo.dstAccelerationStructure, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR, queryPool, queryCount++);
	}
}

/* Obtains query results for the compacted size of BLAS structures, creates new BLAS's with the reduced size and copies the existing
   data to them reducing memory usage */
void Test::cmdCompactBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkQueryPool queryPool) {
	uint32_t queryCount{ 0 };

	//Obtain the queried compacted size result
	std::vector<VkDeviceSize> compactSizes(static_cast<uint32_t>(indices.size()));
	vkGetQueryPoolResults(Vulkan::getDevice()->getLogical(), queryPool, 0, (uint32_t) compactSizes.size(), compactSizes.size() * sizeof(VkDeviceSize), compactSizes.data(), sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT);

	//Go through each BLAS
	for (auto idx : indices) {
		//Previous accleration structure to destroy
		buildAs[idx].cleanupAS = buildAs[idx].as;
		//Assign the compacted size
		buildAs[idx].sizeInfo.accelerationStructureSize = compactSizes[queryCount++];

		//Assign AS create info with the new compacted size
		VkAccelerationStructureCreateInfoKHR asCreateInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
		asCreateInfo.size = buildAs[idx].sizeInfo.accelerationStructureSize;
		asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

		//Create a buffer to store the new AS
		buildAs[idx].as.buffer = new VulkanBuffer(asCreateInfo.size, Vulkan::getDevice(), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
		asCreateInfo.buffer = buildAs[idx].as.buffer->getInstance();
		//Create the compacted BLAS
		vkCreateAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), &asCreateInfo, nullptr, &buildAs[idx].as.accel);

		//Copy the original BLAS to a compact version
		VkCopyAccelerationStructureInfoKHR copyInfo{ VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR };

		std::cout << "HELLO" << std::endl;
		std::cout << buildAs[idx].buildInfo.dstAccelerationStructure << std::endl;
		std::cout << buildAs[idx].as.accel << std::endl;

		copyInfo.src = buildAs[idx].buildInfo.dstAccelerationStructure;
		copyInfo.dst = buildAs[idx].as.accel;
		copyInfo.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
		vkCmdCopyAccelerationStructureKHR(cmdBuf, &copyInfo);
	}
}

/* Function to cleanup after the use of 'cmdCompactBLAS' - destroying the old BLAS structuress
   TODO: Add culling */
void Test::destroyNonCompacted(std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs) {
	//Go through each cleanup BLAS and destroy them
	for (auto& i : indices) {
		vkDestroyAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), buildAs[i].cleanupAS.accel, nullptr);
		delete buildAs[i].cleanupAS.buffer;
	}
}

/* Create TLAS */
void Test::createTopLevelAS() {
	//AS instances that will reference the individual BLAS's
	std::vector<VkAccelerationStructureInstanceKHR> asInstances;
	asInstances.reserve(modelInstances.size());

	//Go through the models/BLAS's
	for (const ModelInstance& inst : modelInstances) {
		//Describe the current BLAS
		VkAccelerationStructureInstanceKHR rayInst{};
		rayInst.transform = toTransformMatrixKHR(inst.transform);  //Transform of the instance
		rayInst.instanceCustomIndex = inst.modelIndex;             //gl_InstanceCustomIndexEXT
		
		//Obtain the BLAS device address and assign it
		VkAccelerationStructureDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
		addressInfo.accelerationStructure = blas[inst.modelIndex].accel;

		rayInst.accelerationStructureReference = vkGetAccelerationStructureDeviceAddressKHR(Vulkan::getDevice()->getLogical(), &addressInfo);
		rayInst.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; //Disable culling on the model for now
		rayInst.mask  = 0xFF;                                                      //Only be hit if rayMask & instance.mask != 0
		rayInst.instanceShaderBindingTableRecordOffset = 0;                        //Use same hit group and therefore shader for all objects

		//Add the instance
		asInstances.emplace_back(rayInst);
	}
	std::cout << "TLAS" << std::endl;
	//Now build the TLAS using the above instance information
	buildTLAS(asInstances, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

/* Converts Matrix4f to VkTransformMatrixKHR while transposing since VkTransformMatrixKHR has a row-major memory layout */
VkTransformMatrixKHR Test::toTransformMatrixKHR(Matrix4f matrix) {
	//Transpose so can use memcpy to transfer the data directly
	Matrix4f temp = matrix.transpose();
	VkTransformMatrixKHR out_matrix;
	memcpy(&out_matrix, &temp, sizeof(VkTransformMatrixKHR));
	return out_matrix;
}

/* Builds a TLAS given the BLAS instances */
void Test::buildTLAS(const std::vector<VkAccelerationStructureInstanceKHR>& asInstances, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion) {
	//Should not call buildTlas twice except to update.
	assert(tlas.accel == VK_NULL_HANDLE || update);

	//Number of AS instances
	uint32_t countInstance = static_cast<uint32_t>(asInstances.size());

	//Command buffer to create the TLAS
	VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();

	//Create a buffer holding the instance data (containing the matrices and BLAS ids) for use by the AS builder and get its device address
	VulkanBuffer* instancesBuffer = new VulkanBuffer((void*) asInstances.data(), asInstances.size() * sizeof(asInstances[0]), Vulkan::getDevice(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, false);
	VkDeviceAddress instBufferAddr = getBufferDeviceAddress(instancesBuffer->getInstance());

	//Make sure the copy of the instance buffer has fininished before triggering the acceleration structure build
	//TODO: Use stanging in the above buffer????
	//      -  Probably need to use cmdBuf for copy involved in VulkanBuffer creation
	VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		0, 1, &barrier, 0, nullptr, 0, nullptr);

	//Scratch buffer to use for building the TLAS
	VulkanBuffer* scratchBuffer = NULL;

	//Create and build the TLAS
	cmdCreateTLAS(cmdBuf, countInstance, instBufferAddr, scratchBuffer, flags, update, motion);

	//Finished executing the commands to create the TLAS
	Vulkan::endSingleTimeCommands(cmdBuf);

	delete scratchBuffer;
	delete instancesBuffer;
}

/* Creates a TLAS given the number of BLASs, device address of the buffer holding the VkAccelerationStructureInstanceKHR instances and a scratch buffer
   (that should be destroyed after use) */
void Test::cmdCreateTLAS(VkCommandBuffer cmdBuf, uint32_t countInstance, VkDeviceAddress instBufferAddr, VulkanBuffer*& scratchBuffer, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion) {
	//Stores device adress to the uploaded instances
	VkAccelerationStructureGeometryInstancesDataKHR instancesVk{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR };
	instancesVk.data.deviceAddress = instBufferAddr;

	//Put the above into a VkAccelerationStructureGeometryKHR and label it as instance data.
	VkAccelerationStructureGeometryKHR topASGeometry{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	topASGeometry.geometryType       = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	topASGeometry.geometry.instances = instancesVk;

	//Structure required for obtaining the size info required for TLAS creation
	VkAccelerationStructureBuildGeometryInfoKHR buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
	buildInfo.flags         = flags;
	buildInfo.geometryCount = 1;
	buildInfo.pGeometries   = &topASGeometry;
	buildInfo.mode          = update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;

	//Obtain the build sizes
	VkAccelerationStructureBuildSizesInfoKHR sizeInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
	vkGetAccelerationStructureBuildSizesKHR(Vulkan::getDevice()->getLogical(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &countInstance, &sizeInfo);

	//Create info for the TLAS
	VkAccelerationStructureCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	createInfo.size = sizeInfo.accelerationStructureSize;

	//Create a buffer for the TLAS data and assign it in the create info
	tlas.buffer = new VulkanBuffer(createInfo.size, Vulkan::getDevice(), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	createInfo.buffer = tlas.buffer->getInstance();

	//Create the TLAS
	vkCreateAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), &createInfo, nullptr, &tlas.accel);

	//Allocate a scratch buffer for building the TLAS and obtain its device address
	scratchBuffer = new VulkanBuffer(sizeInfo.buildScratchSize, Vulkan::getDevice(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	VkDeviceAddress scratchAddress = getBufferDeviceAddress(scratchBuffer->getInstance());

	//Update the build information with the created TLAS and scratch buffer
	buildInfo.srcAccelerationStructure  = VK_NULL_HANDLE;
	buildInfo.dstAccelerationStructure  = tlas.accel;
	buildInfo.scratchData.deviceAddress = scratchAddress;

	//Build fffsets info
	VkAccelerationStructureBuildRangeInfoKHR        buildOffsetInfo{ countInstance, 0, 0, 0 };
	const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

	//Build the TLAS
	vkCmdBuildAccelerationStructuresKHR(cmdBuf, 1, &buildInfo, &pBuildOffsetInfo);
}

/* Create a storage image for the output from the raytracing (will then copy into swapchain to view) */
void Test::createStorageImage() {
	//Create the image and image view
	Vulkan::createImage(Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 1, 1, VK_SAMPLE_COUNT_1_BIT, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, storageImage.image, storageImage.memory);
	storageImage.view = Vulkan::createImageView(storageImage.image, VK_IMAGE_VIEW_TYPE_2D, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);

	//Transition the image into a suitable layout
	VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();
	Vulkan::transitionImageLayout(storageImage.image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1, cmdBuf);
	Vulkan::endSingleTimeCommands(cmdBuf);

	//Create the instance for storing the above
	storageTexture = new Texture(Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, storageImage.image, storageImage.memory, storageImage.view);
}

/* Creates the raytracing pipeline */
void Test::createRtPipeline() {
	//enum StageIndices {
	//	eRaygen,
	//	eMiss,
	//	eClosestHit,
	//	eShaderGroupCount
	//};

	////All stages
	//std::vector<VkPipelineShaderStageCreateInfo> stages{};
	//stages.resize(eShaderGroupCount);
	//VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	//stage.pName = "main";  //All the same entry point
	////Raygen
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rgen.spv"));
	//stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	//stages[eRaygen] = stage;
	////Miss
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rmiss.spv"));
	//stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	//stages[eMiss] = stage;
	////The second miss shader is invoked when a shadow ray misses the geometry. It simply indicates that no occlusion has been found
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytraceShadow.rmiss.spv"));
	//stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	//stages[eMiss] = stage;
	////Hit Group - Closest Hit
	//stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/spv/raytrace.rchit.spv"));
	//stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	//stages[eClosestHit] = stage;

	enum StageIndices {
		eRaygen,
		eMiss,
		eClosestHit,
		eShaderGroupCount
	};

	//All stages
	std::vector<VkPipelineShaderStageCreateInfo> stages{};
	stages.resize(eShaderGroupCount);
	VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	stage.pName = "main";  //All the same entry point
	//Raygen
	stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/raygen.rgen.spv"));
	stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	stages[eRaygen] = stage;
	//Miss
	stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/miss.rmiss.spv"));
	stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	stages[eMiss] = stage;
	//Hit Group - Closest Hit
	stage.module = Shader::createVkShaderModule(Shader::readFile("resources/shaders/raytracing/closesthit.rchit.spv"));
	stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	stages[eClosestHit] = stage;

	//Shader groups
	VkRayTracingShaderGroupCreateInfoKHR group{ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
	group.anyHitShader       = VK_SHADER_UNUSED_KHR;
	group.closestHitShader   = VK_SHADER_UNUSED_KHR;
	group.generalShader      = VK_SHADER_UNUSED_KHR;
	group.intersectionShader = VK_SHADER_UNUSED_KHR;

	//Raygen
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eRaygen;
	shaderGroups.push_back(group);

	//Miss
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss;
	shaderGroups.push_back(group);

	//closest hit shader
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = eClosestHit;
	shaderGroups.push_back(group);

	////Push constants
	//VkPushConstantRange pushConstant{ VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR, 0, sizeof(PushConstantRay) };

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	//pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	//pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	//Descriptor sets for the pipeline - one for the camera (set 0) and anoher for the TLAS/storage image
	//Needs to be in order of set numbers
	std::vector<VkDescriptorSetLayout> rtDescSetLayouts = { Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_CAMERA)->getVkLayout(), Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_RAYTRACING)->getVkLayout() };
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(rtDescSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts    = rtDescSetLayouts.data();

	//Create the pipeline layout
	vkCreatePipelineLayout(Vulkan::getDevice()->getLogical(), &pipelineLayoutCreateInfo, nullptr, &rtPipelineLayout);

	//Assign the shader stages and recursion depth info
	VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
	rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());  //Stages are shaders
	rayPipelineInfo.pStages    = stages.data();

	//In this case shaderGroups == 3 - one raygen group, one miss shader group, one hit group
	rayPipelineInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
	rayPipelineInfo.pGroups    = shaderGroups.data();

	//Ray depth cannot exceed rtProperties.maxRayRecursionDepth
	rayPipelineInfo.maxPipelineRayRecursionDepth = 1;
	rayPipelineInfo.layout = rtPipelineLayout;

	//Create the raytracing pipeline
	vkCreateRayTracingPipelinesKHR(Vulkan::getDevice()->getLogical(), {}, {}, 1, &rayPipelineInfo, nullptr, &rtPipeline);

	//Shader modules not needed anymore
	for (unsigned i = 0; i < stages.size(); ++i)
		vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), stages[i].module, nullptr);
}

/* Create the shader binding table
   - All shaders must be accessible at once when raytracing
     This allows the corect shader to be selected at runtime */
void Test::createRtShaderBindingTable() {
	//Number of each shader
	uint32_t missCount{ 1 };
	uint32_t hitCount{ 1 };
	auto     handleCount = 1 + missCount + hitCount; //Always only 1 raygen shader
	uint32_t handleSize = rtProperties.shaderGroupHandleSize;

	//Need to use align_up here as there is no guarentee the alignment corresponds to the handle or group size
	//Hardware with a smaller handle size than alignment can end up interleaving some shaderRecordEXT data without
	//additional memory usage
	uint32_t handleSizeAligned = align_up(handleSize, rtProperties.shaderGroupHandleAlignment);

	rgenRegion.stride = align_up(handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	rgenRegion.size   = rgenRegion.stride;  //This member of pRayGenShaderBindingTable must be equal to its stride member
	missRegion.stride = handleSizeAligned;
	missRegion.size   = align_up(missCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	hitRegion.stride  = handleSizeAligned;
	hitRegion.size    = align_up(hitCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);

	//Obtain the shader group handles
	uint32_t dataSize = handleCount * handleSize;
	std::vector<uint8_t> handles(dataSize);
	auto result = vkGetRayTracingShaderGroupHandlesKHR(Vulkan::getDevice()->getLogical(), rtPipeline, 0, handleCount, dataSize, handles.data());
	assert(result == VK_SUCCESS);

	//Allocate a buffer for storing the SBT.
	VkDeviceSize sbtSize = rgenRegion.size + missRegion.size + hitRegion.size + callRegion.size;
	rtSBTBuffer = new VulkanBuffer(sbtSize, Vulkan::getDevice(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);

	//Find the SBT addresses of each group (requires VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT in the above)
	VkDeviceAddress sbtAddress = getBufferDeviceAddress(rtSBTBuffer->getInstance());
	rgenRegion.deviceAddress   = sbtAddress;
	missRegion.deviceAddress   = sbtAddress + rgenRegion.size;
	hitRegion.deviceAddress    = sbtAddress + rgenRegion.size + missRegion.size;

	//Returns pointer to the previously retrieved handle (used for copying data into the SBT buffer)
	auto getHandle = [&](int i) { return handles.data() + i * handleSize; };

	//Map the SBT buffer to write the data
	void* pSBTBufferVoid;
	vkMapMemory(Vulkan::getDevice()->getLogical(), rtSBTBuffer->getDeviceMemory(), 0, sbtSize, 0, &pSBTBufferVoid); //Data is now mapped

	auto* pSBTBuffer = reinterpret_cast<uint8_t*>(pSBTBufferVoid);
	uint8_t* pData{ nullptr };
	uint32_t handleIdx{ 0 };

	//Raygen
	pData = pSBTBuffer;
	memcpy(pData, getHandle(handleIdx++), handleSize);

	//Miss
	pData = pSBTBuffer + rgenRegion.size;
	for (uint32_t c = 0; c < missCount; ++c) {
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += missRegion.stride;
	}

	//Hit
	pData = pSBTBuffer + rgenRegion.size + missRegion.size;
	for (uint32_t c = 0; c < hitCount; ++c) {
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += hitRegion.stride;
	}
	
	//Finished so unmap
	vkUnmapMemory(Vulkan::getDevice()->getLogical(), rtSBTBuffer->getDeviceMemory());
}

/* Runs raytracing commands */
void Test::raytrace() {
	//Initialize push constant values
	//pcRay.clearColor     = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	//pcRay.lightPosition  = Vector3f(1.0f, 0.0f, 0.0f);
	//pcRay.lightIntensity = 10;
	//pcRay.lightType      = 1;

	//Bind the RT pipeline and required descriptor sets
	vkCmdBindPipeline(Vulkan::getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline);
	camera->getDescriptorSet()->bind(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipelineLayout);
	raytracingDescriptorSet->bind(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipelineLayout);
	//vkCmdPushConstants(Vulkan::getCurrentCommandBuffer(), rtPipelineLayout,
	//	VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR,
	//	0, sizeof(PushConstantRay), &pcRay);

	//Issue RT command
	//Grid size is window width/height - trace 1 ray per pixel
	vkCmdTraceRaysKHR(Vulkan::getCurrentCommandBuffer(), &rgenRegion, &missRegion, &hitRegion, &callRegion, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 1);
}

void Test::onCreated() {
	std::string glslandValidatorPath = "C:/VulkanSDK/1.3.204.1/Bin/glslangValidator.exe";

	//Renderer::compileEngineShadersToSPIRV(glslandValidatorPath);

	//Renderer::compileEngineShaderToSPIRV(Renderer::SHADER_MATERIAL, glslandValidatorPath);

	if (getSettings().videoRaytracing)
		initRaytracing();

	//model1 = new GameObject3D(resourceLoader.loadModel("", "cube.obj"), Renderer::SHADER_MATERIAL);
	//model1 = new GameObject3D(resourceLoader.loadPBRModel("crytek-sponza/", "sponza.obj"), Renderer::SHADER_MATERIAL);
	//model1 = new GameObject3D(resourceLoader.loadModel("bob/", "bob_lamp_update.model"), Renderer::SHADER_MATERIAL);
	model1 = new GameObject3D(resourceLoader.loadPBRModel("box/", "CornellBox-Glossy.obj"), Renderer::SHADER_MATERIAL);

	//model1 = new GameObject3D(resourceLoader.loadModel("", "buddha.obj"), Renderer::SHADER_MATERIAL);
	std::cout << model1->getMesh()->getNumMaterials() << std::endl;
	model1->update();
	//model2 = new GameObject3D(resourceLoader.loadModel("", "cube.obj"), Renderer::SHADER_MATERIAL);
	//model2->update();

	camera->setFlying(true);
	camera->setPosition(Vector3f(1.0f, 2.0f, 4.0f));
	camera->update(getDeltaSeconds());

	if (getSettings().videoRaytracing) {
		modelInstances.push_back({ Matrix4f().initIdentity(), 0 });
		//modelInstances.push_back({ Matrix4f().initIdentity(), 1 });

		modelObjects.push_back(model1);
		//modelObjects.push_back(model2);

		createBottomLevelAS();

		createTopLevelAS();

		createStorageImage();

		raytracingDescriptorSet = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_RAYTRACING));
		raytracingDescriptorSet->setupVk();
		raytracingDescriptorSet->updateVkRaytracing(&tlas.accel, storageImage.view);

		createRtPipeline();

		createRtShaderBindingTable();

		////Setup the screen texture mesh
		//MeshData* meshData4 = createScreenMeshData();
		//screenTextureMesh = new Mesh(meshData4);
		////screenTextureMesh->getMaterial()->setDiffuse(storageImage);

		//screenTextureMesh->getMaterial()->setDiffuse(storageTexture);
		//screenTextureMesh->setup(Renderer::getRenderShader(Renderer::SHADER_MATERIAL));

		//screenRenderPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_MATERIAL), Renderer::getDefaultRenderPass());
	} else
		materialPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_MATERIAL), Renderer::getDefaultRenderPass());
}

void Test::onUpdate() {
	
}

void Test::onRenderOffscreen() {
	if (getSettings().videoRaytracing) {
		//Vulkan::transitionImageLayout(storageImage.image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 1, 1, Vulkan::getCurrentCommandBuffer());

		//Raytrace
		raytrace();

		//Copy ouput to swap chain (ensuring correct image layouts are used)
		Vulkan::transitionImageLayout(Vulkan::getSwapChain()->getCurrentImage(Vulkan::getCurrentFrame()), Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1, Vulkan::getCurrentCommandBuffer());
		Vulkan::transitionImageLayout(storageImage.image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 1, Vulkan::getCurrentCommandBuffer());

		VkImageCopy copyRegion{};
		copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copyRegion.srcOffset      = { 0, 0, 0 };
		copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copyRegion.dstOffset      = { 0, 0, 0 };
		copyRegion.extent         = { Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 1 };
		vkCmdCopyImage(Vulkan::getCurrentCommandBuffer(), storageImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Vulkan::getSwapChain()->getCurrentImage(Vulkan::getCurrentFrame()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		Vulkan::transitionImageLayout(storageImage.image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 1, 1, Vulkan::getCurrentCommandBuffer());
		Vulkan::transitionImageLayout(Vulkan::getSwapChain()->getCurrentImage(Vulkan::getCurrentFrame()), Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 1, Vulkan::getCurrentCommandBuffer());
		
		//Vulkan::transitionImageLayout(storageImage.image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, Vulkan::getCurrentCommandBuffer());
	}
}

void Test::onRender() {
	if (!getSettings().videoRaytracing) {
		materialPipeline->bind();
		camera->useView();
		model1->render();
	} else {
		//screenRenderPipeline->bind();
		//camera->useView();
		//Matrix4f identity = Matrix4f().initIdentity();
		//Renderer::render(screenTextureMesh, identity, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
	}
}

void Test::onDestroy() {
	delete model1;
	if (getSettings().videoRaytracing) {
		Vulkan::waitDeviceIdle();

		delete rtSBTBuffer;

		delete raytracingDescriptorSet;
		vkDestroyPipeline(Vulkan::getDevice()->getLogical(), rtPipeline, nullptr);
		vkDestroyPipelineLayout(Vulkan::getDevice()->getLogical(), rtPipelineLayout, nullptr);

		delete storageTexture;

		//Destroy the TLAS
		vkDestroyAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), tlas.accel, nullptr);
		delete tlas.buffer;

		//Go through and destroy the BLAS instances
		for (unsigned int i = 0; i < blas.size(); ++i) {
			vkDestroyAccelerationStructureKHR(Vulkan::getDevice()->getLogical(), blas[i].accel, nullptr);
			delete blas[i].buffer;
		}
	}
}

void Test::onKeyPressed(int key) {
	BaseTest3D::onKeyPressed(key);
}

MeshData* Test::createScreenMeshData() {
	MeshData* meshData = new MeshData(MeshData::DIMENSIONS_3D);
	meshData->addPosition(Vector3f(-1.0f, 1.0f, 0.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector3f(-1.0f, -1.0f, 0.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
	meshData->addPosition(Vector3f(1.0f, -1.0f, 0.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector3f(-1.0f, 1.0f, 0.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector3f(1.0f, -1.0f, 0.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector3f(1.0f, 1.0f, 0.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
	return meshData;
}