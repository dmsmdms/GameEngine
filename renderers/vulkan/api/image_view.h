#pragma once

#include <vulkan/vulkan.h>

extern VkImageView swapchainImageViews[];

void vkCreateImageViewSimpleWrap(const VkDevice device, const VkImage image, const VkFormat format,
								 const VkImageAspectFlags aspectFlags, VkImageView * const restrict imageView);
void vkCreateSwapchainImageViewsWrap(void);
void vkDestroySwapchainImageViewsWrap(void);
