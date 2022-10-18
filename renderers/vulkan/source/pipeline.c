#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/pipeline_layout.h>
#include <vulkan/api/pipeline_cache.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/render_pass.h>
#include <vulkan/api/pipeline.h>
#include <vulkan/api/shader.h>
#include <vulkan/api/device.h>
#include <global/api/vertex.h>
#include <defines.h>
#include <asserts.h>

#define VK_SHADER(type, shader)										\
{																	\
	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,	\
	.pNext = NULL,													\
	.flags = 0,														\
	.stage = type,													\
	.module = shader,												\
	.pName = "main",												\
	.pSpecializationInfo = NULL,									\
}

#define VK_VERTEX_BINDING(index, size, rate)	\
{												\
	.binding = index,							\
	.stride = size,								\
	.inputRate = rate,							\
}

#define VK_VERTEX_ATTRIBUTE(index, bindIndex, attrFormat, attrOffset)	\
{																		\
	.location = index,													\
	.binding = bindIndex,												\
	.format = attrFormat,												\
	.offset = attrOffset,												\
}

typedef enum {
	VK_GRAPHICS_PIPELINE_INDEX_MIN,
	VK_GRAPHICS_PIPELINE_INDEX_MAIN = VK_GRAPHICS_PIPELINE_INDEX_MIN,
	VK_GRAPHICS_PIPELINE_INDEX_MAX,
} VkGraphicsPipelineIndex;

typedef enum {
	VK_VERTEX_BINDING_INDEX_MIN,
	VK_VERTEX_BINDING_INDEX_MAIN = VK_VERTEX_BINDING_INDEX_MIN,
	VK_VERTEX_BINDING_INDEX_MAX,
} VkVertexBindingIndex;

typedef enum {
	VK_ATTRIBUTE_INDEX_MIN,
	VK_ATTRIBUTE_INDEX_POSITION = VK_ATTRIBUTE_INDEX_MIN,
	VK_ATTRIBUTE_INDEX_MAX,
} VkAttributeIndex;

typedef enum {
	VK_COLOR_BLEND_ATTACHTMENT_INDEX_MIN,
	VK_COLOR_BLEND_ATTACHTMENT_INDEX_MAIN = VK_COLOR_BLEND_ATTACHTMENT_INDEX_MIN,
	VK_COLOR_BLEND_ATTACHTMENT_INDEX_MAX,
} VkColorBlendAttachmentIndex;

static const VkVertexInputBindingDescription inputBindingDescriptions[] = {
	VK_VERTEX_BINDING(VK_VERTEX_BINDING_INDEX_MAIN, sizeof(vertext_t), VK_VERTEX_INPUT_RATE_VERTEX),
};

static const VkVertexInputAttributeDescription inputAttributeDescription[] = {
	VK_VERTEX_ATTRIBUTE(VK_ATTRIBUTE_INDEX_POSITION, VK_VERTEX_BINDING_INDEX_MAIN,
		VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertext_t, inPosition)),
};

static const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
	.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	.pNext								= NULL,
	.flags								= 0,
	.vertexBindingDescriptionCount		= ARRAY_LEN(inputBindingDescriptions),
	.pVertexBindingDescriptions			= inputBindingDescriptions,
	.vertexAttributeDescriptionCount	= ARRAY_LEN(inputAttributeDescription),
	.pVertexAttributeDescriptions		= inputAttributeDescription,
};

static const VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
	.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	.pNext					= NULL,
	.flags					= 0,
	.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	.primitiveRestartEnable	= VK_FALSE,
};

static const VkPipelineViewportStateCreateInfo viewportInfo = {
	.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	.pNext			= NULL,
	.flags			= 0,
	.viewportCount	= 0,
	.pViewports		= NULL,
	.scissorCount	= 0,
	.pScissors		= NULL,
};

static const VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
	.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	.pNext						= NULL,
	.flags						= 0,
	.depthClampEnable			= VK_FALSE,
	.rasterizerDiscardEnable	= VK_FALSE,
	.polygonMode				= VK_POLYGON_MODE_FILL,
	.cullMode					= VK_CULL_MODE_BACK_BIT,
	.frontFace					= VK_FRONT_FACE_CLOCKWISE,
	.depthBiasEnable			= VK_FALSE,
	.depthBiasConstantFactor	= 0.0f,
	.depthBiasClamp				= 0.0f,
	.depthBiasSlopeFactor		= 0.0f,
	.lineWidth					= 1.0f,
};

static const VkPipelineMultisampleStateCreateInfo multisampleInfo = {
	.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	.pNext					= NULL,
	.flags					= 0,
	.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT,
	.sampleShadingEnable	= VK_FALSE,
	.minSampleShading		= 1.0f,
	.pSampleMask			= NULL,
	.alphaToCoverageEnable	= VK_FALSE,
	.alphaToOneEnable		= VK_FALSE,
};

static const VkPipelineColorBlendAttachmentState colorBlendAttachments[] = {
	[VK_COLOR_BLEND_ATTACHTMENT_INDEX_MAIN] = {
		.blendEnable			= VK_FALSE,
		.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO,
		.colorBlendOp			= VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp			= VK_BLEND_OP_ADD,
		.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	},
};

static const VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
	.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	.pNext				= NULL,
	.flags				= 0,
	.logicOpEnable		= VK_FALSE,
	.logicOp			= VK_LOGIC_OP_COPY,
	.attachmentCount	= ARRAY_LEN(colorBlendAttachments),
	.pAttachments		= colorBlendAttachments,
	.blendConstants[0]	= 0.0f,
	.blendConstants[1]	= 0.0f,
	.blendConstants[2]	= 0.0f,
	.blendConstants[3]	= 0.0f,
};

static const VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR,
};

static const VkPipelineDynamicStateCreateInfo dynamicInfo = {
	.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	.pNext				= NULL,
	.flags				= 0,
	.dynamicStateCount	= ARRAY_LEN(dynamicStates),
	.pDynamicStates		= dynamicStates,
};

VkPipeline mainPipeline = VK_NULL_HANDLE;

void vkCreateGraphicsPipelineWrap(void) {
	const VkPipelineShaderStageCreateInfo shaderStageInfo[] = {
		VK_SHADER(VK_SHADER_STAGE_VERTEX_BIT, vertexShader),
		VK_SHADER(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader),
	};

	const VkGraphicsPipelineCreateInfo infos[] = {
		[VK_GRAPHICS_PIPELINE_INDEX_MAIN] = {
			.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext					= NULL,
			.flags					= 0,
			.stageCount				= ARRAY_LEN(shaderStageInfo),
			.pStages				= shaderStageInfo,
			.pVertexInputState		= &vertexInputInfo,
			.pInputAssemblyState	= &inputAssemblyInfo,
			.pTessellationState		= NULL,
			.pViewportState			= &viewportInfo,
			.pRasterizationState	= &rasterizationInfo,
			.pMultisampleState		= &multisampleInfo,
			.pDepthStencilState		= NULL,
			.pColorBlendState		= &colorBlendInfo,
			.pDynamicState			= &dynamicInfo,
			.layout					= mainPipelineLayout,
			.renderPass				= renderPass,
			.subpass				= VK_SUBPASS_INDEX_MAIN,
			.basePipelineHandle		= VK_NULL_HANDLE,
			.basePipelineIndex		= 0,
		},
	};

	const VkResult result = vkCreateGraphicsPipelines(
		mainDevice, mainPipelineCache, ARRAY_LEN(infos), infos, allocationCallbacks, &mainPipeline
	);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Main pipeline created!");

	// Shaders are no longer needed
	vkDestroyShaderModulesWrap();
}

void vkDestroyGraphicsPipelineWrap(void) {
	if (mainPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(mainDevice, mainPipeline, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main pipeline destroyed!");
		mainPipeline = VK_NULL_HANDLE;
	}
}
