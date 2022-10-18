#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/physical_device.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/helper.h>
#include <simple-math.h>
#include <asserts.h>
#include <options.h>
#include <defines.h>
#include <logger.h>
#include <string.h>

#define VK_FORMAT_INFO(format, colorSpace) \
	{ format, colorSpace }

static const VkSurfaceFormatKHR formatPriorities[] = {
	[0] = VK_FORMAT_INFO(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
};

static const VkPresentModeKHR presentModePriorities[] = {
	[0] = VK_PRESENT_MODE_FIFO_KHR,
};

VkSurfaceKHR surface = VK_NULL_HANDLE;
VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
VkSurfaceFormatKHR surfaceFormat = {
	.format = VK_FORMAT_MAX_ENUM,
	.colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR,
};

void vkDestroySurfaceWrap(void) {
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Surface destroed!");
		surface = VK_NULL_HANDLE;
	}
}

void vkGetPhysicalDeviceSurfaceCapabilitiesWrap(RkSurfaceCapabilities * const restrict info) {
	VkSurfaceCapabilitiesKHR capabilities;
	const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		mainDeviceInfo.physicalDevice, surface, &capabilities);
	RK_VULKAN_ASSERT(result, NULL);

	if (capabilities.maxImageCount == 0) {
		capabilities.maxImageCount = cmdOptions.imageCount;
	}

	info->imageCount = max(
		capabilities.minImageCount, min(cmdOptions.imageCount, capabilities.maxImageCount));
	info->currentTransform = capabilities.currentTransform;

	if (info->imageCount != cmdOptions.imageCount) {
		rkLog(RK_LOG_LEVEL_WARNING, "Unsupported imageCount=%u, used imageCount=%u",
			  (unsigned)cmdOptions.imageCount, (unsigned)info->imageCount);
		cmdOptions.imageCount = info->imageCount;
	}

	if (capabilities.currentExtent.width != UINT32_MAX) {
		cmdOptions.width = capabilities.currentExtent.width;
		cmdOptions.height = capabilities.currentExtent.height;
		info->extent = capabilities.currentExtent;
	} else {
		cmdOptions.width = max(capabilities.minImageExtent.width, min(cmdOptions.width, capabilities.maxImageExtent.width));
		cmdOptions.height = max(capabilities.minImageExtent.height, min(cmdOptions.height, capabilities.maxImageExtent.height));
		info->extent.width = cmdOptions.width;
		info->extent.height = cmdOptions.height;
	}

	vkLog(VULKAN_TAG_DEBUG, "Used extend %ux%u", (unsigned)info->extent.width,
		  (unsigned)info->extent.height);
}

void vkGetPhysicalDeviceSurfaceFormatsWrap(void) {
	uint32_t formatsCount;
	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		mainDeviceInfo.physicalDevice, surface, &formatsCount, NULL
	);
	RK_VULKAN_ASSERT(result, NULL);

	VkSurfaceFormatKHR formats[formatsCount];
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		mainDeviceInfo.physicalDevice, surface, &formatsCount, formats
	);
	RK_VULKAN_ASSERT(result, NULL);

	vkSelectValueByPriority(
		formats, formatsCount, formatPriorities, ARRAY_LEN(formatPriorities),
		&surfaceFormat, sizeof(surfaceFormat)
	);
	RK_ASSERT(surfaceFormat.format == VK_FORMAT_MAX_ENUM, "Vulkan no supported format found!");
}

void vkGetPhysicalDeviceSurfacePresentModesWrap(void) {
	uint32_t presentModesCount;
	VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
		mainDeviceInfo.physicalDevice, surface, &presentModesCount, NULL
	);
	RK_VULKAN_ASSERT(result, NULL);

	VkPresentModeKHR presentModes[presentModesCount];
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(
		mainDeviceInfo.physicalDevice, surface, &presentModesCount, presentModes
	);
	RK_VULKAN_ASSERT(result, NULL);

	vkSelectValueByPriority(
		presentModes, presentModesCount, presentModePriorities, ARRAY_LEN(presentModePriorities),
		&presentMode, sizeof(presentMode)
	);
	RK_ASSERT(presentMode == VK_PRESENT_MODE_MAX_ENUM_KHR, "Vulkan no supported present format!");
}
