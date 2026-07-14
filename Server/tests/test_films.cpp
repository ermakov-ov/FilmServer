#include <gtest/gtest.h>
#include "../src/film_db/film_db.h"
#include "../src/apps/server/load_json.h"
#include <string>
#include <filesystem>

const std::string ACTORS_JSON = "./data/actors.json";
const std::string FILMS_JSON  = "./data/films.json";

TEST(FilmDbLoadTest, LoadAndFindFilm)
{
    FilmSharedPtr film_db = std::make_shared<FilmDb>();

    std::string deb = std::filesystem::current_path() ;

    EXPECT_NO_THROW(loadDataFromJson(film_db, FILMS_JSON, ACTORS_JSON));

    auto film = film_db->findFilmById(100);
    ASSERT_TRUE(film);
    EXPECT_EQ(film->m_title, "Forrest Gump");
    EXPECT_EQ(film->m_releaseYear, 1994);
}

TEST(FilmDbLoadTest, YearFilter)
{
    FilmSharedPtr film_db = std::make_shared<FilmDb>();
    EXPECT_NO_THROW(loadDataFromJson(film_db, FILMS_JSON, ACTORS_JSON));

    int count1994 = 0;
    film_db->visitFilmsByYear(1994, [&](const Film&) { ++count1994; });

    EXPECT_GE(count1994, 2);
}
