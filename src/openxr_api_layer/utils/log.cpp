#include <fstream>
#include <filesystem>
#include <ctime>
#include <stdarg.h>
#include <windows.h>




namespace {
    constexpr uint32_t k_maxLoggedErrors = 100;
    uint32_t g_globalErrorCount = 0;
} // namespace

namespace openxr_api_layer::log {

    namespace {
        std::ofstream logStream;
        

        bool debugEnabled = false;

        // Utility logging function.
        void InternalLog(const char* fmt, va_list va) {
            const std::time_t now = std::time(nullptr);
            std::tm localTime;

            if (localtime_s(&localTime, &now) != 0) {
                if (logStream.is_open()) {
                    logStream << "Failed to get local time" << std::endl;
                    logStream.flush();
                }
            }

            char buf[1024];
            size_t offset = std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S ", &localTime);
            vsnprintf_s(buf + offset, sizeof(buf) - offset, _TRUNCATE, fmt, va);
           
            if (logStream.is_open()) {
                logStream << buf;
                logStream.flush();
            }
        }
    } // namespace

    void Log(const char* fmt, ...) {
        va_list va;
        va_start(va, fmt);
        InternalLog(fmt, va);
        va_end(va);
    }

    void ErrorLog(const char* fmt, ...) {
        if (g_globalErrorCount++ < k_maxLoggedErrors) {
            va_list va;
            va_start(va, fmt);
            InternalLog(fmt, va);
            va_end(va);
            if (g_globalErrorCount == k_maxLoggedErrors) {
                Log("Maximum number of errors logged. Going silent.");
            }
        }
    }

    void DebugLog(const char* fmt, ...) {
        if (debugEnabled) {
            va_list va;
            va_start(va, fmt);
            InternalLog(fmt, va);
            va_end(va);
        } 
    }

    void startLogging() {

        std::filesystem::path appDataDirectory = std::filesystem::path(getenv("LOCALAPPDATA")) / PROJECT_NAME;
        CreateDirectoryA(appDataDirectory.string().c_str(), nullptr);

        // Start logging to file.
        if (!logStream.is_open()) {
            std::string s_logFile = (appDataDirectory / "log.txt").string();
            logStream.open(s_logFile, std::ios_base::ate);
        }
    }

    void stopLogging() {
        if (!logStream.is_open()) {
            logStream.close();
        }
    }

    void EnableDebugLog() {
        debugEnabled = true;
    }

} // namespace openxr_api_layer::log
