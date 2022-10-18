#include <options.h>
#include <stdbool.h>
#include <config.h>

volatile uint_fast8_t appIsRunning = true;

RkCmdOptions cmdOptions = {
	.rendererType		= CONFIG_RENDERER_TYPE,
	.width				= CONFIG_RENDERER_WIDTH,
	.height				= CONFIG_RENDERER_HEIGHT,
	.fullscreen			= CONFIG_RENDERER_FULLSCREEN,
	.logWarning			= CONFIG_LOG_WARNING,
	.logInfo			= CONFIG_LOG_INFO,
	.logDebug			= CONFIG_LOG_DEBUG,
	.logColor			= CONFIG_LOG_COLOR,
	.imageCount			= CONFIG_RENDERER_IMAGE_COUNT,
#ifdef CONFIG_RENDERER_VULKAN
	.vkValidation		= CONFIG_VULKAN_VALIDATION,
	.vkValidationLayer	= CONFIG_VULKAN_VALIDATION_LAYER,
	.vkDebugMarker		= CONFIG_VULKAN_DEBUG_MARKER,
#endif
};
