#pragma once

#include <autoconf.h>
#include <stdint.h>

#ifdef CONFIG_PLATFORM_LINUX
typedef enum {
#ifdef CONFIG_WSI_WAYLAND
	RK_WSI_TYPE_WAYLAND,
#endif
#ifdef CONFIG_WSI_XCB
	RK_WSI_TYPE_XCB,
#endif
	RK_WSI_TYPE_MAX,
	RK_WSI_TYPE_MIN = 0,
} RkWsiType;
#endif

typedef enum {
#ifdef CONFIG_RENDERER_VULKAN
	RK_RENDERER_TYPE_VULKAN,
#endif
	RK_RENDERER_TYPE_MAX,
	RK_RENDERER_TYPE_MIN = 0,
} RkRendererType;

typedef struct {
#ifdef CONFIG_PLATFORM_LINUX
	RkWsiType		wsiType;
#endif
	RkRendererType	rendererType;
	uint_fast16_t	width;
	uint_fast16_t	height;
	uint_fast8_t	fullscreen;
	uint_fast8_t	logWarning;
	uint_fast8_t	logInfo;
	uint_fast8_t	logDebug;
	uint_fast8_t	logColor;
	uint_fast8_t	imageCount;
#ifdef CONFIG_RENDERER_VULKAN
	const char *	vkValidationLayer;
	uint_fast8_t	vkValidation;
	uint_fast8_t	vkDebugMarker;
#endif
} RkCmdOptions;

extern volatile uint_fast8_t appIsRunning;
extern RkCmdOptions cmdOptions;

void rkParseCmdOptions(const int argc, char ** const argv);
