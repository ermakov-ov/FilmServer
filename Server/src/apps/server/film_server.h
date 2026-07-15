#ifndef HORSELINE_FILM_SERVER_H
#define HORSELINE_FILM_SERVER_H

#include <memory>

#include "httplib.h"
#include "thread_pool.h"
#include "../film_db/film_db.h"
#include "load_json.h"

namespace film_server {
    class FilmServer {
    public:
        explicit FilmServer(const film_server::ServerConfig &server_config, std::shared_ptr<FilmDb> db);
        void run();

    private:
        void setupRoutes();
        void setupStatsRoutes();
        void setupFilmRoutes();
        void setupActorRoutes();
        void setupVideoEndpoint();

        std::string getRequestInfodata(const httplib::Request &req) ;

        film_server::ServerConfig m_server_config;
        httplib::Server           m_http_server;
        std::shared_ptr<FilmDb>   m_db;
        ThreadPool                m_threadpool;
    };
}
#endif // HORSELINE_FILM_SERVER_H
