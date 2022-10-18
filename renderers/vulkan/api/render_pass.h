#pragma once

#include <vulkan/vulkan.h>

typedef enum {
	VK_SUBPASS_INDEX_MIN,
	VK_SUBPASS_INDEX_MAIN = VK_SUBPASS_INDEX_MIN,
	VK_SUBPASS_INDEX_MAX,
} VkSubpassIndex;

extern VkRenderPass renderPass;

void vkCreateRenderPassWrap(void);
void vkDestroyRenderPassWrap(void);
