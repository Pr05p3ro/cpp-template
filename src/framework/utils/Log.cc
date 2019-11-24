#include <cstdarg>
#include <android/log.h>
#include "Log.h"

namespace game {

void Log::info(const char *pMessage, ...) {
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_INFO, "Template", pMessage, varArgs);
    va_end(varArgs);
}

void Log::error(const char *pMessage, ...) {
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_ERROR, "Template", pMessage, varArgs);
    va_end(varArgs);
}

void Log::warn(const char *pMessage, ...) {
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_WARN, "Template", pMessage, varArgs);
    __android_log_print(ANDROID_LOG_WARN, "Template", "\n");
    va_end(varArgs);
}

void Log::debug(const char *pMessage, ...) {
    va_list varArgs;
    va_start(varArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_DEBUG, "Template", pMessage, varArgs);
    va_end(varArgs);
}

} // namespace game