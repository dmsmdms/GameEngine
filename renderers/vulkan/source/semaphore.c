#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/semaphore.h>
#include <vulkan/api/device.h>
#include <asserts.h>

static const VkSemaphoreCreateInfo info = {
	.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	.pNext = NULL,
	.flags = 0,
};

VkSemaphore swapchainImageSemaphores[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };
VkSemaphore renderImageSemaphores[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

void vkCreateSemaphoreWrap(const VkDevice device, VkSemaphore * const pSemaphore) {
	const VkResult result = vkCreateSemaphore(device, &info, allocationCallbacks, pSemaphore);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkDestroySemaphoreWrap(const VkDevice device, VkSemaphore * const restrict pSemaphore) {
	if (*pSemaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(device, *pSemaphore, allocationCallbacks);
		*pSemaphore = VK_NULL_HANDLE;
	}
}

void vkCreateSemaphoresWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkCreateSemaphoreWrap(mainDevice, &swapchainImageSemaphores[i]);
		vkCreateSemaphoreWrap(mainDevice, &renderImageSemaphores[i]);
	}

	vkLog(VULKAN_TAG_INFO, "All semaphores created!");
}

void vkDestroySemaphoresWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkDestroySemaphoreWrap(mainDevice, &swapchainImageSemaphores[i]);
		vkDestroySemaphoreWrap(mainDevice, &renderImageSemaphores[i]);
	}

	vkLog(VULKAN_TAG_INFO, "All semaphores destroyed!");
}
