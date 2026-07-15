#include "film_server.h"

#include <future>
#include <iostream>

#include "file_streamer.h"
#include "response_builder.h"
#include "../common/logger.h"

namespace film_server {
FilmServer::FilmServer(const film_server::ServerConfig &server_config, std::shared_ptr<FilmDb> db)
: m_server_config(server_config)
, m_db(std::move(db))
,
m_threadpool(std::thread::hardware_concurrency())
{
    setupRoutes();
}

void FilmServer::setupRoutes()
{
    setupStatsRoutes();
    setupFilmRoutes();
    setupActorRoutes();
    setupVideoEndpoint();
}

void FilmServer::setupStatsRoutes()
{
    m_http_server.Get("/stats",
             [this](const httplib::Request &req, httplib::Response &res) {

                 logInfo(getRequestInfodata(req)) ;
                 size_t total = m_db->numberFilms();

                 res.set_content(response::stats(total), "application/json");
             });
}

void FilmServer::setupFilmRoutes()
{
    m_http_server.Get("/films",
             [this](const httplib::Request &req, httplib::Response &res) {

                 logInfo(getRequestInfodata(req)) ;

                 std::vector<Film> films;
                 m_db->getAllFilms(films);

                 res.set_content(response::films(films), "application/json");
             });
}

void FilmServer::setupActorRoutes()
{
    m_http_server.Get("/actors",
             [this](const httplib::Request &req, httplib::Response &res) {
                 logInfo(getRequestInfodata(req)) ;
                 size_t count = 0;

                 res.set_content(response::actors(count), "application/json");
            });
}

void FilmServer::run()
{
    std::cout << "Starting FilmServer on port " << m_server_config.port << "\n";
    if (!m_http_server.listen("0.0.0.0", m_server_config.port)) {
        std::cerr << "Failed to start server\n";
        return;
    }
}
std::string FilmServer::getRequestInfodata(const httplib::Request &req)
{
    std::string res("Method - ");
    res += req.method + ", target - " + req.target +", remote address - " + req.remote_addr   ;
    return res;
}
void FilmServer::setupVideoEndpoint()
{
    m_http_server.Get("/video", [this](const httplib::Request &req,
                              httplib::Response &res){

        logInfo(getRequestInfodata(req)) ;
        auto id = req.get_param_value("id");
        if (id.empty()) {
            res.status = 400;
            res.set_content("Missing 'id' parameter", "text/plain");
            return;
        }

        res.set_header("Content-Type", "video/mp4");

        std::promise<void> done;
        auto future = done.get_future();

        m_threadpool.enqueue([id, &res, &done,this]() mutable {
            std::string path = m_server_config.video_path + "/" + id + ".mp4";
            FileStreamer streamer(path);

            if (!streamer.isValid()) {
                std::string err_message = "File " + path + " does not exist. Sent 404 code for answer";
                logError(err_message) ;
                res.status = 404;
                res.set_content("File not found", "text/plain");
                done.set_value();
                return;
            }

            logInfo("Start sending " + path + "...") ;

            char buffer[1024 * 1024];

            while (true) {
                std::size_t count = streamer.readChunk(buffer, sizeof(buffer));
                if (count == 0)
                    break;

                res.body.append(buffer, count);
            }
            done.set_value();
            logInfo("Finish sending " + path + "...") ;
        });
        future.wait();
    });
}
}
