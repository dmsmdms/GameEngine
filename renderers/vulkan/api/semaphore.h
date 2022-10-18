#pragma once

#include <vulkan/vulkan.h>

extern VkSemaphore swapchainImageSemaphores[];
extern VkSemaphore renderImageSemaphores[];

void vkCreateSemaphoreWrap(const VkDevice device, VkSemaphore * const pSemaphore);
void vkDestroySemaphoreWrap(const VkDevice device, VkSemaphore * const restrict pSemaphore);
void vkCreateSemaphoresWrap(void);
void vkDestroySemaphoresWrap(void);
