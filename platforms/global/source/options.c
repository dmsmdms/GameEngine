#include <simple-math.h>
#include <options.h>
#include <asserts.h>
#include <stdbool.h>
#include <defines.h>
#include <config.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>

#define RK_OPT_HELP(key, help) \
	[key] = { key, RK_OPT_TYPE_HELP, {}, NULL, help }
#define RK_OPT_SELECT(key, value, data, help) \
	[key] = { key, RK_OPT_TYPE_SELECT, { .select = data }, value, help }
#define RK_OPT_NUMBER(key, value, min, max, help) \
	[key] = { key, RK_OPT_TYPE_NUMBER, { .number = { min, max }}, value, help }
#define RK_OPT_STRING(key, value, min, max, help) \
	[key] = { key, RK_OPT_TYPE_STRING, { .string = { min, max }}, value, help }
#define RK_OPT_BOOL(key, value, help) \
	[key] = { key, RK_OPT_TYPE_SELECT, { .select = boolOptSelectData }, value, help }
#define RK_OPT(key, name, arg) \
	[key] = { name, arg, NULL, key }
#define RK_OPT_CALLBACK(type, parser, helper) \
	[type] = { parser, helper }
#define RK_OPT_SELECT_DATA(key, value) \
	[key] = { value }

typedef enum {
	RK_OPT_KEY_MIN,
	RK_OPT_KEY_HELP = RK_OPT_KEY_MIN,
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT_KEY_WSI,
#endif
	RK_OPT_KEY_RENDERER,
	RK_OPT_KEY_WIDTH,
	RK_OPT_KEY_HEIGHT,
	RK_OPT_KEY_FULLSCREEN,
	RK_OPT_KEY_LOG_WARNING,
	RK_OPT_KEY_LOG_INFO,
	RK_OPT_KEY_LOG_DEBUG,
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT_KEY_LOG_COLOR,
#endif
	RK_OPT_KEY_IMAGE_COUNT,
#ifdef CONFIG_RENDERER_VULKAN
	RK_OPT_KEY_VK_VALIDATION,
	RK_OPT_KEY_VK_VALIDATION_LAYER,
	RK_OPT_KEY_VK_DEBUG_MARKER,
#endif
	RK_OPT_KEY_MAX,
	RK_OPT_KEY_END = -1,
	RK_OPT_KEY_UNDEFINED = '?',
} RkOptKey;

typedef enum {
	RK_OPT_TYPE_MIN,
	RK_OPT_TYPE_HELP = RK_OPT_TYPE_MIN,
	RK_OPT_TYPE_SELECT,
	RK_OPT_TYPE_NUMBER,
	RK_OPT_TYPE_STRING,
	RK_OPT_TYPE_MAX,
} RkOptType;

typedef enum {
	RK_OPT_BOOL_MIN,
	RK_OPT_BOOL_FALSE = RK_OPT_BOOL_MIN,
	RK_OPT_BOOL_TRUE,
	RK_OPT_BOOL_MAX,
} RkOptBool;

typedef struct {
	const char * value;
} RkOptSelectData;

typedef struct {
	int_fast32_t min;
	int_fast32_t max;
} RkOptNumberData;

typedef struct {
	uint_fast8_t min;
	uint_fast8_t max;
} RkOptStringData;

typedef union {
	const RkOptSelectData * select;
	RkOptNumberData number;
	RkOptStringData string;
} RkOptData;

typedef struct {
	RkOptKey key;
	RkOptType type;
	RkOptData data;
	void * value;
	const char * help;
} RkOpt;

typedef void (* RkOptParser)(const RkOpt * const restrict opt, const char * const arg);
static void rkOptParserHelp(const RkOpt * const restrict opt, const char * const arg);
static void rkOptParserSelect(const RkOpt * const restrict opt, const char * const arg);
static void rkOptParserNumber(const RkOpt * const restrict opt, const char * const arg);
static void rkOptParserString(const RkOpt * const restrict opt, const char * const arg);

typedef void (* RkOptHelper)(const RkOpt * const restrict opt);
static void rkOptHelperHelp(const RkOpt * const restrict opt);
static void rkOptHelperSelect(const RkOpt * const restrict opt);
static void rkOptHelperNumber(const RkOpt * const restrict opt);
static void rkOptHelperString(const RkOpt * const restrict opt);

typedef struct {
	RkOptParser parser;
	RkOptHelper helper;
} RkOptCallback;

static const RkOptCallback optCallback[] = {
	RK_OPT_CALLBACK(RK_OPT_TYPE_HELP, rkOptParserHelp, rkOptHelperHelp),
	RK_OPT_CALLBACK(RK_OPT_TYPE_SELECT, rkOptParserSelect, rkOptHelperSelect),
	RK_OPT_CALLBACK(RK_OPT_TYPE_NUMBER, rkOptParserNumber, rkOptHelperNumber),
	RK_OPT_CALLBACK(RK_OPT_TYPE_STRING, rkOptParserString, rkOptHelperString),
};

#ifdef CONFIG_PLATFORM_LINUX
static const RkOptSelectData wsiOptSelectData[] = {
#ifdef CONFIG_WSI_XCB
	RK_OPT_SELECT_DATA(RK_WSI_TYPE_XCB, "xcb"),
#endif
#ifdef CONFIG_WSI_WAYLAND
	RK_OPT_SELECT_DATA(RK_WSI_TYPE_WAYLAND, "wayland"),
#endif
	RK_OPT_SELECT_DATA(RK_WSI_TYPE_MAX, NULL),
};
#endif

static const RkOptSelectData rendererOptSelectData[] = {
#ifdef CONFIG_RENDERER_VULKAN
	RK_OPT_SELECT_DATA(RK_RENDERER_TYPE_VULKAN, "vulkan"),
#endif
	RK_OPT_SELECT_DATA(RK_RENDERER_TYPE_MAX, NULL),
};

static const RkOptSelectData boolOptSelectData[] = {
	RK_OPT_SELECT_DATA(RK_OPT_BOOL_FALSE, "off"),
	RK_OPT_SELECT_DATA(RK_OPT_BOOL_TRUE, "on"),
	RK_OPT_SELECT_DATA(RK_OPT_BOOL_MAX, NULL),
};

static const RkOpt cmdOpt[] = {
	RK_OPT_HELP(RK_OPT_KEY_HELP, "print this help message"),
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT_SELECT(RK_OPT_KEY_WSI, &cmdOptions.wsiType,
		wsiOptSelectData, "select window system interface"),
#endif
	RK_OPT_SELECT(RK_OPT_KEY_RENDERER, &cmdOptions.rendererType,
		rendererOptSelectData, "select renderer backend"),
	RK_OPT_NUMBER(RK_OPT_KEY_WIDTH, &cmdOptions.width, CONFIG_RENDERER_MIN_WIDTH,
		CONFIG_RENDERER_MAX_WIDTH, "set window width"),
	RK_OPT_NUMBER(RK_OPT_KEY_HEIGHT, &cmdOptions.height, CONFIG_RENDERER_MIN_HEIGHT,
		CONFIG_RENDERER_MAX_HEIGHT, "set window height"),
	RK_OPT_BOOL(RK_OPT_KEY_FULLSCREEN, &cmdOptions.fullscreen, "enable fullscreen mode"),
	RK_OPT_BOOL(RK_OPT_KEY_LOG_WARNING, &cmdOptions.logWarning, "enable warning logs"),
	RK_OPT_BOOL(RK_OPT_KEY_LOG_INFO, &cmdOptions.logInfo, "enable info logs"),
	RK_OPT_BOOL(RK_OPT_KEY_LOG_DEBUG, &cmdOptions.logDebug, "enable debug logs"),
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT_BOOL(RK_OPT_KEY_LOG_COLOR, &cmdOptions.logColor, "enable color output"),
#endif
	RK_OPT_NUMBER(RK_OPT_KEY_IMAGE_COUNT, &cmdOptions.imageCount, CONFIG_RENDERER_MIN_IMAGE_COUNT,
		CONFIG_RENDERER_MAX_IMAGE_COUNT, "set image count (double/triple buffering)"),
#ifdef CONFIG_RENDERER_VULKAN
	RK_OPT_BOOL(RK_OPT_KEY_VK_VALIDATION, &cmdOptions.vkValidation, "enable vulkan validation"),
	RK_OPT_STRING(RK_OPT_KEY_VK_VALIDATION_LAYER, &cmdOptions.vkValidationLayer,
		CONFIG_VULKAN_VALIDATION_LAYER_MIN, CONFIG_VULKAN_VALIDATION_LAYER_MAX,
		"vulkan validation layer name"),
	RK_OPT_BOOL(RK_OPT_KEY_VK_DEBUG_MARKER, &cmdOptions.vkDebugMarker,
		"enable vulkan debug marker, used by RenderDoc"),
#endif
};

static const struct option cmdOptionsInfo[] = {
	RK_OPT(RK_OPT_KEY_HELP, "help", no_argument),
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT(RK_OPT_KEY_WSI, "wsi", required_argument),
#endif
	RK_OPT(RK_OPT_KEY_RENDERER, "renderer", required_argument),
	RK_OPT(RK_OPT_KEY_WIDTH, "width", required_argument),
	RK_OPT(RK_OPT_KEY_HEIGHT, "height", required_argument),
	RK_OPT(RK_OPT_KEY_FULLSCREEN, "fullscreen", required_argument),
	RK_OPT(RK_OPT_KEY_LOG_WARNING, "log-warning", required_argument),
	RK_OPT(RK_OPT_KEY_LOG_INFO, "log-info", required_argument),
	RK_OPT(RK_OPT_KEY_LOG_DEBUG, "log-debug", required_argument),
#ifdef CONFIG_PLATFORM_LINUX
	RK_OPT(RK_OPT_KEY_LOG_COLOR, "log-color", required_argument),
#endif
	RK_OPT(RK_OPT_KEY_IMAGE_COUNT, "image-count", required_argument),
#ifdef CONFIG_RENDERER_VULKAN
	RK_OPT(RK_OPT_KEY_VK_VALIDATION, "vulkan-validation", required_argument),
	RK_OPT(RK_OPT_KEY_VK_VALIDATION_LAYER, "vulkan-validation-layer", required_argument),
	RK_OPT(RK_OPT_KEY_VK_DEBUG_MARKER, "vulkan-debug-marker", required_argument),
#endif
	RK_OPT(RK_OPT_KEY_MAX, NULL, no_argument),
};

volatile uint_fast8_t appIsRunning = true;

RkCmdOptions cmdOptions = {
#ifdef CONFIG_PLATFORM_LINUX
	.wsiType			= CONFIG_WSI_TYPE,
#endif
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

static void rkOptParserHelp(UNUSED const RkOpt * const restrict opt, UNUSED const char * const arg) {
	for (RkOptKey key = RK_OPT_KEY_MIN; key < RK_OPT_KEY_MAX; key++) {
		const RkOptType type = cmdOpt[key].type;
		const RkOptHelper helper = optCallback[type].helper;
		helper(&cmdOpt[key]);
	}

	rkExit(RK_RESULT_SUCCESS, NULL);
}

static void rkOptParserSelect(const RkOpt * const restrict opt, const char * const arg) {
	const RkOptSelectData * restrict const select = opt->data.select;
	const char * const name = cmdOptionsInfo[opt->key].name;

	for (uint_fast8_t key = 0; select[key].value != NULL; key++) {
		if (strcmp(arg, select[key].value) == RK_RESULT_SUCCESS) {
			*(uint8_t *)opt->value = key;
			return;
		}
	}

	rkLog(RK_LOG_LEVEL_WARNING, "Unexpected value --%s=%s", name, arg);
}

static void rkOptParserNumber(const RkOpt * const restrict opt, const char * const arg) {
	const unsigned long numberMin = opt->data.number.min;
	const unsigned long numberMax = opt->data.number.max;
	const unsigned long abloluteMax = max(numberMin, numberMax);
	const char * const name = cmdOptionsInfo[opt->key].name;
	const long newNumber = atol(arg);

	if (newNumber >= (long)numberMin && newNumber <= (long)numberMax) {
		if (abloluteMax < UINT8_MAX) {
			*(uint8_t *)opt->value = newNumber;
		} else if (abloluteMax < UINT16_MAX) {
			*(uint16_t *)opt->value = newNumber;
		} else {
			*(uint32_t *)opt->value = newNumber;
		}

		return;
	}

	rkLog(RK_LOG_LEVEL_WARNING, "Unexpected value --%s=%s (min: %lu, max: %lu)",
		  name, arg, numberMin, numberMax);
}

static void rkOptParserString(const RkOpt * const restrict opt, const char * const arg) {
	const unsigned minLength = opt->data.string.min;
	const unsigned maxLength = opt->data.string.max;
	const unsigned strLength = strlen(arg);
	const char * const name = cmdOptionsInfo[opt->key].name;

	if (strLength >= minLength && strLength <= maxLength) {
		*(const char **)opt->value = arg;
		return;
	}

	rkLog(RK_LOG_LEVEL_WARNING, "Unexpected value --%s=%s (minLength: %u, maxLength: %u)",
		  name, arg, minLength, maxLength);
}

static void rkOptHelperHelp(const RkOpt * const restrict opt) {
	const char * const name = cmdOptionsInfo[opt->key].name;
	printf("--%s - %s.\n", name, opt->help);
}

static const char * rkOptGetSelect(const RkOpt * const restrict opt) {
	const uint_fast8_t key = *(uint_fast8_t *)opt->value;
	return opt->data.select[key].value;
}

static void rkOptHelperSelect(const RkOpt * const restrict opt) {
	const RkOptSelectData * restrict const select = opt->data.select;
	const char * const name = cmdOptionsInfo[opt->key].name;
	const char * const value = rkOptGetSelect(opt);
	const char * separator = "";

	printf("--%s=%s - %s. [", name, value, opt->help);

	for (uint_fast8_t key = 0; select[key].value != NULL; key++) {
		printf("%s'%s'", separator, select[key].value);
		separator = ", ";
	}

	printf("]\n");
}

static int_fast32_t rkOptGetNumber(const RkOpt * const restrict opt) {
	const uint_fast32_t numberMin = opt->data.number.min;
	const uint_fast32_t numberMax = opt->data.number.max;
	const uint_fast32_t abloluteMax = max(numberMin, numberMax);
	int_fast32_t value;

	if (abloluteMax < UINT8_MAX) {
		value = *(uint8_t *)opt->value;
	} else if (abloluteMax < UINT16_MAX) {
		value = *(uint16_t *)opt->value;
	} else {
		value = *(uint32_t *)opt->value;
	}

	return value;
}

static void rkOptHelperNumber(const RkOpt * const restrict opt) {
	const char * const name = cmdOptionsInfo[opt->key].name;
	const unsigned long numberMin = opt->data.number.min;
	const unsigned long numberMax = opt->data.number.max;
	const unsigned long value = rkOptGetNumber(opt);

	printf("--%s=%lu - %s. (min: %lu, max: %lu)\n", name, value,
		   opt->help, numberMin, numberMax);
}

static void rkOptHelperString(const RkOpt * const restrict opt) {
	const char * const name = cmdOptionsInfo[opt->key].name;
	const unsigned numberMin = opt->data.string.min;
	const unsigned numberMax = opt->data.string.max;

	printf("--%s=%s - %s. (minLength: %u, maxLength: %u)\n", name,
		   *(const char **)opt->value, opt->help, numberMin, numberMax);
}

void rkParseCmdOptions(const int argc, char ** const argv) {
	while (true) {
		const RkOptKey key = getopt_long(argc, argv, "", cmdOptionsInfo, NULL);

		if (key == RK_OPT_KEY_UNDEFINED) {
			rkExit(RK_RESULT_UNDEFINED_OPTION, NULL);
		} else if (key == RK_OPT_KEY_END) {
			return;
		}

		const RkOptType type = cmdOpt[key].type;
		const RkOptParser parser = optCallback[type].parser;
		parser(&cmdOpt[key], optarg);
	}
}
