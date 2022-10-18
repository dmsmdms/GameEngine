#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/render_pass.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/device.h>
#include <defines.h>
#include <asserts.h>

typedef enum {
	VK_ATTACHMENT_DESCRIPTION_MIN,
	VK_ATTACHMENT_DESCRIPTION_MAIN = VK_ATTACHMENT_DESCRIPTION_MIN,
	VK_ATTACHMENT_DESCRIPTION_MAX,
} VkAttachmentDescriptionIndex;

typedef enum {
	VK_ATTACHMENT_REFERENCE_MIN,
	VK_ATTACHMENT_REFERENCE_MAIN = VK_ATTACHMENT_REFERENCE_MIN,
	VK_ATTACHMENT_REFERENCE_MAX,
} VkAttachmentReferenceIndex;

static const VkAttachmentReference colorAttachmentReferences[] = {
	[VK_ATTACHMENT_REFERENCE_MAIN] = {
		.attachment	= VK_ATTACHMENT_DESCRIPTION_MAIN,
		.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	},
};

static const VkSubpassDescription subpassDescriptions[] = {
	[VK_SUBPASS_INDEX_MAIN] = {
		.flags						= 0,
		.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount		= 0,
		.pInputAttachments			= NULL,
		.colorAttachmentCount		= ARRAY_LEN(colorAttachmentReferences),
		.pColorAttachments			= colorAttachmentReferences,
		.pResolveAttachments		= NULL,
		.pDepthStencilAttachment	= NULL,
		.preserveAttachmentCount	= 0,
		.pPreserveAttachments		= NULL,
	},
};

VkRenderPass renderPass = VK_NULL_HANDLE;

void vkCreateRenderPassWrap(void) {
	const VkAttachmentDescription attachmentDescriptions[] = {
		[VK_ATTACHMENT_DESCRIPTION_MAIN] = {
			.flags			= 0,
			.format			= surfaceFormat.format,
			.samples		= VK_SAMPLE_COUNT_1_BIT,
			.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		},
	};

	const VkRenderPassCreateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext				= NULL,
		.flags				= 0,
		.attachmentCount	= ARRAY_LEN(attachmentDescriptions),
		.pAttachments		= attachmentDescriptions,
		.subpassCount		= ARRAY_LEN(subpassDescriptions),
		.pSubpasses			= subpassDescriptions,
		.dependencyCount	= 0,
		.pDependencies		= NULL,
	};

	const VkResult result = vkCreateRenderPass(mainDevice, &info, allocationCallbacks, &renderPass);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Render pass created!");
}

void vkDestroyRenderPassWrap(void) {
	if (renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(mainDevice, renderPass, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Render pass destroyed!");
		renderPass = VK_NULL_HANDLE;
	}
}
