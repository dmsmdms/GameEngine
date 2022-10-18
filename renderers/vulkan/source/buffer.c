#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/physical_device.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <vulkan/api/buffer.h>
#include <global/api/vertex.h>
#include <asserts.h>

#define VK_BUFFER_INFO(id, buf, bufFlags, bufSize)	\
[id] = {											\
	.buffer = buf,									\
	.flags = bufFlags,								\
	.size = bufSize,								\
	.offset = 0,									\
}

typedef enum {
	VK_VERTEX_ID_MIN,
	VK_VERTEX_ID_BUFFER_VERTEX = VK_VERTEX_ID_MIN,
	VK_VERTEX_ID_BUFFER_INDEX,
	VK_VERTEX_ID_MAX,
} VkVertexId;

static VkDeviceMemory vertexMemory = VK_NULL_HANDLE;
VkBuffer vertexBuffer = VK_NULL_HANDLE;
VkBuffer indexBuffer = VK_NULL_HANDLE;

static uint_fast32_t vkGetAlignedOffset(const uint_fast32_t base, const uint_fast32_t aligment) {
	return (base + aligment - 1) & ~(aligment - 1);
}

static void vkFillVertexBuffersWrap(const VkBufferInfo * const restrict infos) {
	const uint_fast32_t vertexOffset = infos[VK_VERTEX_ID_BUFFER_VERTEX].offset;
	const uint_fast32_t indexOffset = infos[VK_VERTEX_ID_BUFFER_INDEX].offset;
	const VkMemoryMapFlags flags = 0;
	const VkDeviceSize offset = 0;
	uint8_t * pVertexMemory;

	const VkResult result = vkMapMemory(mainDevice, vertexMemory, offset, VK_WHOLE_SIZE,
										flags, (void *)&pVertexMemory);
	RK_VULKAN_ASSERT(result, NULL);

	rkFillVertexBuffer((vertext_t *)(pVertexMemory + vertexOffset));
	rkFillIndexBuffer((uint16_t *)(pVertexMemory + indexOffset));

	vkUnmapMemory(mainDevice, vertexMemory);
}

void vkCreateBufferWrap(const VkDevice device, const VkBufferUsageFlags flags, const uint_fast32_t size,
						VkBuffer * const buffer)
{
	const VkBufferCreateInfo info = {
		.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext					= NULL,
		.flags					= 0,
		.size					= size,
		.usage					= flags,
		.sharingMode			= VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount	= 0,
		.pQueueFamilyIndices	= NULL,
	};

	const VkResult result = vkCreateBuffer(device, &info, allocationCallbacks, buffer);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkCreateBufferGroupWrap(const VkDevice device, VkBufferInfo * const restrict infos,
							 const uint_fast8_t length, VkDeviceMemory * const restrict memory,
							 const uint_fast8_t memoryType)
{
	uint_fast32_t memorySize = 0;

	for (uint_fast8_t i = 0; i < length; i++) {
		VkBufferInfo * const restrict info = &infos[i];
		VkMemoryRequirements requirements;

		vkCreateBufferWrap(device, info->flags, info->size, info->buffer);
		vkGetBufferMemoryRequirements(device, *info->buffer, &requirements);

		const uint_fast32_t checkMemoryType = requirements.memoryTypeBits & (1 << memoryType);
		RK_ASSERT(checkMemoryType == VK_FALSE, "Check buffer memory type! (id=%u, type=%u)",
				  (unsigned)i, (unsigned)memoryType);

		const uint_fast32_t offset = vkGetAlignedOffset(memorySize, requirements.alignment);
		memorySize = offset + requirements.size;
		info->offset = offset;
	}

	const VkMemoryAllocateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext				= NULL,
		.allocationSize		= memorySize,
		.memoryTypeIndex	= memoryType,
	};

	VkResult result = vkAllocateMemory(device, &info, allocationCallbacks, memory);
	RK_VULKAN_ASSERT(result, NULL);

	for (uint_fast8_t i = 0; i < length; i++) {
		const VkBufferInfo * const restrict info = &infos[i];
		result = vkBindBufferMemory(device, *info->buffer, *memory, info->offset);
		RK_VULKAN_ASSERT(result, NULL);
	}
}

void vkCreateVertexBufferGroupWrap(void) {
	VkBufferInfo vertexInfos[] = {
		VK_BUFFER_INFO(VK_VERTEX_ID_BUFFER_VERTEX, &vertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			vertexCount * sizeof(vertext_t)),
		VK_BUFFER_INFO(VK_VERTEX_ID_BUFFER_INDEX, &indexBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			indexCount * sizeof(uint16_t)),
	};

	const uint_fast8_t hasLocal = mainDeviceInfo.memoryInfo.hasLocal;
	const uint_fast8_t localId = mainDeviceInfo.memoryInfo.localId;
	const uint_fast8_t localHostId = mainDeviceInfo.memoryInfo.localHostId;
	const uint_fast8_t memoryType = (hasLocal ? localId : localHostId);

	vkCreateBufferGroupWrap(mainDevice, vertexInfos, ARRAY_LEN(vertexInfos), &vertexMemory, memoryType);
	vkFillVertexBuffersWrap(vertexInfos);

	vkLog(VULKAN_TAG_INFO, "Vertex buffer group created!");
}

void vkDestroyVertexBufferGroupWrap(void) {
	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(mainDevice, vertexBuffer, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Vertex buffer destroyed!");
		vertexBuffer = VK_NULL_HANDLE;
	}

	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(mainDevice, indexBuffer, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Index buffer destroyed!");
		indexBuffer = VK_NULL_HANDLE;
	}

	if (vertexMemory != VK_NULL_HANDLE) {
		vkFreeMemory(mainDevice, vertexMemory, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Vertex memory freed!");
		vertexMemory = VK_NULL_HANDLE;
	}
}
