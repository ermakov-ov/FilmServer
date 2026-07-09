#include <gtest/gtest.h>
#include "../src/film_db/film_db.h"
#include "../src/apps/server/load_json.h"
#include <string>
#include <filesystem>

const std::string ACTORS_JSON = "./data/actors.json";
const std::string FILMS_JSON  = "./data/films.json";

TEST(FilmDbLoadTest, LoadAndFindFilm)
{
    FilmDb db;

    std::string deb = std::filesystem::current_path() ;
    // 1. Загружаем данные
    EXPECT_NO_THROW(loadDataFromJson(db, ACTORS_JSON, FILMS_JSON));

    // 2. Ищем конкретный фильм
    auto film = db.findFilmById(100);
    ASSERT_TRUE(film);
    EXPECT_EQ(film->title, "Forrest Gump");
    EXPECT_EQ(film->releaseYear, 1994);
}

TEST(FilmDbLoadTest, YearFilter) {
    FilmDb db;
    EXPECT_NO_THROW(loadDataFromJson(db, ACTORS_JSON, FILMS_JSON));

    int count1994 = 0;
    db.visitFilmsByYear(1994, [&](const Film&) { ++count1994; });

    EXPECT_GE(count1994, 2); // Forrest Gump и Shawshank точно есть
}
