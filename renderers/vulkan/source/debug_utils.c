#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/helper.h>
#include <defines.h>
#include <asserts.h>
#include <options.h>
#include <logger.h>
#include <stdarg.h>

static const char * const vulkanTags[] = {
	RK_STR(VULKAN_TAG_INFO),
	RK_STR(VULKAN_TAG_DEBUG),
};

static const RkLogLevel vulkanTagLogLevels[] = {
	RK_VAL(VULKAN_TAG_INFO, RK_LOG_LEVEL_INFO),
	RK_VAL(VULKAN_TAG_DEBUG, RK_LOG_LEVEL_DEBUG),
};

static RkLogLevel vkDebugUtilsMessageSeverityGetLogLevel(const VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
	switch (severity) {
	RK_CVT(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, RK_LOG_LEVEL_DEBUG);
	RK_CVT(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, RK_LOG_LEVEL_INFO);
	RK_CVT(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, RK_LOG_LEVEL_WARNING);
	RK_CVT(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, RK_LOG_LEVEL_ERROR);
	default: return RK_LOG_LEVEL_MAX;
	}
}

static const char * vkDebugUtilsMessageTypeGetStr(const VkDebugUtilsMessageTypeFlagBitsEXT type) {
	switch (type) {
	RK_CASE(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
	RK_CASE(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
	RK_CASE(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
	default: return NULL;
	}
}

static VkBool32 VKAPI_PTR vkDebugUtilsMessengerCallback(
		const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		const VkDebugUtilsMessageTypeFlagsEXT types,
		const VkDebugUtilsMessengerCallbackDataEXT * const restrict callbackData,
		UNUSED void * const userData)
{
	const RkLogLevel logLevel = vkDebugUtilsMessageSeverityGetLogLevel(severity);
	const char * tags[CONFIG_VULKAN_VALIDATION_TAGS_MAX] = { NULL };

	vkFlagsToStr(types, VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT,
				 vkDebugUtilsMessageTypeGetStr, tags);
	rkLogTaged(logLevel, tags, callbackData->pMessage);

	return VK_FALSE;
}

static VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;

static const VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
	.sType				= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
	.pNext				= NULL,
	.flags				= 0,
	.messageSeverity	= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
	.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
	.pfnUserCallback	= vkDebugUtilsMessengerCallback,
	.pUserData			= NULL,
};

const VkDebugUtilsMessengerCreateInfoEXT * vkDebugUtilsMessengerCreateInfoGet(void) {
	return (cmdOptions.vkValidation != VK_FALSE ? &debugUtilsMessengerCreateInfo : NULL);
}

void vkCreateDebugUtilsMessengerWrap(void) {
	if (cmdOptions.vkValidation != VK_FALSE) {
		const VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfo,
															   allocationCallbacks, &debugUtilsMessenger);
		RK_VULKAN_ASSERT(result, NULL);
		vkLog(VULKAN_TAG_INFO, "Debug messenger created!");
	}
}

void vkDestroyDebugUtilsMessengerWrap(void) {
	if (debugUtilsMessenger != VK_NULL_HANDLE) {
		vkDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Debug messenger destroyed!");
		debugUtilsMessenger = VK_NULL_HANDLE;
	}
}

void vkLog(const VulkanTag tag, const char * const msg, ...) {
	const char * const tags[] = {
		vulkanTags[tag],
		RK_VAL(VULKAN_TAG_MAX, NULL),
	};

	va_list args;
	va_start(args, msg);
	rkLogTagedV(vulkanTagLogLevels[tag], tags, msg, args);
	va_end(args);
}
