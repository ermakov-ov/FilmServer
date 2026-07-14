#ifndef HORSELINE_FILM_SERVER_H
#define HORSELINE_FILM_SERVER_H

#include <memory>

#include "httplib.h"
#include "thread_pool.h"
#include "../film_db/film_db.h"

namespace film_server {
    class FilmServer {
    public:
        explicit FilmServer(int port, std::shared_ptr<FilmDb> db);
        void run();

    private:
        void setupRoutes();
        void setupStatsRoutes();
        void setupFilmRoutes();
        void setupActorRoutes();
        void setupVideoEndpoint();

        httplib::Server         m_http_server;
        const int               m_port;
        std::shared_ptr<FilmDb> db_;
        ThreadPool              m_threadpool;
    };
}
#endif // HORSELINE_FILM_SERVER_H
