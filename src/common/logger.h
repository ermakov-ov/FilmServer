#ifndef HORSELINE_LOGGER_H
#define HORSELINE_LOGGER_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    // Устанавливаем поток вывода (по умолчанию уже стоит cerr в .cpp)
    static void setOutput(std::ostream* out)
    {
        output_stream_ = (out ? out : &std::cerr);
    }

    static void log(LogLevel level, const std::string& message)
    {
        // Если вдруг забыли инициализировать (хотя в .cpp мы это делаем всегда)
        if (!output_stream_) {
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%d-%m-%Y %H:%M:%S")
           << " [" << levelToString(level) << "] "
           << message << "\n";

        *output_stream_ << ss.str();
        output_stream_->flush();
        if (output_stream_ != &std::cout) {
            std::cout<<ss.str();
            std::cout.flush();
        }
    }

private:
    static std::string levelToString(LogLevel level) {
        switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }

    // ТОЛЬКО объявление! Определение будет в logger.cpp
    static std::ostream* output_stream_;
};

// Inline-функции для удобного вызова
inline void logDebug(const std::string& msg) { Logger::log(LogLevel::DEBUG, msg); }
inline void logInfo(const std::string& msg)  { Logger::log(LogLevel::INFO, msg); }
inline void logWarn(const std::string& msg)  { Logger::log(LogLevel::WARN, msg); }
inline void logError(const std::string& msg) { Logger::log(LogLevel::ERROR, msg); }

#endif
