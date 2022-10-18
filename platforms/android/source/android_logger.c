#include <android/log.h>
#include <options.h>
#include <asserts.h>
#include <defines.h>
#include <stdbool.h>
#include <logger.h>
#include <stdio.h>

static const char * const logLevelStr[] = {
	RK_STR(RK_LOG_LEVEL_ERROR),
	RK_STR(RK_LOG_LEVEL_WARNING),
	RK_STR(RK_LOG_LEVEL_INFO),
	RK_STR(RK_LOG_LEVEL_DEBUG),
};

static const android_LogPriority logPriority[] = {
	RK_VAL(RK_LOG_LEVEL_ERROR, ANDROID_LOG_ERROR),
	RK_VAL(RK_LOG_LEVEL_WARNING, ANDROID_LOG_WARN),
	RK_VAL(RK_LOG_LEVEL_INFO, ANDROID_LOG_INFO),
	RK_VAL(RK_LOG_LEVEL_DEBUG, ANDROID_LOG_DEBUG),
};

static void rkPrintTag(const RkLogLevel logLevel, const char * const tag) {
	__android_log_print(logPriority[logLevel], CONFIG_PROJECT_NAME, "[%s]", tag);
}

static void rkPrintMsg(const RkLogLevel logLevel, const char * const msg, va_list args) {
	__android_log_vprint(logPriority[logLevel], CONFIG_PROJECT_NAME, msg, args);
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

	char buffer[CONFIG_LOG_BUFFER_SIZE];
	sprintf(buffer, " %s\n", msg);
	rkPrintMsg(logLevel, buffer, args);
}
