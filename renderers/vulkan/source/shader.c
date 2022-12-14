#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <vulkan/api/shader.h>
#include <asserts.h>
#include <defines.h>

// Autogenerated shaders from .glsl files //
#include <main.vert.spv.h>
#include <main.frag.spv.h>

#define VK_SHADER_INFO(index, shader, source) \
	[index] = { shader, source, #index, sizeof(source) }

typedef enum {
	VK_SHADER_INDEX_MIN,
	VK_SHADER_INDEX_VERTEX = VK_SHADER_INDEX_MIN,
	VK_SHADER_INDEX_FRAGMENT,
	VK_SHADER_INDEX_MAX,
} VkShaderIndex;

typedef struct {
	VkShaderModule * shader;
	const uint32_t * source;
	const char * debugName;
	uint_fast32_t size;
} VkShaderInfo;

static const VkShaderInfo shaderInfo[] = {
	VK_SHADER_INFO(VK_SHADER_INDEX_VERTEX, &vertexShader, main_vert_spv_h),
	VK_SHADER_INFO(VK_SHADER_INDEX_FRAGMENT, &fragmentShader, main_frag_spv_h),
};

VkShaderModule vertexShader = VK_NULL_HANDLE;
VkShaderModule fragmentShader = VK_NULL_HANDLE;

static void vkCreateShaderModuleWrap(const VkShaderInfo * const restrict info) {
	const VkShaderModuleCreateInfo createInfo = {
		.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext		= NULL,
		.flags		= 0,
		.codeSize	= info->size,
		.pCode		= info->source,
	};

	const VkResult result = vkCreateShaderModule(mainDevice, &createInfo, allocationCallbacks, info->shader);
	RK_VULKAN_ASSERT(result, "Error while loading shader! [%s]", info->debugName);
	vkLog(VULKAN_TAG_INFO, "Create shader module! [%s]", info->debugName);
}

static void vkDestroyShaderModuleWrap(const VkShaderInfo * const restrict info) {
	const VkShaderModule shader = *info->shader;

	if (shader != VK_NULL_HANDLE) {
		vkDestroyShaderModule(mainDevice, shader, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Destroy shader module! [%s]", info->debugName);
		*info->shader = VK_NULL_HANDLE;
	} else {
		vkLog(VULKAN_TAG_DEBUG, "Shader [%s] already destroyed!", info->debugName);
	}
}

void vkCreateShaderModulesWrap(void) {
	for (uint_fast8_t i = VK_SHADER_INDEX_MIN; i < VK_SHADER_INDEX_MAX; i++) {
		vkCreateShaderModuleWrap(&shaderInfo[i]);
	}
}

void vkDestroyShaderModulesWrap(void) {
	for (uint_fast8_t i = VK_SHADER_INDEX_MIN; i < VK_SHADER_INDEX_MAX; i++) {
		vkDestroyShaderModuleWrap(&shaderInfo[i]);
	}
}
