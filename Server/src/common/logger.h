#ifndef HORSELINE_LOGGER_H
#define HORSELINE_LOGGER_H

#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    static void log(LogLevel level, const std::string& message)
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S") << " ["
           << levelToString(level) << "] " << message << "\n";

        std::cerr << ss.str();
    }

private:
    static std::string levelToString(LogLevel level)
    {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }
};

// Удобные inline-функции, чтобы писать просто logInfo(...)
inline void logDebug(const std::string& msg) { Logger::log(LogLevel::DEBUG, msg); }
inline void logInfo(const std::string& msg)  { Logger::log(LogLevel::INFO, msg); }
inline void logWarn(const std::string& msg)  { Logger::log(LogLevel::WARN, msg); }
inline void logError(const std::string& msg) { Logger::log(LogLevel::ERROR, msg); }



#endif // HORSELINE_LOGGER_H
