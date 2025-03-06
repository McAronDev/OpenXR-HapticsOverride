#pragma once

#include <string_view>

namespace openxr_api_layer::log {

    // General logging function.
    void Log(const char* fmt, ...);
    static inline void Log(const std::string_view& str) {
        Log(str.data());
    }

    // Debug logging function. Can make things very slow (only enabled on Debug builds).
    void DebugLog(const char* fmt, ...);
    static inline void DebugLog(const std::string_view& str) {
        Log(str.data());
    }

    // Error logging function. Goes silent after too many errors.
    void ErrorLog(const char* fmt, ...);
    static inline void ErrorLog(const std::string_view& str) {
        Log(str.data());
    }

    void EnableDebugLog();

    void startLogging();
    void stopLogging();

} // namespace openxr_api_layer::log
