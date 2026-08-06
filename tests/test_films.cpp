#include <gtest/gtest.h>
#include "../src/film_db/film_db.h"
#include "../src/apps/server/load_json.h"
#include <string>
#include <filesystem>

const std::string ACTORS_JSON = "./data/actors.json";
const std::string FILMS_JSON  = "./data/films.json";
const std::string DIRECTOR_JSON = "./data/directors.json";
const std::string GENRES_JSON  = "./data/genres.json";

TEST(FilmDbLoadTest, LoadAndFindFilm)
{
    FilmSharedPtr film_db = std::make_shared<FilmDb>();

    std::string deb = std::filesystem::current_path() ;

    EXPECT_NO_THROW(film_db->loadDbFromJson(FILMS_JSON, ACTORS_JSON, DIRECTOR_JSON, GENRES_JSON));

    auto film = film_db->getFilmById(100) ;
    ASSERT_TRUE(film);
    EXPECT_EQ(film.value().title(), "Forrest Gump");
    EXPECT_EQ(film.value().year(), 1994);
}

TEST(FilmDbLoadTest, SearchFilms)
{
    FilmSharedPtr film_db = std::make_shared<FilmDb>();
    EXPECT_NO_THROW(film_db->loadDbFromJson(FILMS_JSON, ACTORS_JSON, DIRECTOR_JSON, GENRES_JSON));

    int count1994 = 0;
    auto films_data = film_db->searchFilms("", "Chris Evans","", "");
    EXPECT_EQ(films_data.size(), 1);

    auto film = films_data[0];
    EXPECT_EQ(film.id(), 102);
}
