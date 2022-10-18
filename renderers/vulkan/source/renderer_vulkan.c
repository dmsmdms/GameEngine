#include <vulkan/api/physical_device.h>
#include <vulkan/api/pipeline_layout.h>
#include <vulkan/api/pipeline_cache.h>
#include <vulkan/api/command_buffer.h>
#include <vulkan/api/command_pool.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/render_pass.h>
#include <vulkan/api/framebuffer.h>
#include <vulkan/api/image_view.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/semaphore.h>
#include <vulkan/api/pipeline.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/device.h>
#include <vulkan/api/shader.h>
#include <vulkan/api/buffer.h>
#include <vulkan/api/queue.h>
#include <vulkan/api/fence.h>
#include <window.h>

void rkVulkanRendererCreate(void) {
	vkCreateInstanceWrap();
	vkCreateDebugUtilsMessengerWrap();
	vkCreateSurfaceWrap();
	vkEnumeratePhysicalDevicesWrap();
	vkGetPhysicalDeviceSurfaceFormatsWrap();
	vkGetPhysicalDeviceSurfacePresentModesWrap();
	vkCreateDeviceWrap();
	vkCreateVertexBufferGroupWrap();
	vkCreateShaderModulesWrap();
	vkCreateRenderPassWrap();
	vkCreatePipelineLayoutWrap();
	vkCreatePipelineCacheWrap();
	vkCreateGraphicsPipelineWrap();
	vkCreateCommandPoolsWrap();
	vkCreateSwapchainWrap();
	vkGetSwapchainImagesWrap();
	vkCreateSwapchainImageViewsWrap();
	vkCreateSwapchainFramebuffersWrap();
	vkCreateRenderCommandBuffersWrap();
	vkCreateSemaphoresWrap();
	vkCreateFencesWrap();
}

void rkVulkanRendererDestroy(void) {
	vkDeviceWaitIdleWrap();
	vkDestroyFencesWrap();
	vkDestroySemaphoresWrap();
	vkResetRenderCommandPoolWrap();
	vkDestroySwapchainFramebuffersWrap();
	vkDestroySwapchainImageViewsWrap();
	vkDestroySwapchainWrap();
	vkDestroyCommandPoolsWrap();
	vkDestroyGraphicsPipelineWrap();
	vkDestroyPipelineCacheWrap();
	vkDestroyPipelineLayoutWrap();
	vkDestroyRenderPassWrap();
	vkDestroyShaderModulesWrap();
	vkDestroyVertexBufferGroupWrap();
	vkDestroyDeviceWrap();
	vkDestroySurfaceWrap();
	vkDestroyDebugUtilsMessengerWrap();
	vkDestroyInstanceWrap();
}

void rkVulkanRendererDraw(void) {
	vkAcquireNextImageWrap();
	vkQueueSubmitWrap();
	vkQueuePresentWrap();
}
