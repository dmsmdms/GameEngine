#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/image_view.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/device.h>
#include <asserts.h>

static const VkComponentMapping simpleComponents = {
	.r = VK_COMPONENT_SWIZZLE_IDENTITY,
	.g = VK_COMPONENT_SWIZZLE_IDENTITY,
	.b = VK_COMPONENT_SWIZZLE_IDENTITY,
	.a = VK_COMPONENT_SWIZZLE_IDENTITY,
};

VkImageView swapchainImageViews[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

void vkCreateImageViewSimpleWrap(const VkDevice device, const VkImage image, const VkFormat format,
								 const VkImageAspectFlags aspectFlags, VkImageView * const restrict imageView)
{
	const VkImageSubresourceRange subresourceRange = {
		.aspectMask		= aspectFlags,
		.baseMipLevel	= 0,
		.levelCount		= 1,
		.baseArrayLayer	= 0,
		.layerCount		= 1,
	};

	const VkImageViewCreateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext				= NULL,
		.flags				= 0,
		.image				= image,
		.viewType			= VK_IMAGE_VIEW_TYPE_2D,
		.format				= format,
		.components			= simpleComponents,
		.subresourceRange	= subresourceRange,
	};

	const VkResult result = vkCreateImageView(device, &info, allocationCallbacks, imageView);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkCreateSwapchainImageViewsWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkCreateImageViewSimpleWrap(mainDevice, swapchainImages[i], surfaceFormat.format,
									VK_IMAGE_ASPECT_COLOR_BIT, &swapchainImageViews[i]);
	}

	vkLog(VULKAN_TAG_INFO, "Swapchain image viewes created!");
}

void vkDestroySwapchainImageViewsWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		const VkImageView imageView = swapchainImageViews[i];

		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(mainDevice, imageView, allocationCallbacks);
			swapchainImageViews[i] = VK_NULL_HANDLE;
		}
	}

	vkLog(VULKAN_TAG_INFO, "Swapchain image viewes destroyed!");
}
