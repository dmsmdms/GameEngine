#pragma once

#include <vulkan/vulkan.h>

extern VkCommandBuffer renderCommandBuffers[];

void vkAllocateCommandBuffersWrap(const VkDevice device, const VkCommandPool pool, const uint_fast8_t count,
								  VkCommandBuffer * const buffers);
void vkCreateRenderCommandBuffersWrap(void);
