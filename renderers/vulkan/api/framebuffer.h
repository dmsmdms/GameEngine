#pragma once

#include <vulkan/vulkan.h>

extern VkFramebuffer swapchainFramebuffers[];

void vkCreateFramebufferWrap(const VkImageView imageView, VkFramebuffer * const framebuffer);
void vkCreateSwapchainFramebuffersWrap(void);
void vkDestroySwapchainFramebuffersWrap(void);
