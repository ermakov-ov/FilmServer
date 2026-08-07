#ifndef HORSELINE_FILM_SERVER_H
#define HORSELINE_FILM_SERVER_H

#include <memory>

#include "httplib.h"
#include "thread_pool.h"
#include "../film_db/film_db.h"
#include "load_json.h"

namespace film_server {

    struct FilmServerStat
    {
        std::size_t pf_queue_length;
        std::size_t pf_active_workers;
        std::size_t pf_requests_total;
        double      pf_avg_response_time_ms;
        std::size_t pf_errors_total;
    };
    struct FilmServerStatCounter
    {
        std::size_t errors_request = 0;
        std::size_t all_request = 0;
    };
    class FilmServer {
    public:
        explicit FilmServer(const film_server::ServerConfig &server_config, std::shared_ptr<FilmDb> db);
        void run();

    private:
        void setupRoutes();
        void setupStatsRoutesV1();
        void setupVideoEndpointV1();
        void setupSearchRoutesV1();
        void setupDocsEndpointV1() ;

        std::optional<FilmServerStat> getFilmServerStatStat() const;

        std::string getRequestInfoData(const httplib::Request &req) ;

        film_server::ServerConfig m_server_config;
        httplib::Server           m_http_server;
        std::shared_ptr<FilmDb>   m_db;
        ThreadPool                m_threadpool;
        FilmServerStatCounter     m_stat_counter;
    };
}
#endif // HORSELINE_FILM_SERVER_H
