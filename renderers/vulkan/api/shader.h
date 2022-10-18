#pragma once

#include <vulkan/vulkan.h>

extern VkShaderModule vertexShader;
extern VkShaderModule fragmentShader;

void vkCreateShaderModulesWrap(void);
void vkDestroyShaderModulesWrap(void);
