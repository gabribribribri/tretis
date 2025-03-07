#pragma once
#include <cassert>
#include <iostream>

static const constexpr int ERROR_LOG_LEVEL = 1;
static const constexpr int WARNING_LOG_LEVEL = 2;
static const constexpr int INFO_LOG_LEVEL = 3;
static const constexpr int DEBUG_LOG_LEVEL = 4;

class Log {
private:
    inline static int LOG_LEVEL;
    static constexpr const char *const INFO_PREFIX = "\033[1;36m >>> \033[0m";
    static constexpr const char *const WARNING_PREFIX =
        "\033[1;33m >>> \033[0m";
    static constexpr const char *const ERROR_PREFIX = "\033[1;31m >>> \033[0m";
    static constexpr const char *const DEBUG_PREFIX = "\033[1;35m >>> \033[0m";

    template <typename... Args>
    static void Logger(const char *const level_prefix, Args const &...args) {
        std::cout << level_prefix;
        (std::cout << ... << args);
        std::cout << '\n';
    }

public:
    static void SetLevel(int level) {
        assert(level >= ERROR_LOG_LEVEL and level <= DEBUG_LOG_LEVEL);
        LOG_LEVEL = level;
    }

    template <typename... Args>
    static void Error(Args const &...args) {
        if (LOG_LEVEL >= ERROR_LOG_LEVEL) {
            Logger(ERROR_PREFIX, args...);
        }
    }

    template <typename... Args>
    static void Warn(Args const &...args) {
        if (LOG_LEVEL >= INFO_LOG_LEVEL) {
            Logger(WARNING_PREFIX, args...);
        }
    }

    template <typename... Args>
    static void Info(Args const &...args) {
        if (LOG_LEVEL >= INFO_LOG_LEVEL) {
            Logger(INFO_PREFIX, args...);
        }
    }
    template <typename... Args>
    static void Debug(Args const &...args) {
        if (LOG_LEVEL >= DEBUG_LOG_LEVEL) {
            Logger(DEBUG_PREFIX, args...);
        }
    }
};
