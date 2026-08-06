#include "load_config.h"

#include <filesystem>
#include <sstream>
#include <iostream>
#include <fstream>

#include "../json/parser_json.h"
#include "../common/common.h"
#include "../common/logger.h"

namespace fs = std::filesystem;

namespace player_config {
void loadDataFromConfig(const std::string& configPath, player_config::PlayerConfig &player_config)
{
    auto jsonStr = common::readFile(configPath);
    parser::ParserJson parser(std::move(jsonStr));

    auto parsedRoot = parser.parse();
    const json_data::JsonValue* root = parsedRoot.get();

    if (!root || !root->isObject()) {
        logError("Couldn't find right structure for config file") ;
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    logInfo(root->toString()) ;

    const json_data::JsonObject* obj = dynamic_cast<const json_data::JsonObject*>(root);
    const json_data::JsonObject* log_path = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kPlayerLogging)));

    if ( !log_path || !log_path->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    //-------- log
    {
        auto log_dir= log_path->find(std::string(JsonKeys::kPlayerLogDir)) ;

        if ( !log_dir || !log_dir->isString()) {
            throw std::runtime_error("Couldn't find right structure for config file");
        }
        player_config.log_path = log_dir->asString();
    }

    //------------ video
    {
        const json_data::JsonObject* videoData = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kPlayerVideo)));
        if ( videoData) {

            auto video_patch = videoData->find(std::string(JsonKeys::kPlayerVideoPath)) ;

            if ( video_patch && !video_patch->isString()) {
                throw std::runtime_error("Couldn't find right structure for config file");
            }
            player_config.video_path = video_patch->asString();
        }
    }
    //-------------  connection
    {
        const json_data::JsonObject* connectionData = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kPlayerConnection)));
        if ( connectionData) {

            auto sddress_jsn = connectionData->find(std::string(JsonKeys::kPlayerConnectionAddress)) ;
            auto port_jsn = connectionData->find(std::string(JsonKeys::kPlayerConnectionPort)) ;

            if ( sddress_jsn && !sddress_jsn->isString() && port_jsn && !port_jsn->isNumber()) {
                throw std::runtime_error("Couldn't find right structure for config file");
            }
            player_config.address = sddress_jsn->asString();
            player_config.port = port_jsn->asNumber();
        }
    }
}
bool initDailyLogs(const std::string& baseDir)
{
    try {
        std::string logDir = baseDir.empty() ? "." : baseDir;
        fs::path path(logDir);
        path /= "log";

        if (!fs::exists(path)) {
            fs::create_directories(path);
        }

        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);

        std::stringstream dateStream;
        dateStream << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d");
        std::string dateStr = dateStream.str();

        fs::path logFilePath = path;
        logFilePath /= ("player_" + dateStr + ".log");

        static std::ofstream logFile;
        logFile.open(logFilePath, std::ios::app);

        if (logFile.is_open()) {
            Logger::setOutput(&logFile);
            return true;
        } else {
            std::cerr << "Warning: Could not open log file at " << logFilePath << "\n";
            return false;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error initializing logs: " << e.what() << "\n";
        return false;
    }
}
}