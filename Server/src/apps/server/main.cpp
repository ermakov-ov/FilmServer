#include "../../film_db/film_db.h"
#include "../../film_db/film_db_view.h"
#include "load_json.h"
#include <iostream>

int main()
{
    FilmDb db;

    try {
        loadDataFromJson(db, "actors.json", "films.json");
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
