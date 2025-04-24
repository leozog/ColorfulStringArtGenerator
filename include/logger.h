#pragma once
#include <chrono>
#include <cstdio>
#include <mutex>
#include <print>

class Logger
{
    static std::mutex mtx;
    static std::chrono::time_point<std::chrono::steady_clock> start_time;

    enum class LogLevel : uint8_t
    {
        INFO,
        WARN,
        ERROR,
        DEBUG
    };

    static constexpr const char* log_level_str(LogLevel level);

    template<LogLevel LOG_LEVEL, typename... Args>
    static void log(const std::format_string<Args...> format, Args&&... args);

public:
    template<typename... Args>
    static void info(const std::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    static void warn(const std::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    static void error(const std::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    static void debug(const std::format_string<Args...> format, Args&&... args);
};

constexpr const char* Logger::log_level_str(LogLevel level)
{
    switch (level) {
        case LogLevel::INFO:
            return "[ {} ms ] {}";
        case LogLevel::WARN:
            return "[ {} ms ] \033[33mWARNING\033[0m: {}"; // Yellow
        case LogLevel::ERROR:
            return "[ {} ms ] \033[31mERROR\033[0m: {}"; // Red
        case LogLevel::DEBUG:
            return "[ {} ms ] \033[90mDEBUG\033[0m: {}"; // Gray
    }
    std::unreachable();
}

template<Logger::LogLevel LOG_LEVEL, typename... Args>
void Logger::log(const std::format_string<Args...> format, Args&&... args)
{
#if defined(NDEBUG)
    if constexpr (LOG_LEVEL == LogLevel::DEBUG) {
        return;
    }
#endif
    std::lock_guard<std::mutex> lock(mtx);

    auto now_time = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - start_time).count();

    std::string formatted_msg = std::format(format, std::forward<Args>(args)...);
    if constexpr (LOG_LEVEL == LogLevel::ERROR) {
        std::println(stderr, log_level_str(LOG_LEVEL), elapsed_ms, formatted_msg);
    } else {
        std::println(log_level_str(LOG_LEVEL), elapsed_ms, formatted_msg);
    }
}

template<typename... Args>
void Logger::info(const std::format_string<Args...> format, Args&&... args)
{
    log<LogLevel::INFO>(format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::warn(const std::format_string<Args...> format, Args&&... args)
{
    log<LogLevel::WARN>(format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::error(const std::format_string<Args...> format, Args&&... args)
{
    log<LogLevel::ERROR>(format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::debug(const std::format_string<Args...> format, Args&&... args)
{
    log<LogLevel::DEBUG>(format, std::forward<Args>(args)...);
}