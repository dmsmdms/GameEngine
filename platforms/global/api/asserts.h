#pragma once

#include <autoconf.h>
#include <stdint.h>

#define RK_ASSERT(cond, msg, ...)							\
	if (cond) {												\
		rkAssert(__FILE__, __LINE__, msg, ##__VA_ARGS__);	\
	} (void)(cond)

typedef enum {
	RK_RESULT_MIN,
	RK_RESULT_SUCCESS = RK_RESULT_MIN,
	RK_RESULT_UNDEFINED_OPTION,
	RK_RESULT_FILE_OPEN,
	RK_RESULT_FILE_STAT,
#ifdef CONFIG_RENDERER_VULKAN
	RK_RESULT_VULKAN_DEVICE_NOT_FOUND,
#endif
	RK_RESULT_MAX,
} RkResult;

_Noreturn void rkAssert(const char * const file, const unsigned line, const char * const msg, ...);
_Noreturn void rkExit(const RkResult result, const char * const msg, ...);

#ifdef CONFIG_WSI_XCB
#define RK_XCB_ASSERT(connection, msg, ...)									\
	if (xcb_connection_has_error(connection)) {								\
		rkXcbAssert(connection, __FILE__, __LINE__, msg, ##__VA_ARGS__);	\
	} (void)(connection)

_Noreturn void rkXcbAssert(void * const connection, const char * const file,
						   const unsigned line, const char * const msg, ...);
#endif

#ifdef CONFIG_RENDERER_VULKAN
#define RK_VULKAN_ASSERT(result, msg, ...)								\
	if (result != VK_SUCCESS) {											\
		rkVulkanAssert(result, __FILE__, __LINE__, msg, ##__VA_ARGS__);	\
	} (void)(result)

_Noreturn void rkVulkanAssert(const uint32_t result, const char * const file,
							  const unsigned line, const char * const msg, ...);
#endif
