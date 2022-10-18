#pragma once

#include <vulkan/vulkan.h>

typedef struct {
	VkBuffer * restrict buffer;
	VkBufferUsageFlags flags;
	uint_fast32_t size;
	uint_fast32_t offset;
} VkBufferInfo;

extern VkBuffer vertexBuffer;
extern VkBuffer indexBuffer;

void vkCreateBufferWrap(const VkDevice device, const VkBufferUsageFlags flags, const uint_fast32_t size,
						VkBuffer * const buffer);
void vkCreateBufferGroupWrap(const VkDevice device, VkBufferInfo * const restrict infos,
							 const uint_fast8_t length, VkDeviceMemory * const restrict memory,
							 const uint_fast8_t memoryType);
void vkCreateVertexBufferGroupWrap(void);
void vkDestroyVertexBufferGroupWrap(void);
