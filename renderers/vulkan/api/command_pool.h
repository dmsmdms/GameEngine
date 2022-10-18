#pragma once

#include <vulkan/vulkan.h>

extern VkCommandPool mainRenderCommandPool;
extern VkCommandPool mainTransferCommandPool;

void vkCreateCommandPoolWrap(const uint_fast8_t index, VkCommandPool * const commandPool);
void vkCreateCommandPoolsWrap(void);
void vkDestroyCommandPoolsWrap(void);
void vkResetRenderCommandPoolWrap(void);
