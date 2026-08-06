
#include <iostream>
#include <QApplication>
#include "main_window.h"
#include <getopt.h>
#include "version.h"

#include "load_config.h"


struct CommandLineData
{
    std::string config_file;
};
CommandLineData parseCommandLine(int argc, char* argv[]) ;

int main(int argc, char *argv[])
{
    CommandLineData command_line = parseCommandLine(argc, argv) ;
    player_config::PlayerConfig player_config;

    player_config::loadDataFromConfig(command_line.config_file, player_config) ;
    player_config::initDailyLogs(player_config.log_path) ;

    QApplication app(argc, argv);
    MainWindow w(player_config);

    w.setWindowTitle(QString::fromStdString(std::string(player_version::name_app))+ " " + QString::fromStdString(player_version::makeServerVersionString()) + "");
    w.show();
    return app.exec();

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