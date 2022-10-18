#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/physical_device.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <vulkan/api/helper.h>
#include <autoconf.h>
#include <asserts.h>
#include <defines.h>
#include <options.h>

typedef enum {
	VK_QUEUE_INDEX_MIN,
	VK_QUEUE_INDEX_MAIN = VK_QUEUE_INDEX_MIN,
	VK_QUEUE_INDEX_MAX,
} VkQueueIndex;

static const float queuePriorities[] = {
	RK_VAL(VK_QUEUE_INDEX_MAIN, 1.0),
};

static const VkDeviceQueueCreateInfo queueInfo = {
	.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
	.pNext				= NULL,
	.flags				= 0,
	.queueFamilyIndex	= 0,
	.queueCount			= ARRAY_LEN(queuePriorities),
	.pQueuePriorities	= queuePriorities,
};

uint_fast8_t mainRenderQueueIndex = 0;
uint_fast8_t mainTransferQueueIndex = 0;
VkDevice mainDevice = VK_NULL_HANDLE;
VkQueue mainRenderQueue = VK_NULL_HANDLE;
VkQueue mainTransferQueue = VK_NULL_HANDLE;
VkQueue presentQueue = VK_NULL_HANDLE;

void vkCreateDeviceWrap(void) {
	const char * extensions[CONFIG_VULKAN_DEVICE_EXTENSIONS_MAX];
	uint_fast8_t extensionsCount = 0;

	extensions[extensionsCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	if (cmdOptions.vkDebugMarker != VK_FALSE) {
		extensions[extensionsCount++] = VK_EXT_DEBUG_MARKER_EXTENSION_NAME;
	}

	VkDeviceQueueCreateInfo queuesInfo[CONFIG_VULKAN_DEVICE_QUEUE_FAMILIES_MAX];
	uint_fast8_t queuesInfoCount = 0;

	VkDeviceQueueCreateInfo * const restrict renderPresentQueue = &queuesInfo[queuesInfoCount++];
	const VkQueueFamilyInfo * const restrict queueFamilyInfo = &mainDeviceInfo.queueFamilyInfo;
	*renderPresentQueue = queueInfo;

	const uint_fast8_t renderPresentPriorities[] = {
		[0] = queueFamilyInfo->renderPresentId,
		[1] = queueFamilyInfo->renderPresentTransferId,
		[2] = queueFamilyInfo->renderComputePresentId,
		[4] = queueFamilyInfo->renderComputePresentTransferId,
	};

	const uint_fast8_t renderPresentId = vkSelectIdByPriority(
		renderPresentPriorities, ARRAY_LEN(renderPresentPriorities)
	);
	RK_ASSERT(renderPresentId == UINT8_MAX, "RenderPresent queue family is absent!");
	renderPresentQueue->queueFamilyIndex = renderPresentId;
	mainRenderQueueIndex = renderPresentId;

	vkLog(VULKAN_TAG_DEBUG, "Selected renderPresent queue! (id=%u)", renderPresentId);

	uint_fast8_t transferId = UINT8_MAX;
	if (mainDeviceInfo.memoryInfo.hasLocal) {
		VkDeviceQueueCreateInfo * const restrict transferQueue = &queuesInfo[queuesInfoCount++];
		*transferQueue = queueInfo;

		const uint_fast8_t transferPriorities[] = {
			[0] = queueFamilyInfo->transferId,
		};

		transferId = vkSelectIdByPriority(transferPriorities, ARRAY_LEN(transferPriorities));
		RK_ASSERT(transferId == UINT8_MAX, "Transfer queue family is absent!");
		transferQueue->queueFamilyIndex = transferId;
		mainTransferQueueIndex = transferId;

		vkLog(VULKAN_TAG_DEBUG, "Selected transfer queue! (id=%u)", transferId);
	}

	const VkDeviceCreateInfo info = {
		.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext						= NULL,
		.flags						= 0,
		.queueCreateInfoCount		= queuesInfoCount,
		.pQueueCreateInfos			= queuesInfo,
		.enabledLayerCount			= 0,
		.ppEnabledLayerNames		= NULL,
		.enabledExtensionCount		= extensionsCount,
		.ppEnabledExtensionNames	= extensions,
		.pEnabledFeatures			= NULL,
	};

	const VkResult result = vkCreateDevice(
		mainDeviceInfo.physicalDevice, &info, allocationCallbacks, &mainDevice
	);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Main device created!");

	vkGetDeviceQueue(mainDevice, renderPresentId, VK_QUEUE_INDEX_MAIN, &mainRenderQueue);
	presentQueue = mainRenderQueue;

	if (mainDeviceInfo.memoryInfo.hasLocal) {
		vkGetDeviceQueue(mainDevice, transferId, VK_QUEUE_INDEX_MAIN, &mainTransferQueue);
	}
}

void vkDestroyDeviceWrap(void) {
	if (mainDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(mainDevice, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main device destroyed!");
		mainDevice = VK_NULL_HANDLE;
	}
}

void vkDeviceWaitIdleWrap(void) {
	const VkResult result = vkDeviceWaitIdle(mainDevice);
	RK_VULKAN_ASSERT(result, NULL);
}
