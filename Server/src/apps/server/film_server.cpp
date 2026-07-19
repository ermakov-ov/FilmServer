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
, m_threadpool(std::thread::hardware_concurrency())
{
    setupRoutes();
}

void FilmServer::run()
{
    std::cout << "Starting FilmServer on port " << m_server_config.port << "\n";
    if (!m_http_server.listen("0.0.0.0", m_server_config.port)) {
        std::cerr << "Failed to start server\n";
    }
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
         auto fut = m_threadpool.submit([this, &res]() {
             getListFilms(res) ;
         });
         try {
             fut.get();
             res.status = 200 ;
         }
         catch (...) {
             logError("Unknown error for \"films\" request") ;
             res.set_content("Unknown error for request", "text/plain");
             res.status = 500;
         }

     });
}

void FilmServer::setupActorRoutes()
{
    m_http_server.Get("/actors",
             [this](const httplib::Request &req, httplib::Response &res) {

         logInfo(getRequestInfodata(req)) ;
         res.set_content(response::actors(m_db->numberActors()), "application/json");
    });
}

void FilmServer::setupVideoEndpoint()
{
    m_http_server.Get("/video", [this](const httplib::Request &req,
                              httplib::Response &res){

        logInfo(getRequestInfodata(req)) ;
        auto id = req.get_param_value("id");
        if (id.empty()) {
            res.status = 400;
            logError("Missing 'id' parameter");
            res.set_content("Missing 'id' parameter", "text/plain");
            return;
        }
        std::string path = m_server_config.video_path + "/" + id + ".mp4";
        FileStreamer probe(path);

        if (!probe.isValid()) {
            logError("File " + path + " does not exist.");
            res.status = 404;
            res.set_content("File not found", "text/plain");
            return;
        }
        res.set_header("Content-Type", "video/mp4");

        logInfo("Start sending " + path + "...") ;

        res.set_chunked_content_provider("video/mp4",
            [streamer = std::make_shared<FileStreamer>(path)](size_t /*offset*/, httplib::DataSink &sink) mutable -> bool {
                if (!streamer || !streamer->isValid()) {
                    logError("File does not exist.");
                    return false;
                }

                char buffer[1024 * 1024];
                auto count = streamer->readChunk(buffer, sizeof(buffer));

                if (count == 0) {
                    streamer.reset(); // освобождаем заранее
                    return false;     // конец потока
                }

                sink.write(buffer, count);
                return true;
            },
            [path](bool success) {
                if (success) {
                    logDebug("Video stream completed: " + path);
                }
                else {
                    logWarn("Video stream interrupted: " + path);
                }
            }
        );

        logInfo("Finish sending " + path + "...") ;
    });
}

void FilmServer::getListFilms(httplib::Response &res) const
{
    std::vector<Film> films;
    m_db->getAllFilms(films);

    res.set_content(response::films(films), "application/json");
}

std::string FilmServer::getRequestInfodata(const httplib::Request &req)
{
    std::string res("Method - ");
    res += req.method + ", target - " + req.target +", remote address - " + req.remote_addr   ;
    return res;
}
}
