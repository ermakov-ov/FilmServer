#include "load_json.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <iostream>

#include "../../json/json_data.h"
#include "../../json/parser_json.h"
#include "../common/logger.h"
#include "../common/common.h"


namespace fs = std::filesystem;

namespace film_server {

void loadDataFromConfig(const std::string& configPath, film_server::ServerConfig &server_config)
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
    const json_data::JsonObject* dbValue      = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kDb)));

    if ( !dbValue || !dbValue->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    //------------ path
    const json_data::JsonObject* dbpathes      = dynamic_cast<const json_data::JsonObject*>(dbValue->find(std::string(JsonKeys::kPathes)));
    if ( dbpathes) {

        auto actor_patch = dbpathes->find(std::string(JsonKeys::kActors)) ;
        auto film_patch  = dbpathes->find(std::string(JsonKeys::kFilms)) ;
        auto video_patch = dbpathes->find(std::string(JsonKeys::kVideo)) ;

        auto directors_patch  = dbpathes->find(std::string(JsonKeys::kDirectors)) ;
        auto genres_patch = dbpathes->find(std::string(JsonKeys::kGenresConfig)) ;

        if ( !actor_patch || !film_patch || !video_patch || !directors_patch || !genres_patch
            || !actor_patch->isString() || !film_patch->isString() || !video_patch->isString()
            || !directors_patch->isString() || !genres_patch->isString()) {
            throw std::runtime_error("Couldn't find right structure for config file (field \"actor\" or \"films\")");
        }

        server_config.actors_path = actor_patch->asString();
        server_config.film_path = film_patch->asString();
        server_config.video_path = video_patch->asString();
        server_config.directors_path = directors_patch->asString();
        server_config.genres_path = genres_patch->asString();
    }
    //---------- connection
    const json_data::JsonObject* server_connection = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kConnection)));
    if ( !server_connection || !server_connection->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }

    auto port_connection= server_connection->find(std::string(JsonKeys::kPort)) ;

    if ( !port_connection || !port_connection->isNumber()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    server_config.port = port_connection->asNumber();
    //-------- log
    const json_data::JsonObject* log_path = dynamic_cast<const json_data::JsonObject*>(obj->find(std::string(JsonKeys::kLogging)));

    if ( !log_path || !log_path->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    auto log_dir= log_path->find(std::string(JsonKeys::kLogDir)) ;

    if ( !log_dir || !log_dir->isString()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    server_config.log_path = log_dir->asString();
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
        logFilePath /= ("server_" + dateStr + ".log");

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