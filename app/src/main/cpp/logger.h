#ifndef ZOMDROID_LOGGER_H
#define ZOMDROID_LOGGER_H

#ifdef __ANDROID__
#include <android/log.h>

#define _LOG(priority, fmt, ...) \
  ((void)__android_log_print((priority), (LOG_TAG), (fmt), ##__VA_ARGS__))

#define LOGF(fmt, ...) _LOG(ANDROID_LOG_FATAL, "[%s] " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...) _LOG(ANDROID_LOG_ERROR, "[%s] " fmt, __func__, ##__VA_ARGS__)
#define LOGW(fmt, ...) _LOG(ANDROID_LOG_WARN, (fmt), ##__VA_ARGS__)
#define LOGI(fmt, ...) _LOG(ANDROID_LOG_INFO, (fmt), ##__VA_ARGS__)
#define LOGD(fmt, ...) _LOG(ANDROID_LOG_DEBUG, (fmt), ##__VA_ARGS__)
#define LOGV(fmt, ...) _LOG(ANDROID_LOG_VERBOSE, (fmt), ##__VA_ARGS__)
#else
#include <stdio.h>

#define _LOG_STDERR(level, fmt, ...) \
  ((void)fprintf(stderr, "[" level "][%s] " fmt "\n", LOG_TAG, ##__VA_ARGS__))

#define LOGF(fmt, ...) _LOG_STDERR("F", "[%s] " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...) _LOG_STDERR("E", "[%s] " fmt, __func__, ##__VA_ARGS__)
#define LOGW(fmt, ...) _LOG_STDERR("W", (fmt), ##__VA_ARGS__)
#define LOGI(fmt, ...) _LOG_STDERR("I", (fmt), ##__VA_ARGS__)
#define LOGD(fmt, ...) _LOG_STDERR("D", (fmt), ##__VA_ARGS__)
#define LOGV(fmt, ...) _LOG_STDERR("V", (fmt), ##__VA_ARGS__)
#endif

#endif //ZOMDROID_LOGGER_H
