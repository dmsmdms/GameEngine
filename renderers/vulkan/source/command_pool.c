#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/physical_device.h>
#include <vulkan/api/command_pool.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <asserts.h>

VkCommandPool mainRenderCommandPool = VK_NULL_HANDLE;
VkCommandPool mainTransferCommandPool = VK_NULL_HANDLE;

void vkCreateCommandPoolWrap(const uint_fast8_t index, VkCommandPool * const commandPool) {
	const VkCommandPoolCreateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext				= NULL,
		.flags				= 0,
		.queueFamilyIndex	= mainRenderQueueIndex,
	};

	const VkResult result = vkCreateCommandPool(mainDevice, &info, allocationCallbacks, commandPool);
	RK_VULKAN_ASSERT(result, "Create command pool! (index=%u)", (unsigned)index);
}

void vkCreateCommandPoolsWrap(void) {
	vkCreateCommandPoolWrap(mainRenderQueueIndex, &mainRenderCommandPool);
	vkLog(VULKAN_TAG_INFO, "Main render command pool created!");

	if (mainDeviceInfo.memoryInfo.hasLocal) {
		vkCreateCommandPoolWrap(mainTransferQueueIndex, &mainTransferCommandPool);
		vkLog(VULKAN_TAG_INFO, "Main transfer command pool created!");
	}
}

void vkDestroyCommandPoolsWrap(void) {
	if (mainRenderCommandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(mainDevice, mainRenderCommandPool, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main render command pool destroyed!");
		mainRenderCommandPool = VK_NULL_HANDLE;
	}

	if (mainTransferCommandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(mainDevice, mainTransferCommandPool, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main transfer command pool destroyed!");
		mainTransferCommandPool = VK_NULL_HANDLE;
	}
}

void vkResetRenderCommandPoolWrap(void) {
	const VkResult result = vkResetCommandPool(mainDevice, mainRenderCommandPool, 0);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Main render command pool resetted!");
}
