#include <vulkan/api/command_buffer.h>
#include <vulkan/api/semaphore.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/device.h>
#include <vulkan/api/fence.h>
#include <vulkan/api/queue.h>
#include <defines.h>
#include <asserts.h>

typedef enum {
	VK_SUBMIT_INDEX_MIN,
	VK_SUBMIT_INDEX_MAIN = VK_SUBMIT_INDEX_MIN,
	VK_SUBMIT_INDEX_MAX,
} VkSubmitIndex;

typedef enum {
	VK_FENCE_INDEX_MIN,
	VK_FENCE_INDEX_MAIN = VK_FENCE_INDEX_MIN,
	VK_FENCE_INDEX_MAX,
} VkFenceIndex;

static const VkPipelineStageFlags waitStages[] = {
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
};

void vkQueueSubmitWrap(void) {
	const VkFence fences[] = {
		RK_VAL(VK_FENCE_INDEX_MAIN, renderImageFences[swapchainImageIndex]),
	};

	const VkSemaphore waitSemaphores[] = {
		swapchainImageSemaphores[swapchainImageIndex],
	};

	const VkCommandBuffer commandBuffers[] = {
		renderCommandBuffers[swapchainImageIndex],
	};

	const VkSemaphore signalSemaphores[] = {
		renderImageSemaphores[swapchainImageIndex],
	};

	const VkSubmitInfo infos[] = {
		[VK_SUBMIT_INDEX_MAIN] = {
			.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext					= NULL,
			.waitSemaphoreCount		= ARRAY_LEN(waitSemaphores),
			.pWaitSemaphores		= waitSemaphores,
			.pWaitDstStageMask		= waitStages,
			.commandBufferCount		= ARRAY_LEN(commandBuffers),
			.pCommandBuffers		= commandBuffers,
			.signalSemaphoreCount	= ARRAY_LEN(signalSemaphores),
			.pSignalSemaphores		= signalSemaphores,
		}
	};

	VkResult result = vkWaitForFences(mainDevice, ARRAY_LEN(fences), fences, VK_TRUE, UINT64_MAX);
	RK_VULKAN_ASSERT(result, NULL);

	result = vkResetFences(mainDevice, ARRAY_LEN(fences), fences);
	RK_VULKAN_ASSERT(result, NULL);

	result = vkQueueSubmit(mainRenderQueue, ARRAY_LEN(infos), infos, fences[VK_FENCE_INDEX_MAIN]);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkQueuePresentWrap(void) {
	const VkSemaphore waitSemaphores[] = {
		renderImageSemaphores[swapchainImageIndex],
	};

	const VkSwapchainKHR swapchains[] = {
		swapchain,
	};

	const uint32_t imageIndices[ARRAY_LEN(swapchains)] = {
		swapchainImageIndex,
	};

	VkResult results[ARRAY_LEN(swapchains)] = {
		VK_SUCCESS,
	};

	const VkPresentInfoKHR info = {
		.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext				= NULL,
		.waitSemaphoreCount	= ARRAY_LEN(waitSemaphores),
		.pWaitSemaphores	= waitSemaphores,
		.swapchainCount		= ARRAY_LEN(swapchains),
		.pSwapchains		= swapchains,
		.pImageIndices		= imageIndices,
		.pResults			= results,
	};

	const VkResult result = vkQueuePresentKHR(presentQueue, &info);
	RK_VULKAN_ASSERT(result, NULL);
}
