// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "StringUtils.h"

#include <cstdio>
#include <string>
#include <ctime>
#include <mutex>
#include <memory>

enum class LogLevel {
    Trace = 0,
    Debug = 10,
    Info = 20,
    Warn = 30,
    Error = 40,
    Critical = 50
};

static bool isValidLogLevel(int value){
    switch (static_cast<LogLevel>(value)) {
        case LogLevel::Trace:
        case LogLevel::Debug:
        case LogLevel::Info:
        case LogLevel::Warn:
        case LogLevel::Error:
        case LogLevel::Critical:
            return true;
        default:
            return false;
    }
}

class ILoggerFormatter {
public:
    virtual std::string Format(LogLevel loggingLevel, const std::string& message) = 0;
    virtual ~ILoggerFormatter() = default;
};

class DefaultFormatter : public ILoggerFormatter {
private:
    inline static std::string LogLevelToString(LogLevel logLevel) {
        switch (logLevel) {
            case LogLevel::Trace: return "TRC";
            case LogLevel::Debug: return "DBG";
            case LogLevel::Info: return "INF";
            case LogLevel::Warn: return "WRN";
            case LogLevel::Error: return "ERR";
            case LogLevel::Critical: return "CRT";
            default: return "__";
        }
    }
public:
    std::string Format(LogLevel loggingLevel, const std::string& message) override {
        return MagicPodsCore::StringUtils::Format("%s %s", LogLevelToString(loggingLevel).c_str(), message.c_str());
    }
};

class Logger {
private:
    LogLevel _loggingLevel{};
    std::mutex _mutex{};
    std::unique_ptr<ILoggerFormatter> _formatter{};

public:
    explicit Logger(LogLevel loggingLevel, std::unique_ptr<ILoggerFormatter> formatter) : _loggingLevel{loggingLevel}, _formatter{std::move(formatter)} {}

private:
    static Logger& GetGlobalInstance()
    {
        static Logger instance{LogLevel::Debug, std::make_unique<DefaultFormatter>()};
        return instance;
    }

public:
    template <typename... Args>
    void Log(LogLevel loggingLevel, const char* format, const Args&... args) {
        std::lock_guard<std::mutex> lock{_mutex};
        if (static_cast<int>(loggingLevel) < static_cast<int>(_loggingLevel))
            return;
        printf("%s\n", _formatter->Format(loggingLevel, MagicPodsCore::StringUtils::Format(format, args...)).c_str());
    }

    void SetLoggingLevel(LogLevel loggingLevel) {
        std::lock_guard<std::mutex> lock{_mutex};
        _loggingLevel = loggingLevel;
    }

    void SetFormatter(std::unique_ptr<ILoggerFormatter> formatter) {
        std::lock_guard<std::mutex> lock{_mutex};
        _formatter = std::move(formatter);
    }

    template <typename... Args>
    static void Trace(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Trace, format, args...);
    }

    template <typename... Args>
    static void Debug(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Debug, format, args...);
    }

    template <typename... Args>
    static void Info(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Info, format, args...);
    }

    template <typename... Args>
    static void Warn(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Warn, format, args...);
    }

    template <typename... Args>
    static void Error(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Error, format, args...);
    }

    template <typename... Args>
    static void Critical(const char* format, const Args&... args) {
        GetGlobalInstance().Log(LogLevel::Critical, format, args...);
    }

    static void SetLoggingLevelForGlobalLogger(LogLevel loggingLevel) {
        GetGlobalInstance().SetLoggingLevel(loggingLevel);
        Critical("Set log level to %d", loggingLevel);
    }

    static void SetLoggingLevelForGlobalLogger(int loggingLevel) {
        if (isValidLogLevel(loggingLevel)){
            SetLoggingLevelForGlobalLogger(static_cast<LogLevel>(loggingLevel));
        }
    }

    void SetFormatterForGlobalLogger(std::unique_ptr<ILoggerFormatter> formatter) {
        GetGlobalInstance().SetFormatter(std::move(formatter));
    }
};
