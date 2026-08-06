#ifndef HORSELINE_LOAD_CONFIG_H
#define HORSELINE_LOAD_CONFIG_H

#include <string>


namespace player_config {
struct PlayerConfig
{
    std::string log_path    ;
    std::string video_path  ;

    std::string address  = "127.0.0.1";
    int         port     = 8080;
};


bool initDailyLogs(const std::string& baseDir);
void loadDataFromConfig(const std::string& configPath, player_config::PlayerConfig &player_config) ;

}

#endif // HORSELINE_LOAD_CONFIG_H
