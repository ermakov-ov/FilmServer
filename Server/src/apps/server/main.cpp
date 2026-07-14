#include "httplib.h"
#include "film_server.h"
#include "../../film_db/film_db.h"
#include "load_json.h"

int main()
{
    FilmSharedPtr film_db = std::make_shared<FilmDb>();

    loadDataFromJson(film_db, "data/films.json", "data/actors.json");

    film_server::FilmServer svr(8080, film_db);

    svr.run();

    return 0;
}
