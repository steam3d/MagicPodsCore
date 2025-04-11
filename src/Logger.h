#pragma once

#include "StringUtils.h"

#include <cstdio>
#include <string>
#include <ctime>
#include <mutex>
#include <memory>

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Off = 5
};

class ILoggerFormatter {
public:
    virtual std::string Format(LogLevel loggingLevel, const std::string& message) = 0;
    virtual ~ILoggerFormatter() = default;
};

class DefaultFormatter : public ILoggerFormatter {
private:
    inline static std::string LogLevelToString(LogLevel logLevel) {
        switch (logLevel) {
            case LogLevel::Trace: return "TR";
            case LogLevel::Debug: return "DE";
            case LogLevel::Info: return "IN";
            case LogLevel::Warn: return "WA";
            case LogLevel::Error: return "ER";
            default: return "__";
        }
    }
public:
    std::string Format(LogLevel loggingLevel, const std::string& message) override {
        return MagicPodsCore::StringUtils::Format("[%s] %s", LogLevelToString(loggingLevel).c_str(), message.c_str());
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
        if (static_cast<int>(loggingLevel) < static_cast<int>(_loggingLevel) || loggingLevel == LogLevel::Off)
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

    static void SetLoggingLevelForGlobalLogger(LogLevel loggingLevel) {
        GetGlobalInstance().SetLoggingLevel(loggingLevel);
    }

    void SetFormatterForGlobalLogger(std::unique_ptr<ILoggerFormatter> formatter) {
        GetGlobalInstance().SetFormatter(std::move(formatter));
    }
};
