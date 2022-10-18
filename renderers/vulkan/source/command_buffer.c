#include <vulkan/api/command_buffer.h>
#include <vulkan/api/command_pool.h>
#include <vulkan/api/render_pass.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/framebuffer.h>
#include <vulkan/api/swapchain.h>
#include <vulkan/api/pipeline.h>
#include <vulkan/api/device.h>
#include <vulkan/api/buffer.h>
#include <global/api/vertex.h>
#include <defines.h>
#include <asserts.h>
#include <options.h>

typedef enum {
	VK_SCISSOR_INDEX_MIN,
	VK_SCISSOR_INDEX_MAIN = VK_SCISSOR_INDEX_MIN,
	VK_SCISSOR_INDEX_MAX,
} VkAreaIndex;

typedef enum {
	VK_CLEAR_VALUE_INDEX_MIN,
	VK_CLEAR_VALUE_INDEX_MAIN = VK_CLEAR_VALUE_INDEX_MIN,
	VK_CLEAR_VALUE_INDEX_MAX,
} VkClearValueIndex;

typedef enum {
	VK_VIEWPORT_INDEX_MIN,
	VK_VIEWPORT_INDEX_MAIN = VK_VIEWPORT_INDEX_MIN,
	VK_VIEWPORT_INDEX_MAX,
} VkViewportIndex;

typedef enum {
	VK_BUFFER_INDEX_MIN,
	VK_BUFFER_INDEX_MAIN = VK_BUFFER_INDEX_MIN,
	VK_BUFFER_INDEX_MAX,
} VkBufferIndex;

static const VkClearValue clearValue[] = {
	[VK_CLEAR_VALUE_INDEX_MAIN] = {
		.color.float32 = { 0.0f, 0.0f, 0.0f, 1.0f },
	},
};

static const VkDeviceSize vertexOffsets[] = {
	RK_VAL(VK_BUFFER_INDEX_MAIN, 0),
};

VkCommandBuffer renderCommandBuffers[CONFIG_RENDERER_MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

void vkAllocateCommandBuffersWrap(const VkDevice device, const VkCommandPool pool, const uint_fast8_t count,
								  VkCommandBuffer * const buffers)
{
	const VkCommandBufferAllocateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext				= NULL,
		.commandPool		= pool,
		.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount	= count,
	};

	const VkResult result = vkAllocateCommandBuffers(device, &info, buffers);
	RK_VULKAN_ASSERT(result, NULL);
}

void vkCreateRenderCommandBuffersWrap(void) {
	vkAllocateCommandBuffersWrap(mainDevice, mainRenderCommandPool, swapchainImagesCount, renderCommandBuffers);

	for (uint_fast8_t i = 0; i < swapchainImagesCount; i++) {
		VkCommandBufferUsageFlags commandBufferFlags = 0;

#ifdef CONFIG_WSI_WAYLAND // FIXME: seams that wayland surface was created incorrectly
		if (cmdOptions.wsiType == RK_WSI_TYPE_WAYLAND) {
			commandBufferFlags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		}
#endif

		const VkCommandBufferBeginInfo commandBufferInfo = {
			.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext				= NULL,
			.flags				= commandBufferFlags,
			.pInheritanceInfo	= NULL,
		};

		const VkRect2D renderArea = {
			.offset.x		= 0,
			.offset.y		= 0,
			.extent.width	= cmdOptions.width,
			.extent.height	= cmdOptions.height,
		};

		const VkRenderPassBeginInfo renderPassInfo = {
			.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext				= NULL,
			.renderPass			= renderPass,
			.framebuffer		= swapchainFramebuffers[i],
			.renderArea			= renderArea,
			.clearValueCount	= ARRAY_LEN(clearValue),
			.pClearValues		= clearValue,
		};

		const VkViewport viewports[] = {
			[VK_VIEWPORT_INDEX_MAIN] = {
				.x			= 0.0f,
				.y			= 0.0f,
				.width		= cmdOptions.width,
				.height		= cmdOptions.height,
				.minDepth	= 0.0f,
				.maxDepth	= 1.0f,
			},
		};

		const VkRect2D scissors[] = {
			RK_VAL(VK_SCISSOR_INDEX_MAIN, renderArea),
		};

		const VkBuffer vertexBuffers[] = {
			RK_VAL(VK_BUFFER_INDEX_MAIN, vertexBuffer),
		};

		const uint32_t vertexOffset = 0;
		const VkDeviceSize indexOffset = 0;
		const VkCommandBuffer commandBuffer = renderCommandBuffers[i];
		VkResult result = vkBeginCommandBuffer(commandBuffer, &commandBufferInfo);
		RK_VULKAN_ASSERT(result, "Begin command buffer! (id=%u)", (unsigned)i);

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline);
		vkCmdSetViewport(commandBuffer, VK_VIEWPORT_INDEX_MIN, ARRAY_LEN(viewports), viewports);
		vkCmdSetScissor(commandBuffer, VK_SCISSOR_INDEX_MIN, ARRAY_LEN(scissors), scissors);
		vkCmdBindVertexBuffers(commandBuffer, VK_BUFFER_INDEX_MIN, ARRAY_LEN(vertexBuffers),
							   vertexBuffers, vertexOffsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, indexOffset, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		vkCmdEndRenderPass(commandBuffer);

		result = vkEndCommandBuffer(commandBuffer);
		RK_VULKAN_ASSERT(result, "End command buffer! (id=%u)", (unsigned)i);
	}
}
