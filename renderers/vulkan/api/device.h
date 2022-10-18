#pragma once

#include <vulkan/vulkan.h>

extern uint_fast8_t mainRenderQueueIndex;
extern uint_fast8_t mainTransferQueueIndex;
extern VkDevice mainDevice;
extern VkQueue mainRenderQueue;
extern VkQueue mainTransferQueue;
extern VkQueue presentQueue;

void vkCreateDeviceWrap(void);
void vkDestroyDeviceWrap(void);
void vkDeviceWaitIdleWrap(void);
