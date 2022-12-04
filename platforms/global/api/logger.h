#pragma once

#include <stdarg.h>
#include <stddef.h>

#define rkLog(logLevel, msg, ...) \
	rkLogTaged(logLevel, NULL, msg, ##__VA_ARGS__)

#define rkLogV(logLevel, msg, args) \
	rkLogTagedV(logLevel, NULL, msg, args)

typedef enum {
	RK_LOG_LEVEL_MIN,
	RK_LOG_LEVEL_ERROR = RK_LOG_LEVEL_MIN,
	RK_LOG_LEVEL_WARNING,
	RK_LOG_LEVEL_INFO,
	RK_LOG_LEVEL_DEBUG,
	RK_LOG_LEVEL_MAX,
} RkLogLevel;

void rkLogTaged(const RkLogLevel logLevel, const char * const * const restrict tags, const char * const msg, ...);
void rkLogTagedV(const RkLogLevel logLevel, const char * const * const restrict tags, const char * const msg, va_list args);
