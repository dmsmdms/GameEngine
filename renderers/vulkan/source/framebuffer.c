#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/framebuffer.h>
#include <vulkan/api/render_pass.h>
#include <vulkan/api/image_view.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/device.h>
#include <defines.h>
#include <options.h>
#include <asserts.h>

typedef enum {
	VK_IMAGE_VIEW_INDEX_MIN,
	VK_IMAGE_VIEW_INDEX_MAIN = VK_IMAGE_VIEW_INDEX_MIN,
	VK_IMAGE_VIEW_INDEX_MAX,
} VkImageViewIndex;

VkFramebuffer swapchainFramebuffers[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

void vkCreateFramebufferWrap(const VkImageView imageView, VkFramebuffer * const framebuffer) {
	const VkImageView attachments[] = {
		RK_VAL(VK_IMAGE_VIEW_INDEX_MAIN, imageView),
	};

	const VkFramebufferCreateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext				= NULL,
		.flags				= 0,
		.renderPass			= renderPass,
		.attachmentCount	= ARRAY_LEN(attachments),
		.pAttachments		= attachments,
		.width				= cmdOptions.width,
		.height				= cmdOptions.height,
		.layers				= CONFIG_VULKAN_SWAPCHAIN_LAYERS_COUNT,
	};

	const VkResult result = vkCreateFramebuffer(mainDevice, &info, allocationCallbacks, framebuffer);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkCreateSwapchainFramebuffersWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		vkCreateFramebufferWrap(swapchainImageViews[i], &swapchainFramebuffers[i]);
	}

	vkLog(VULKAN_TAG_INFO, "Swapchain framebuffers created!");
}

void vkDestroySwapchainFramebuffersWrap(void) {
	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		const VkFramebuffer framebuffer = swapchainFramebuffers[i];

		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(mainDevice, framebuffer, allocationCallbacks);
			swapchainFramebuffers[i] = VK_NULL_HANDLE;
		}
	}

	vkLog(VULKAN_TAG_INFO, "Swapchain framebuffers destroyed!");
}
