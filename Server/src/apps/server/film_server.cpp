#include "film_server.h"

#include <future>
#include <iostream>

#include "file_streamer.h"
#include "response_builder.h"

namespace film_server {
    FilmServer::FilmServer(int port, std::shared_ptr<FilmDb> db)
        : m_port(port), db_(std::move(db)),
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
                 [this](const httplib::Request &, httplib::Response &res) {
                     size_t total = db_->numberFilms();

                     res.set_content(response::stats(total), "application/json");
                 });
    }

    void FilmServer::setupFilmRoutes()
    {
        m_http_server.Get("/films",
                 [this](const httplib::Request &, httplib::Response &res) {
                     std::vector<Film> films;
                     db_->getAllFilms(films);

                     res.set_content(response::films(films), "application/json");
                 });
    }

    void FilmServer::setupActorRoutes()
    {
        m_http_server.Get("/actors",
                 [this](const httplib::Request &, httplib::Response &res) {
                     size_t count = 0;

                     res.set_content(response::actors(count), "application/json");
                 });
    }

    void FilmServer::run()
    {
        std::cout << "Starting FilmServer on port " << m_port << "\n";
        if (!m_http_server.listen("0.0.0.0", m_port)) {
            std::cerr << "Failed to start server\n";
            return;
        }
    }
    void FilmServer::setupVideoEndpoint()
    {
        m_http_server.Get("/video", [this](const httplib::Request &req,
                                  httplib::Response &res) {
            auto id = req.get_param_value("id");
            if (id.empty()) {
                res.status = 400;
                res.set_content("Missing 'id' parameter", "text/plain");
                return;
            }

            res.set_header("Content-Type", "video/mp4");

            std::promise<void> done;
            auto future = done.get_future();

            m_threadpool.enqueue([id, &res, &done]() mutable {
                // std::string path = "./data/videos/" + id + ".mp4";
                std::string path = "./data/videos/1001.mp4";
                FileStreamer streamer(path);

                if (!streamer.isValid()) {
                    res.status = 404;
                    res.set_content("File not found", "text/plain");
                    done.set_value();
                    return;
                }

                char buffer[1024 * 1024];

                while (true) {
                    std::size_t count = streamer.readChunk(buffer, sizeof(buffer));
                    if (count == 0)
                        break;

                    res.body.append(buffer, count);
                }
                done.set_value();
            });
            future.wait();
        });
    }
}
