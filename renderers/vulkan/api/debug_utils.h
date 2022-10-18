#pragma once

#include <vulkan/vulkan.h>

typedef enum {
	VULKAN_TAG_MIN,
	VULKAN_TAG_INFO = VULKAN_TAG_MIN,
	VULKAN_TAG_DEBUG,
	VULKAN_TAG_MAX,
} VulkanTag;

const VkDebugUtilsMessengerCreateInfoEXT * vkDebugUtilsMessengerCreateInfoGet(void);
void vkCreateDebugUtilsMessengerWrap(void);
void vkDestroyDebugUtilsMessengerWrap(void);
void vkLog(const VulkanTag tag, const char * const msg, ...);
