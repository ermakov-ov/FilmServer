#include "httplib.h"
#include "film_server.h"
#include "load_json.h"
#include <getopt.h>
#include "../../json/json_data.h"
#include "../../json/parser_json.h"
#include "../common/logger.h"

struct CommandLineData
{
    std::string config_file;
};
CommandLineData parseCommandLine(int argc, char* argv[]) ;

int main(int argc, char* argv[])
{
    try {
        CommandLineData command_line = parseCommandLine(argc, argv) ;
        film_server::ServerConfig server_config;

        film_server::loadDataFromConfig(command_line.config_file, server_config) ;
        film_server::initDailyLogs(server_config.log_path) ;
        logInfo("Start FilmServer application.") ;

        FilmSharedPtr film_db = std::make_shared<FilmDb>();

        film_server::loadDataFromJson(film_db, server_config.film_path, server_config.actors_path);

        film_server::FilmServer svr(server_config, film_db);

        svr.run();
    }
    catch (parser::ParseError &e) {
        const std::string msg = std::string("Parser error :") + e.what();
        std::cerr << msg << std::endl;
        logError(msg) ;
    }
    catch (std::invalid_argument &e) {
        const std::string msg = std::string("Command line error :") + e.what();
        //std::cerr << msg << std::endl;
        logError(msg) ;
    }
    catch (std::runtime_error &e) {
        const std::string msg = std::string("Runtime error :") + e.what();
        std::cerr << msg << std::endl;
        logError(msg) ;
    }
    catch (std::exception& e) {
        const std::string msg = std::string("Unknown exception :") + e.what();
        std::cerr << msg << std::endl;
        logError(msg) ;
    }


    return 0;
}

CommandLineData parseCommandLine(int argc, char* argv[])
{
    CommandLineData cfg;

    static struct option long_options[] = {
        {"config",   required_argument, nullptr, 'c'},
        {"help",     no_argument,       nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:p:vh", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'c':
            cfg.config_file = optarg;
            break;
        case 'h':
            std::cout << "Usage: film-server [--config FILE] [--port PORT] [--verbose]\n"
                      << "  -c, --config   Path to config file\n"
                      << "  -h, --help     Show this help message\n";
            throw std::runtime_error("help requested");
        default:
            throw std::invalid_argument("Unknown option");
        }
    }

    return cfg;
}