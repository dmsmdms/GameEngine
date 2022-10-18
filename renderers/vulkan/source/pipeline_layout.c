#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/pipeline_layout.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <asserts.h>

static const VkPipelineLayoutCreateInfo info = {
	.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	.pNext					= NULL,
	.flags					= 0,
	.setLayoutCount			= 0,
	.pSetLayouts			= NULL,
	.pushConstantRangeCount	= 0,
	.pPushConstantRanges	= NULL,
};

VkPipelineLayout mainPipelineLayout = VK_NULL_HANDLE;

void vkCreatePipelineLayoutWrap(void) {
	const VkResult result = vkCreatePipelineLayout(mainDevice, &info, allocationCallbacks, &mainPipelineLayout);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Main pipeline layout created!");
}

void vkDestroyPipelineLayoutWrap(void) {
	if (mainPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(mainDevice, mainPipelineLayout, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main pipeline layout destroyed!");
		mainPipelineLayout = VK_NULL_HANDLE;
	}
}
