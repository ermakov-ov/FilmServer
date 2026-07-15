#include "load_json.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <iostream>

#include "../../json/json_data.h"
#include "../../json/parser_json.h"
#include "../common/logger.h"


namespace fs = std::filesystem;

namespace film_server {

std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

void loadDataFromJson(FilmSharedPtr db, const std::string& filmsPath, const std::string& actorsPath)
{
    {
        auto jsonStr = film_server::readFile(actorsPath);
        parser::ParserJson parser(jsonStr);

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("actors.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            // Получаем поля через find
            const json_data::JsonValue* idVal = obj->find("id");
            const json_data::JsonValue* nameVal = obj->find("name");

            if (!idVal || !nameVal || !idVal->isNumber() || !nameVal->isString()) {
                continue; // или throw, если хочешь строгую валидацию
            }

            int id = static_cast<int>(idVal->asNumber());
            std::string name = nameVal->asString();

            db->addActor(Actor{id, std::move(name)});
        }
    }
    {
        auto directors = film_server::getDefaultDirectors();
        for (const auto& d : directors) {
            db->addDirector(d);
        }
    }
    {
        auto jsonStr = film_server::readFile(filmsPath);
        parser::ParserJson parser(std::move(jsonStr));

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("films.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal       = obj->find("id");
            const json_data::JsonValue* titleVal     = obj->find("title");
            const json_data::JsonValue* yearVal      = obj->find("releaseYear");
            const json_data::JsonValue* descVal      = obj->find("description");
            const json_data::JsonValue* genresVal    = obj->find("genres");
            const json_data::JsonValue* actorsVal    = obj->find("actorIds");
            const json_data::JsonValue* directorVal  = obj->find("directorId");

            if (!idVal || !titleVal || !yearVal || !descVal ||
                !genresVal || !actorsVal || !directorVal ||
                !idVal->isNumber() || !titleVal->isString() ||
                !yearVal->isNumber() || !descVal->isString() ||
                !genresVal->isArray() || !actorsVal->isArray() ||
                !directorVal->isNumber()) {
                continue; // или throw
                }

            Film f;
            f.m_id = static_cast<int>(idVal->asNumber());
            f.m_title = titleVal->asString();
            f.m_releaseYear = static_cast<int>(yearVal->asNumber());
            f.m_description = descVal->asString();

            // genres
            const auto& genresArr = genresVal->asArray();
            for (const auto& gPtr : genresArr) {
                if (gPtr && gPtr->isString()) {
                    f.m_genres.push_back(gPtr->asString());
                }
            }

            // actorIds
            const auto& actorsArr = actorsVal->asArray();
            for (const auto& aPtr : actorsArr) {
                if (aPtr && aPtr->isNumber()) {
                    f.m_actorIds.push_back(static_cast<int>(aPtr->asNumber()));
                }
            }

            f.m_directorId = static_cast<int>(directorVal->asNumber());

            db->addFilm(std::move(f));
        }
    }
}
std::vector<Director> getDefaultDirectors()
{
    return {
        Director{10, "Robert Zemeckis"},
        Director{11, "Frank Darabont"},
        Director{12, "Christopher Nolan"},
        Director{13, "Paul Thomas Anderson"}
    };
}


void loadDataFromConfig(const std::string& configPath, film_server::ServerConfig &server_config)
{
    auto jsonStr = film_server::readFile(configPath);
    parser::ParserJson parser(std::move(jsonStr));

    auto parsedRoot = parser.parse();
    const json_data::JsonValue* root = parsedRoot.get();

    if (!root || !root->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }

    const json_data::JsonObject* obj = dynamic_cast<const json_data::JsonObject*>(root);
    const json_data::JsonObject* dbValue      = dynamic_cast<const json_data::JsonObject*>(obj->find("db"));

    if ( !dbValue || !dbValue->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    //------------ path
    const json_data::JsonObject* dbpathes      = dynamic_cast<const json_data::JsonObject*>(dbValue->find("pathes"));
    if ( dbpathes) {
        auto actor_patch = dbpathes->find("actors") ;
        auto film_patch = dbpathes->find("films") ;
        auto video_patch = dbpathes->find("video") ;

        if ( !actor_patch || !film_patch || !video_patch || !actor_patch->isString() || !film_patch->isString() || !video_patch->isString()) {
            throw std::runtime_error("Couldn't find right structure for config file (field \"actor\" or \"films\")");
        }

        server_config.actors_path = actor_patch->asString();
        server_config.film_path = film_patch->asString();
        server_config.video_path = video_patch->asString();
    }
    //---------- connection
    const json_data::JsonObject* server_connection = dynamic_cast<const json_data::JsonObject*>(obj->find("connection"));
    if ( !server_connection || !server_connection->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    auto port_connection= server_connection->find("port") ;

    if ( !port_connection || !port_connection->isNumber()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    server_config.port = port_connection->asNumber();
    //-------- log
    const json_data::JsonObject* log_path = dynamic_cast<const json_data::JsonObject*>(obj->find("logging"));

    if ( !log_path || !log_path->isObject()) {
        throw std::runtime_error("Couldn't find right structure for config file");
    }
    auto log_dir= log_path->find("log_dir") ;

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