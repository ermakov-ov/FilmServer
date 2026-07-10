#include "httplib.h"
#include <iostream>
// Подключаем свои модули — как у тебя разложено по папкам
#include "../../film_db/film_db.h"
#include "load_json.h"

int main() {
    // 1. Инициализируем базу и загружаем данные
    FilmDb db;
    // Если у тебя есть функция загрузки из JSON — используй её
    loadDataFromJson(db, "data/films.json", "data/actors.json");

    httplib::Server svr;

    // Эндпоинт "/" — оставляем как есть
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string json = R"({"status":"ok","service":"film-server"})";
        res.set_content(json, "application/json");
    });

    // Эндпоинт "/stats" — теперь с реальными данными
    svr.Get("/stats", [&db](const httplib::Request&, httplib::Response& res) {
        // Тут ты используешь свою логику — просто берём размер базы
        int count = static_cast<int>(db.numberFilms());

        // Вариант А: ручная склейка (быстро, чтобы проверить)
        std::string json = "{\"total_films\":" + std::to_string(count) + "}";

        // Вариант Б (если хочешь сразу показать свою json_lib):
        // auto json = MyJsonBuilder().add("total_films", count).build();

        res.set_content(json, "application/json");
    });

    std::cout << "Server started on port 8080\n";
    if (!svr.listen("0.0.0.0", 8080)) {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    return 0;
}


/*
#include "../../film_db/film_db.h"
#include "../../film_db/film_db_view.h"
#include "load_json.h"
#include <iostream>

int main()
{
    FilmDb db;

    try {
        loadDataFromJson(db, "./data/actors.json", "./data/films.json");
    }
    catch (const std::exception& e) {
        std::cerr << "Startup error: " << e.what() << "\n";
        return 1;
    }

    if (auto film = db.findFilmById(100)) {
        std::cout << "Loaded: " << film->title << "\n";
    } else {
        std::cout << "Film 100 not found\n";
    }

    std::cout << "Films from 1994:\n";
    db.visitFilmsByYear(1994, [](const Film& f) {
        std::cout << "- " << f.title << " (" << f.releaseYear << ")\n";
    });

    FilmDbSharedView view(db);
    auto sp = view.getFilmById(102);
    if (sp) {
        std::cout << "Shared view: " << sp->title << "\n";
    }

    return 0;
}
*/
