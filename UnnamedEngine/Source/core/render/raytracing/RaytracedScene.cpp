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

#include "RaytracedScene.h"

#include <cassert>

#include "../../vulkan/Vulkan.h"
#include "../../vulkan/VulkanExtensions.h"

 /*****************************************************************************
  * The RaytracedScene class
  *****************************************************************************/

RaytracedScene::RaytracedScene(bool lighting) : lighting(lighting) {
	//Obtain the current physical device and information about its raytracing capabilities
	this->device = Vulkan::getDevice();

	initRaytracing();
}

RaytracedScene::~RaytracedScene() {
	delete rtDescriptorSet;
	delete rtDescriptorSetLayout;
	delete rtPipeline;

	for (unsigned int i = 0; i < lights.size(); ++i)
		delete lights[i];

	if (pbrEnvironment)
		delete pbrEnvironment;

	delete materialDataBuffer;
	delete storageTexture;

	delete screenRenderMesh;
	delete screenRenderPipeline;

	//Destroy the TLAS
	VulkanExtensions::vkDestroyAccelerationStructureKHR(device->getLogical(), tlas.accel, nullptr);
	delete tlas.buffer;

	//Go through and destroy the BLAS instances
	for (unsigned int i = 0; i < blas.size(); ++i) {
		VulkanExtensions::vkDestroyAccelerationStructureKHR(device->getLogical(), blas[i].accel, nullptr);
		delete blas[i].buffer;
	}

	//Delete all added objects
	for (GameObject3D* object : objects)
		delete object;
}

void RaytracedScene::initRaytracing() {
	//Obtain physical device properies for raytracing capabilities
	VkPhysicalDeviceProperties2 prop2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	prop2.pNext = &rtDeviceProperties;
	vkGetPhysicalDeviceProperties2(device->getPhysical(), &prop2);

	std::cout << "Max recursion depth: " + utils_string::str(rtDeviceProperties.maxRayRecursionDepth) << std::endl;
}

RaytracedScene::BLASInput RaytracedScene::objectToVkGeometryKHR(Mesh* mesh, MeshData::SubData& subData) {
	//Obtain the number of indices and triangels making up the mesh
	uint32_t numIndices   = mesh->getData()->getNumIndices();
	uint32_t numTriangles = numIndices / 3;

	//BLAS builder requires raw device addresses
	VkDeviceAddress vertexAddress = Vulkan::getBufferDeviceAddress(mesh->getRenderData()->getVBOOthers()->getVkCurrentBuffer()->getInstance());
	VkDeviceAddress indexAddress  = Vulkan::getBufferDeviceAddress(mesh->getRenderData()->getIBO()->getVkCurrentBuffer()->getInstance());

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
	//TODO: Update this after adding sub data
	triangles.maxVertex = numIndices; //Number of vertices

	//Create the BLASInput - can add more geometry in each BLAS, only one for now
	//TODO: add more for handling multiple materials
	BLASInput input{};

	//Define the above data data as containing opaque triangles
	VkAccelerationStructureGeometryKHR asGeom{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	asGeom.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags              = VK_GEOMETRY_OPAQUE_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	//For now the entire array will be used to build the BLAS
	VkAccelerationStructureBuildRangeInfoKHR offset;
	offset.firstVertex     = subData.baseVertex; //Vertex offset
	offset.primitiveCount  = subData.count / 3;  //Number of triangles - this * 3 indices are consumed
	offset.primitiveOffset = (subData.baseIndex * sizeof(unsigned int)); //Offset in indices (in bytes)
	offset.transformOffset = 0;

	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);

	return input;
}

void RaytracedScene::buildBLAS(const std::vector<BLASInput>& input, VkBuildAccelerationStructureFlagsKHR flags) {
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
		VulkanExtensions::vkGetAccelerationStructureBuildSizesKHR(device->getLogical(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildAs[idx].buildInfo, maxPrimCount.data(), &buildAs[idx].sizeInfo);

		//Size needed for the acceleration structure
		asTotalSize += buildAs[idx].sizeInfo.accelerationStructureSize;
		//Scratch size for the largest BLAS (allows same buffer to be used for multiple)
		maxScratchSize = std::max(maxScratchSize, buildAs[idx].sizeInfo.buildScratchSize);
		numCompactions += hasFlag(buildAs[idx].buildInfo.flags, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
	}

	//Create the scratch buffer and obtain its device address
	VulkanBuffer* scratchBuffer = new VulkanBuffer(maxScratchSize, device, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	VkDeviceAddress scratchAddress = Vulkan::getBufferDeviceAddress(scratchBuffer->getInstance());

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
		vkCreateQueryPool(device->getLogical(), &qpCreateInfo, nullptr, &queryPool);
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
		vkDestroyQueryPool(device->getLogical(), queryPool, nullptr);
	delete scratchBuffer;
}

void RaytracedScene::cmdCreateBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkDeviceAddress scratchAddress, VkQueryPool queryPool) {
	//Reset the query pool for querying the compaction size
	if (queryPool)
		vkResetQueryPool(device->getLogical(), queryPool, 0, static_cast<uint32_t>(indices.size()));
	uint32_t queryCount{ 0 };

	//Go through each BLAS to create
	for (const auto& idx : indices) {
		//Setup the data required for the creation of the BLAS
		VkAccelerationStructureCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		createInfo.size = buildAs[idx].sizeInfo.accelerationStructureSize;  //Will be used to allocate memory

		//Create a buffer to store the acceleration structure and assign it in the create info
		buildAs[idx].as.buffer = new VulkanBuffer(createInfo.size, device, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
		createInfo.buffer = buildAs[idx].as.buffer->getInstance();

		//Create the BLAS
		VulkanExtensions::vkCreateAccelerationStructureKHR(device->getLogical(), &createInfo, nullptr, &buildAs[idx].as.accel);

		//Setup the additional build info required for building the BLAS (the rest is done in buildBLAS)
		buildAs[idx].buildInfo.dstAccelerationStructure  = buildAs[idx].as.accel; //Where to store the acceleration structure
		buildAs[idx].buildInfo.scratchData.deviceAddress = scratchAddress;        //Scratch buffer

		//Build the BLAS
		VulkanExtensions::vkCmdBuildAccelerationStructuresKHR(cmdBuf, 1, &buildAs[idx].buildInfo, &buildAs[idx].rangeInfo);

		//Since the scratch buffer is reused across builds need a memory barrier to ensure each build has finished before starting the next one.
		VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);

		//Pass the AS info ready to query the actual memory required by the BLAS - this will be used for compaction
		//requires the AS - &buildAs[idx].buildInfo.dstAccelerationStructure = buildAs[idx].as.accel above
		if (queryPool)
			VulkanExtensions::vkCmdWriteAccelerationStructuresPropertiesKHR(cmdBuf, 1, &buildAs[idx].buildInfo.dstAccelerationStructure, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR, queryPool, queryCount++);
	}
}

void RaytracedScene::cmdCompactBLAS(VkCommandBuffer cmdBuf, std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs, VkQueryPool queryPool) {
	uint32_t queryCount{ 0 };

	//Obtain the queried compacted size result
	std::vector<VkDeviceSize> compactSizes(static_cast<uint32_t>(indices.size()));
	vkGetQueryPoolResults(device->getLogical(), queryPool, 0, (uint32_t)compactSizes.size(), compactSizes.size() * sizeof(VkDeviceSize), compactSizes.data(), sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT);

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
		buildAs[idx].as.buffer = new VulkanBuffer(asCreateInfo.size, device, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
		asCreateInfo.buffer = buildAs[idx].as.buffer->getInstance();
		//Create the compacted BLAS
		VulkanExtensions::vkCreateAccelerationStructureKHR(device->getLogical(), &asCreateInfo, nullptr, &buildAs[idx].as.accel);

		//Copy the original BLAS to a compact version
		VkCopyAccelerationStructureInfoKHR copyInfo{ VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR };

		copyInfo.src  = buildAs[idx].buildInfo.dstAccelerationStructure;
		copyInfo.dst  = buildAs[idx].as.accel;
		copyInfo.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
		VulkanExtensions::vkCmdCopyAccelerationStructureKHR(cmdBuf, &copyInfo);
	}
}

void RaytracedScene::destroyNonCompacted(std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs) {
	//Go through each cleanup BLAS and destroy them
	for (auto& i : indices) {
		VulkanExtensions::vkDestroyAccelerationStructureKHR(device->getLogical(), buildAs[i].cleanupAS.accel, nullptr);
		delete buildAs[i].cleanupAS.buffer;
	}
}

/* Creates the BLAS using the above method */
void RaytracedScene::setupAllBLAS() {
	//BLAS objects for the scene
	std::vector<BLASInput> allBLAS;
	//Create the BLASInput instances - assume one BLAS per model for now
	allBLAS.reserve(objects.size());
	for (GameObject3D* model : objects) {
		for (unsigned int i = 0; i < model->getMesh()->getData()->getSubDataCount(); ++i) {
			allBLAS.emplace_back(objectToVkGeometryKHR(model->getMesh(), model->getMesh()->getData()->getSubData(i)));
		}
	}

	//Create the BLAS instances using the above input
	buildBLAS(allBLAS, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR); //VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR is optional
}

VkTransformMatrixKHR RaytracedScene::toTransformMatrixKHR(Matrix4f matrix) {
	//Transpose so can use memcpy to transfer the data directly
	Matrix4f temp = matrix.transpose();
	VkTransformMatrixKHR out_matrix;
	memcpy(&out_matrix, &temp, sizeof(VkTransformMatrixKHR));
	return out_matrix;
}

void RaytracedScene::buildTLAS(const std::vector<VkAccelerationStructureInstanceKHR>& asInstances, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion) {
	//Should not call buildTlas twice except to update.
	assert(tlas.accel == VK_NULL_HANDLE || update);

	//Number of AS instances
	uint32_t countInstance = static_cast<uint32_t>(asInstances.size());

	//Command buffer to create the TLAS
	VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();

	//Create a buffer holding the instance data (containing the matrices and BLAS ids) for use by the AS builder and get its device address
	VulkanBuffer* instancesBuffer = new VulkanBuffer((void*)asInstances.data(), asInstances.size() * sizeof(asInstances[0]), device, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, false);
	VkDeviceAddress instBufferAddr = Vulkan::getBufferDeviceAddress(instancesBuffer->getInstance());

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
void RaytracedScene::cmdCreateTLAS(VkCommandBuffer cmdBuf, uint32_t countInstance, VkDeviceAddress instBufferAddr, VulkanBuffer*& scratchBuffer, VkBuildAccelerationStructureFlagsKHR flags, bool update, bool motion) {
	//Stores device adress to the uploaded instances
	VkAccelerationStructureGeometryInstancesDataKHR instancesVk{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR };
	instancesVk.data.deviceAddress = instBufferAddr;

	//Put the above into a VkAccelerationStructureGeometryKHR and label it as instance data.
	VkAccelerationStructureGeometryKHR topASGeometry{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	topASGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
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
	VulkanExtensions::vkGetAccelerationStructureBuildSizesKHR(device->getLogical(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &countInstance, &sizeInfo);

	//Create info for the TLAS
	VkAccelerationStructureCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
	createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	createInfo.size = sizeInfo.accelerationStructureSize;

	//Create a buffer for the TLAS data and assign it in the create info
	tlas.buffer = new VulkanBuffer(createInfo.size, device, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	createInfo.buffer = tlas.buffer->getInstance();

	//Create the TLAS
	VulkanExtensions::vkCreateAccelerationStructureKHR(device->getLogical(), &createInfo, nullptr, &tlas.accel);

	//Allocate a scratch buffer for building the TLAS and obtain its device address
	scratchBuffer = new VulkanBuffer(sizeInfo.buildScratchSize, device, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	VkDeviceAddress scratchAddress = Vulkan::getBufferDeviceAddress(scratchBuffer->getInstance());

	//Update the build information with the created TLAS and scratch buffer
	buildInfo.srcAccelerationStructure  = VK_NULL_HANDLE;
	buildInfo.dstAccelerationStructure  = tlas.accel;
	buildInfo.scratchData.deviceAddress = scratchAddress;

	//Build fffsets info
	VkAccelerationStructureBuildRangeInfoKHR        buildOffsetInfo{ countInstance, 0, 0, 0 };
	const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

	//Build the TLAS
	VulkanExtensions::vkCmdBuildAccelerationStructuresKHR(cmdBuf, 1, &buildInfo, &pBuildOffsetInfo);
}

/* TODO: Add culling */
void RaytracedScene::setupTLAS() {
	//AS instances that will reference the individual BLAS's
	std::vector<VkAccelerationStructureInstanceKHR> asInstances;
	asInstances.reserve(objects.size());

	//Current object index
	unsigned int objectIndex = 0;

	//Go through the models/BLAS's
	for (GameObject3D* inst : objects) {
		for (unsigned int i = 0; i < inst->getMesh()->getData()->getSubDataCount(); ++i) {
			//Describe the current BLAS
			VkAccelerationStructureInstanceKHR rayInst{};
			rayInst.transform           = toTransformMatrixKHR(inst->getModelMatrix());  //Transform of the instance
			rayInst.instanceCustomIndex = objectIndex;                                   //gl_InstanceCustomIndexEXT

			//Obtain the BLAS device address and assign it
			VkAccelerationStructureDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
			addressInfo.accelerationStructure = blas[objectIndex].accel;

			rayInst.accelerationStructureReference = VulkanExtensions::vkGetAccelerationStructureDeviceAddressKHR(device->getLogical(), &addressInfo);
			rayInst.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; //Disable culling on the model for now
			rayInst.mask  = 0xFF;                                                      //Only be hit if rayMask & instance.mask != 0
			rayInst.instanceShaderBindingTableRecordOffset = 0;                        //Use same hit group and therefore shader for all objects

			//Add the instance
			asInstances.emplace_back(rayInst);

			objectIndex++;
		}
	}
	//Now build the TLAS using the above instance information
	buildTLAS(asInstances, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

void RaytracedScene::createStorageTexture() {
	VkDeviceMemory memory;
	VkImage image;
	VkImageView view;
	VkFormat format;

	//Create the image and image view
	Vulkan::createImage(Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 1, 1, VK_SAMPLE_COUNT_1_BIT, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
	view = Vulkan::createImageView(image, VK_IMAGE_VIEW_TYPE_2D, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);

	//Transition the image into a suitable layout
	VkCommandBuffer cmdBuf = Vulkan::beginSingleTimeCommands();
	Vulkan::transitionImageLayout(image, Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, cmdBuf);
	Vulkan::endSingleTimeCommands(cmdBuf);

	//Create the instance for storing the above
	storageTexture = new Texture(Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, image, memory, view);
}

void RaytracedScene::setupModelData() {
	//All of the material data for the scene
	std::vector<ShaderBlock_Material> materialData;

	//Go through each object and add the material data
	for (GameObject3D* object : objects) {
		for (Material* mat : object->getMesh()->getMaterials()) {
			materialData.push_back(mat->getShaderData());

			texturesAmbient.push_back(mat->getAmbientTexture());
			texturesDiffuse.push_back(mat->getDiffuseTexture());
			texturesSpecular.push_back(mat->getSpecularTexture());
			texturesShininess.push_back(mat->getShininessTexture());
			texturesNormalMap.push_back(mat->getNormalMap());
			texturesParallaxMap.push_back(mat->getParallaxMap());
			texturesEmissive.push_back(mat->getEmissiveTexture());
		}
	}

	//Create the material data buffer
	materialDataBuffer = new VulkanBuffer(materialData.data(), materialData.size() * sizeof(ShaderBlock_Material), device, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, false);

	//Current offset for offsetting the materials device address and textures
	//TODO: Can combine these
	unsigned int materialOffset = 0;
	unsigned int textureOffset = 0;

	//Go through each object
	for (GameObject3D* object : objects) {
		//Obtain the mesh/RenderData of the current object
		Mesh* mesh                 = object->getMesh();
		MeshRenderData* renderData = mesh->getRenderData();

		//Current offset for offsetting the offsets device address
		unsigned int offsetOffset = 0;

		//Create a model data for each SubData instance (one BLAS is created for each)
		for (unsigned int i = 0; i < mesh->getData()->getSubDataCount(); ++i) {
			//Model data instance
			ModelData data = {};

			//Asign the vertex/index buffer addresses
			data.vertexBufferAddress = Vulkan::getBufferDeviceAddress(renderData->getVBOOthers()->getVkCurrentBuffer()->getInstance());
			data.indexBufferAddress  = Vulkan::getBufferDeviceAddress(renderData->getIBO()->getVkCurrentBuffer()->getInstance());

			//Assing the material index/data buffer addresses
			//TODO: Need to add mat index offset as well so index in shader gives index in total materials
			//      (enables multiple objects to work)
			data.matIndexBufferAddress = Vulkan::getBufferDeviceAddress(renderData->getMaterialIndicesBuffer()->getInstance());
			data.matDataBufferAddress  = Vulkan::getBufferDeviceAddress(materialDataBuffer->getInstance()) + materialOffset;

			//Assign the offset indices buffer
			//Offset depending on the subdata that came before it (ensures can use gl_PrimitiveID to access the correct offsets)
			data.offsetIndicesBufferAddress = Vulkan::getBufferDeviceAddress(renderData->getOffsetIndicesBuffer()->getInstance()) + offsetOffset;
			data.textureOffset = textureOffset;

			//Add the model data
			sceneModelData.push_back(data);

			//Offset buffer address position to point at correct data (gl_PrimitiveID in shader resets to 0 for each subdata/material)
			offsetOffset += (mesh->getData()->getSubData(i).count / 3) * 2 * sizeof(mesh->getData()->getOffsetIndices()[0]);
		}
		//Increase the material buffer address and texture offset
		materialOffset += mesh->getNumMaterials() * sizeof(ShaderBlock_Material);
		textureOffset += mesh->getNumMaterials();
	}
}

void RaytracedScene::setup(Shader* rtShader, Camera3D* camera, PBREnvironment* pbrEnvironment) {
	//Assign the environment (may be NULL)
	this->pbrEnvironment = pbrEnvironment;

	//Setup the acceleration structures, model data and create the storage texture
	setupAllBLAS();
	setupTLAS();
	setupModelData();
	createStorageTexture();

	//Setup the raytracing descriptor set
	//TODO: Move into shader interface - but need to add way of adding UBO to layout without the size (bascially don't autocreate the UBO)
	rtDescriptorSetLayout = new DescriptorSetLayout(1);
	rtDescriptorSetLayout->addAccelerationStructure(0, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addStorageTexture(1, VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 2, texturesAmbient.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 3, texturesDiffuse.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 4, texturesSpecular.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 5, texturesShininess.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 6, texturesNormalMap.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 7, texturesParallaxMap.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	rtDescriptorSetLayout->addTextureBinding(DescriptorSet::TextureType::TEXTURE_2D, 8, texturesEmissive.size(), VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);

	if (pbrEnvironment) {
		rtDescriptorSetLayout->addTextureCube(9, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
		rtDescriptorSetLayout->addTextureCube(10, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
		rtDescriptorSetLayout->addTexture2D(11, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	}

	rtDescriptorSetLayout->addTexture2D(14, VK_SHADER_STAGE_MISS_BIT_KHR);
	rtDescriptorSetLayout->addSSBO(sceneModelData.size() * sizeof(sceneModelData[0]), DataUsage::STATIC, 2, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	if (lighting) {
		//Add lighting info
		rtDescriptorSetLayout->addUBO(lights.size() * sizeof(ShaderBlock_RaytracedLighting), DataUsage::STATIC, 3, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);

		//Setup the lighting info
		for (unsigned int i = 0; i < lights.size(); ++i) {
			lights[i]->setUniforms(rtLightingData.ue_lights[i]);
		}

		rtLightingData.ue_lightAmbient = Colour(0.01f, 0.01f, 0.01f);
		rtLightingData.ue_numLights = lights.size();
	}
	rtDescriptorSetLayout->setup();

	rtDescriptorSet = new DescriptorSet(rtDescriptorSetLayout, true);
	rtDescriptorSet->setTexture(0, storageTexture);
	
	//Add all of the textures
	unsigned int index = 1;
	for (unsigned int i = 0; i < texturesAmbient.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesAmbient[i]);
	for (unsigned int i = 0; i < texturesDiffuse.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesDiffuse[i]);
	for (unsigned int i = 0; i < texturesSpecular.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesSpecular[i]);
	for (unsigned int i = 0; i < texturesShininess.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesShininess[i]);
	for (unsigned int i = 0; i < texturesNormalMap.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesNormalMap[i]);
	for (unsigned int i = 0; i < texturesParallaxMap.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesParallaxMap[i]);
	for (unsigned int i = 0; i < texturesEmissive.size(); ++i)
		rtDescriptorSet->setTexture(index++, texturesEmissive[i]);

	if (pbrEnvironment) {
		rtDescriptorSet->setTexture(index++, pbrEnvironment->getIrradianceCubemap());
		rtDescriptorSet->setTexture(index++, pbrEnvironment->getPrefilterCubemap());
		rtDescriptorSet->setTexture(index++, pbrEnvironment->getBRDFLUTTexture());
	}

	rtDescriptorSet->setTexture(index++, camera->getSkyBox()->getTexture());

	rtDescriptorSet->setAccclerationStructure(0, &tlas.accel);
	rtDescriptorSet->setupVk();

	rtDescriptorSet->getShaderBuffer(0)->update(sceneModelData.data(), 0, sceneModelData.size() * sizeof(sceneModelData[0]));

	if (lighting) {
		rtDescriptorSet->getShaderBuffer(1)->update(&rtLightingData, 0, sizeof(ShaderBlock_RaytracedLighting));
	}

	//Create the raytracing pipeline
	rtPipeline = new RaytracingPipeline(rtDeviceProperties, rtShader, rtDescriptorSetLayout);

	//Setup for rendering the output to the screen
	screenRenderPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_TEXTURE_PASSTHROUGH), Renderer::getDefaultRenderPass());

	screenRenderMesh = new Mesh(createScreenMeshData());
	screenRenderMesh->getMaterial()->setDiffuse(storageTexture);
	screenRenderMesh->setup(Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
}

void RaytracedScene::raytrace(Camera3D* camera) {
	//Update the current frame
	updateFrame(camera);

	//Transition the storage texture layout ready for modifying
	Vulkan::transitionImageLayout(storageTexture->getVkImage(), Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 1, 1, Vulkan::getCurrentCommandBuffer());

	//Bind the RT pipeline and camera/rt descriptor sets
	rtPipeline->bind(&rtPushConstants);

	//TODO: Remove these methods
	camera->getDescriptorSet()->bind(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->getLayout());
	rtDescriptorSet->bind(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->getLayout());

	//Issue RT command
	//Grid size is window width/height - trace 1 ray per pixel
	VulkanExtensions::vkCmdTraceRaysKHR(Vulkan::getCurrentCommandBuffer(), rtPipeline->getRGenRegion(), rtPipeline->getMissRegion(), rtPipeline->getHitRegion(), rtPipeline->getCallRegion(), Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight, 1);

	//Transition the storage texture layout ready reading in shader
	Vulkan::transitionImageLayout(storageTexture->getVkImage(), Vulkan::getSwapChain()->getSurfaceFormat(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, Vulkan::getCurrentCommandBuffer());
}

void RaytracedScene::renderOutput() {
	screenRenderPipeline->bind();
	Matrix4f identity = Matrix4f().initIdentity();

	Renderer::render(screenRenderMesh, identity, Renderer::getRenderShader(Renderer::SHADER_FRAMEBUFFER));
}

void RaytracedScene::updateFrame(Camera3D* camera) {
	Matrix4f& currentMatrix = camera->getViewMatrix();

	//Check for a change of the camera's view matrix
	if (memcmp(lastViewMatrix.data(), currentMatrix.data(), sizeof(Matrix4f)) != 0) {
		//Reset the frame as the camera has moved
		rtPushConstants.frame = -1;

		lastViewMatrix = currentMatrix;
	}

	//Increment the current frame
	rtPushConstants.frame++;
}

MeshData* RaytracedScene::createScreenMeshData() {
	MeshData* meshData = new MeshData(MeshData::DIMENSIONS_2D);
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(-1.0f, -1.0f)); meshData->addTextureCoord(Vector2f(0.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(-1.0f, 1.0f));  meshData->addTextureCoord(Vector2f(0.0f, 1.0f));
	meshData->addPosition(Vector2f(1.0f, -1.0f));  meshData->addTextureCoord(Vector2f(1.0f, 0.0f));
	meshData->addPosition(Vector2f(1.0f, 1.0f));   meshData->addTextureCoord(Vector2f(1.0f, 1.0f));
	return meshData;
}