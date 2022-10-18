#pragma once

#include <vulkan/vulkan.h>

extern VkPipelineCache mainPipelineCache;

void vkCreatePipelineCacheWrap(void);
void vkDestroyPipelineCacheWrap(void);
