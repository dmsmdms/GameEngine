#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/device.h>
#include <vulkan/api/fence.h>
#include <asserts.h>

static const VkFenceCreateInfo info = {
	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	.pNext = NULL,
	.flags = VK_FENCE_CREATE_SIGNALED_BIT,
};

VkFence renderImageFences[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

void vkCreateFenceWrap(const VkDevice device, VkFence * const fence) {
	const VkResult result = vkCreateFence(device, &info, allocationCallbacks, fence);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkDestroyFenceWrap(const VkDevice device, VkFence * const restrict fence) {
	if (*fence != VK_NULL_HANDLE) {
		vkDestroyFence(device, *fence, allocationCallbacks);
		*fence = VK_NULL_HANDLE;
	}
}


void vkCreateFencesWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkCreateFenceWrap(mainDevice, &renderImageFences[i]);
	}

	vkLog(VULKAN_TAG_INFO, "All fences created!");
}

void vkDestroyFencesWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkDestroyFenceWrap(mainDevice, &renderImageFences[i]);
	}

	vkLog(VULKAN_TAG_INFO, "All fences destroyed!");
}
