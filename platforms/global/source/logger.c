#include <options.h>
#include <asserts.h>
#include <defines.h>
#include <stdbool.h>
#include <logger.h>
#include <stdio.h>

#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_RESET	"\x1b[0m"

static const char * const logLevelStr[] = {
	RK_STR(RK_LOG_LEVEL_ERROR),
	RK_STR(RK_LOG_LEVEL_WARNING),
	RK_STR(RK_LOG_LEVEL_INFO),
	RK_STR(RK_LOG_LEVEL_DEBUG),
};

static const char * const logLevelColors[] = {
	RK_VAL(RK_LOG_LEVEL_ERROR, ANSI_COLOR_RED),
	RK_VAL(RK_LOG_LEVEL_WARNING, ANSI_COLOR_YELLOW),
	RK_VAL(RK_LOG_LEVEL_INFO, ANSI_COLOR_GREEN),
	RK_VAL(RK_LOG_LEVEL_DEBUG, ANSI_COLOR_BLUE),
	RK_VAL(RK_LOG_LEVEL_MAX, ANSI_COLOR_RESET),
};

static void rkPrintColor(const RkLogLevel logLevel) {
	if (cmdOptions.logColor != false) {
		printf("%s", logLevelColors[logLevel]);
	}
}

static void rkPrintTag(const RkLogLevel logLevel, const char * const tag) {
	rkPrintColor(logLevel);
	printf("[%s]", tag);
	rkPrintColor(RK_LOG_LEVEL_MAX);
}

void rkLogTaged(const RkLogLevel logLevel, const char * const * const restrict tags, const char * const msg, ...) {
	va_list args;
	va_start(args, msg);
	rkLogTagedV(logLevel, tags, msg, args);
	va_end(args);
}

void rkLogTagedV(const RkLogLevel logLevel, const char * const * const restrict tags, const char * const msg, va_list args) {
	switch (logLevel) {
	case RK_LOG_LEVEL_WARNING:
		if (cmdOptions.logWarning == false) {
			return;
		} break;
	case RK_LOG_LEVEL_INFO:
		if (cmdOptions.logInfo == false) {
			return;
		} break;
	case RK_LOG_LEVEL_DEBUG:
		if (cmdOptions.logDebug == false) {
			return;
		} break;
	default:
		break;
	}

	rkPrintTag(logLevel, logLevelStr[logLevel]);

	if (tags != NULL) {
		for (uint_fast8_t i = 0; tags[i] != NULL; i++) {
			rkPrintTag(logLevel, tags[i]);
		}
	}

	putchar(' ');
	vprintf(msg, args);
	putchar('\n');
	fflush(stdout);
}
