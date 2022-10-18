#pragma once

#include <vulkan/vulkan.h>

extern VkFence renderImageFences[];

void vkCreateFenceWrap(const VkDevice device, VkFence * const fence);
void vkDestroyFenceWrap(const VkDevice device, VkFence * const restrict fence);
void vkCreateFencesWrap(void);
void vkDestroyFencesWrap(void);
