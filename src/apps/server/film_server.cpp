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
    setupStatsRoutesV1();
    setupVideoEndpointV1();
    setupSearchRoutesV1();
}

void FilmServer::setupStatsRoutesV1()
{
    m_http_server.Get("/api/v1/stats",
     [this](const httplib::Request &req, httplib::Response &res) {

         ++m_stat_counter.all_request;
         logInfo(getRequestInfoData(req)) ;

         auto film_stat = getFilmServerStatStat();
         auto film_db_stat = m_db->getFilmDbStat();

         json_data::JsonValuePtr response_jsn(std::move(response::stats_answer(film_stat.value(), film_db_stat.value()))) ;
         if (response_jsn.get() == nullptr) {
             ++m_stat_counter.errors_request;
             logError("Unknown error for \"films\" request") ;
             res.set_content("Unknown error for request", "text/plain");
             res.status = 500;
             return ;
         }
         res.set_content(std::move(response_jsn->toString()), "application/json");
     });
}

void FilmServer::setupSearchRoutesV1()
{
    m_http_server.Get("/api/v1/search",
             [this](const httplib::Request &req, httplib::Response &res) {

                 ++m_stat_counter.all_request;
                 logInfo(getRequestInfoData(req)) ;
                 std::string title = req.get_param_value("by_title");
                 std::string actorCsv = req.get_param_value("by_actor");
                 std::string director =  req.get_param_value("by_director");
                 std::string genre = req.get_param_value("by_genre");

                 auto results = m_db->searchFilms(title, actorCsv, director, genre);

                 if ( results.empty() ) {
                     res.set_content(std::move(response::error_answer(100, "No data found")->toString()), "application/json");
                     return ;
                 }
                 std::vector<common::FilmAnswer> answer_result;
                 m_db->convertToFilmsAnswer(results, answer_result) ;

                 json_data::JsonValuePtr response_jsn(std::move(response::films_answer(answer_result))) ;
                 if (response_jsn.get() == nullptr) {
                     ++m_stat_counter.errors_request;
                     logError("Unknown error for \"films\" request") ;
                     res.set_content("Unknown error for request", "text/plain");
                     res.status = 500;
                     return ;
                 }

                 res.set_content(std::move(response_jsn->toString()), "application/json");
    });
}

void FilmServer::setupVideoEndpointV1()
{
    m_http_server.Get("/api/v1/video", [this](const httplib::Request &req,
                              httplib::Response &res){
        ++m_stat_counter.all_request;

        logInfo(getRequestInfoData(req)) ;

        std::string idFilm = req.get_param_value("by_number");
        if (idFilm.empty()) {
            ++m_stat_counter.errors_request;
            res.status = 400;
            logError("Missing 'id' parameter");
            res.set_content("Missing 'id' parameter", "text/plain");
            return;
        }
        std::string path = m_server_config.video_path + "/" + idFilm + ".mp4";
        FileStreamer probe(path);

        if (!probe.isValid()) {
            ++m_stat_counter.errors_request;
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
                    logError("File does not exist or streamer invalid.");
                    return false;
                }

                char buffer[1024 * 1024]; // 1 MB
                auto count = streamer->readChunk(buffer, sizeof(buffer));

                if (count == 0) {
                    return false;
                }
                if (!sink.write(buffer, count)) {
                    logWarn("Client disconnected while streaming.");
                    return false;
                }

                return true;
            },
            [path](bool success) {
                if (success) {
                    logDebug("Video stream completed: " + path);
                }
                else {
                    logInfo("Video stream finished: " + path);
                }
            }
        );
        logInfo("Finish sending " + path + "...") ;
    });
}

std::string FilmServer::getRequestInfoData(const httplib::Request &req)
{
    std::string res("Method - ");
    res += req.method + ", target - " + req.target +", remote address - " + req.remote_addr   ;
    return res;
}

std::optional<FilmServerStat> FilmServer::getFilmServerStatStat() const
{
    FilmServerStat stat;

    stat.pf_active_workers = m_threadpool.numbers_threads();
    stat.pf_requests_total = m_stat_counter.all_request;
    stat.pf_avg_response_time_ms = 0.0;
    stat.pf_errors_total = m_stat_counter.errors_request;

    stat.pf_queue_length = m_threadpool.numbers_threads();
    stat.pf_avg_response_time_ms = 0;

    return stat;
}

}
