#include <asserts.h>
#include <options.h>
#include <defines.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <logger.h>
#include <stdio.h>
#include <errno.h>
#ifdef CONFIG_WSI_XCB
#include <xcb/xcb.h>
#endif
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/vulkan.h>
#endif

typedef enum {
	RK_TAG_MIN,
	RK_TAG_EXIT = RK_TAG_MIN,
	RK_TAG_ASSERT,
	RK_TAG_MAX,
} RkTag;

static const char * const tagStr[] = {
	RK_STR(RK_TAG_EXIT),
	RK_STR(RK_TAG_ASSERT),
};

static const char * const resultStr[] = {
	RK_STR(RK_RESULT_SUCCESS),
	RK_STR(RK_RESULT_UNDEFINED_OPTION),
	RK_STR(RK_RESULT_FILE_OPEN),
	RK_STR(RK_RESULT_FILE_STAT),
#ifdef CONFIG_RENDERER_VULKAN
	RK_STR(RK_RESULT_VULKAN_DEVICE_NOT_FOUND),
#endif
};

static void rkGlobalAssert(char * restrict buffer, const char * const file,
						   const unsigned line, const char * const msg)
{
	buffer += sprintf(buffer, "In file %s:%u!", file, line);

	if (errno != RK_RESULT_SUCCESS) {
		buffer += sprintf(buffer, " (errno - %s) ", strerror(errno));
	}

	if (msg != NULL) {
		strcat(buffer, msg);
	}
}

_Noreturn void rkAssert(const char * const file, const unsigned line, const char * const msg, ...) {
	char buffer[CONFIG_LOG_BUFFER_SIZE];
	rkGlobalAssert(buffer, file, line, msg);

	const char * const tags[] = {
		tagStr[RK_TAG_ASSERT],
		NULL,
	};

	va_list args;
	va_start(args, msg);
	rkLogTagedV(RK_LOG_LEVEL_ERROR, tags, buffer, args);
	va_end(args);

	exit(EXIT_FAILURE);
}

_Noreturn void rkExit(const RkResult result, const char * const msg, ...) {
	RkLogLevel logLevel = RK_LOG_LEVEL_INFO;
	const char * resultTag = NULL;
	int exitCode = EXIT_SUCCESS;

	if (result != RK_RESULT_SUCCESS) {
		resultTag = resultStr[result];
		logLevel = RK_LOG_LEVEL_ERROR;
		exitCode = EXIT_FAILURE;
	}

	const char * const tags[] = {
		tagStr[RK_TAG_EXIT],
		resultTag,
		NULL,
	};

	va_list args;
	va_start(args, msg);
	rkLogTagedV(logLevel, tags, msg, args);
	va_end(args);

	exit(exitCode);
}

#ifdef CONFIG_WSI_XCB
static const char * const xcbResultStr[] = {
	RK_STR(XCB_CONN_ERROR),
	RK_STR(XCB_CONN_CLOSED_EXT_NOTSUPPORTED),
	RK_STR(XCB_CONN_CLOSED_MEM_INSUFFICIENT),
	RK_STR(XCB_CONN_CLOSED_REQ_LEN_EXCEED),
	RK_STR(XCB_CONN_CLOSED_PARSE_ERR),
	RK_STR(XCB_CONN_CLOSED_INVALID_SCREEN),
};

_Noreturn void rkXcbAssert(void * const connection, const char * const file,
						   const unsigned line, const char * const msg, ...)
{
	char buffer[CONFIG_LOG_BUFFER_SIZE];
	rkGlobalAssert(buffer, file, line, msg);

	const int result = xcb_connection_has_error(connection);
	const char * const tags[] = {
		tagStr[RK_TAG_ASSERT],
		xcbResultStr[result],
		NULL,
	};

	va_list args;
	va_start(args, msg);
	rkLogTagedV(RK_LOG_LEVEL_ERROR, tags, buffer, args);
	va_end(args);

	exit(EXIT_FAILURE);
}
#endif

#ifdef CONFIG_RENDERER_VULKAN
static const char * vkResultGetStr(const VkResult result) {
	switch (result) {
		RK_CASE(VK_NOT_READY);
		RK_CASE(VK_TIMEOUT);
		RK_CASE(VK_EVENT_SET);
		RK_CASE(VK_EVENT_RESET);
		RK_CASE(VK_INCOMPLETE);
		RK_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
		RK_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		RK_CASE(VK_ERROR_INITIALIZATION_FAILED);
		RK_CASE(VK_ERROR_DEVICE_LOST);
		RK_CASE(VK_ERROR_MEMORY_MAP_FAILED);
		RK_CASE(VK_ERROR_LAYER_NOT_PRESENT);
		RK_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
		RK_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
		RK_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
		RK_CASE(VK_ERROR_TOO_MANY_OBJECTS);
		RK_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		RK_CASE(VK_ERROR_FRAGMENTED_POOL);
		RK_CASE(VK_ERROR_UNKNOWN);
		RK_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
		RK_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		RK_CASE(VK_ERROR_FRAGMENTATION);
		RK_CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
		RK_CASE(VK_ERROR_SURFACE_LOST_KHR);
		RK_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		RK_CASE(VK_SUBOPTIMAL_KHR);
		RK_CASE(VK_ERROR_OUT_OF_DATE_KHR);
		RK_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		RK_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
		RK_CASE(VK_ERROR_INVALID_SHADER_NV);
		RK_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		RK_CASE(VK_ERROR_NOT_PERMITTED_EXT);
		RK_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
		default: return NULL;
	}
}

_Noreturn void rkVulkanAssert(const uint32_t result, const char * const file,
							  const unsigned line, const char * const msg, ...)
{
	char buffer[CONFIG_LOG_BUFFER_SIZE];
	rkGlobalAssert(buffer, file, line, msg);

	const char * const tags[] = {
		tagStr[RK_TAG_ASSERT],
		vkResultGetStr(result),
		NULL,
	};

	va_list args;
	va_start(args, msg);
	rkLogTagedV(RK_LOG_LEVEL_ERROR, tags, buffer, args);
	va_end(args);

	exit(EXIT_FAILURE);
}
#endif
