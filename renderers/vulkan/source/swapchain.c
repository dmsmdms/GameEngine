#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/image_view.h>
#include <vulkan/api/semaphore.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/helper.h>
#include <vulkan/api/device.h>
#include <vulkan/api/fence.h>
#include <autoconf.h>
#include <asserts.h>

VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImage swapchainImages[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };
uint_fast8_t swapchainImagesCount = 0;
uint_fast8_t swapchainImageIndex = 0;

void vkCreateSwapchainWrap(void) {
	RkSurfaceCapabilities surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesWrap(&surfaceCapabilities);

	const VkSwapchainCreateInfoKHR info = {
		.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext					= NULL,
		.flags					= 0,
		.surface				= surface,
		.minImageCount			= surfaceCapabilities.imageCount,
		.imageFormat			= surfaceFormat.format,
		.imageColorSpace		= surfaceFormat.colorSpace,
		.imageExtent			= surfaceCapabilities.extent,
		.imageArrayLayers		= CONFIG_VULKAN_SWAPCHAIN_LAYERS_COUNT,
		.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount	= 0,
		.pQueueFamilyIndices	= NULL,
		.preTransform			= surfaceCapabilities.currentTransform,
		.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode			= presentMode,
		.clipped				= VK_TRUE,
		.oldSwapchain			= swapchain,
	};

	VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
	const VkResult result = vkCreateSwapchainKHR(mainDevice, &info, allocationCallbacks, &newSwapchain);
	RK_VULKAN_ASSERT(result, NULL);

	// Destroy old swapchain if exists //
	vkDestroySwapchainWrap();

	surfaceFormat.format = info.imageFormat;
	swapchain = newSwapchain;

	vkLog(VULKAN_TAG_INFO, "New swapchain created!");
}

void vkDestroySwapchainWrap(void) {
	if (swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(mainDevice, swapchain, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Old swapchain destroyed!");
		swapchain = VK_NULL_HANDLE;
	}
}

void vkGetSwapchainImagesWrap(void) {
	uint32_t imagesCount;
	VkResult result = vkGetSwapchainImagesKHR(mainDevice, swapchain, &imagesCount, NULL);
	RK_VULKAN_ASSERT(result, NULL);

	RK_ASSERT(imagesCount > CONFIG_RENDERER_MAX_IMAGE_COUNT,
			  "Unexpexted swapchainImagesCount=%u, maxSwapchainImagesCount=%u",
			  (unsigned)imagesCount, (unsigned)CONFIG_RENDERER_MAX_IMAGE_COUNT);
	swapchainImagesCount = imagesCount;

	result = vkGetSwapchainImagesKHR(mainDevice, swapchain, &imagesCount, swapchainImages);
	RK_VULKAN_ASSERT(result, NULL);

	vkLog(VULKAN_TAG_INFO, "Get swapchain images! (swapchainImagesCount=%u)", (unsigned)imagesCount);
}

void vkAcquireNextImageWrap(void) {
	swapchainImageIndex = (swapchainImageIndex + 1) % swapchainImagesCount;
	const VkSemaphore semaphore = swapchainImageSemaphores[swapchainImageIndex];
	uint32_t index;

	const VkResult result = vkAcquireNextImageKHR(mainDevice, swapchain, UINT64_MAX,
												  semaphore, VK_NULL_HANDLE, &index);
	RK_VULKAN_ASSERT(result, NULL);

	if (swapchainImageIndex != index) {
		vkSwapHandlers((uint64_t *)swapchainImageSemaphores, swapchainImageIndex, index);
		vkSwapHandlers((uint64_t *)renderImageSemaphores, swapchainImageIndex, index);
		vkSwapHandlers((uint64_t *)renderImageFences, swapchainImageIndex, index);
		swapchainImageIndex = index;
	}
}
