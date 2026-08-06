#ifndef JSONPARSER_LOAD_JSON_H
#define JSONPARSER_LOAD_JSON_H

#include <string>
#include "../../film_db/film_db.h"

namespace film_server {
struct ServerConfig
{
    int         port = 8080;
    std::string actors_path;
    std::string film_path;
    std::string video_path;
    std::string directors_path;
    std::string genres_path;

    std::string log_path;

};
void loadDataFromConfig(const std::string& configPath, ServerConfig &);
std::vector<Director> getDefaultDirectors();
bool initDailyLogs(const std::string& baseDir);
}


#endif //JSONPARSER_LOAD_JSON_H
