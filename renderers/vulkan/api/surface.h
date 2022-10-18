#pragma once

#include <vulkan/vulkan.h>

typedef struct {
	uint32_t imageCount;
	VkExtent2D extent;
	VkSurfaceTransformFlagBitsKHR currentTransform;
} RkSurfaceCapabilities;

extern VkSurfaceKHR surface;
extern VkSurfaceFormatKHR surfaceFormat;
extern VkPresentModeKHR presentMode;

void vkDestroySurfaceWrap(void);
void vkGetPhysicalDeviceSurfaceFormatsWrap(void);
void vkGetPhysicalDeviceSurfacePresentModesWrap(void);
void vkGetPhysicalDeviceSurfaceCapabilitiesWrap(RkSurfaceCapabilities * const restrict info);
