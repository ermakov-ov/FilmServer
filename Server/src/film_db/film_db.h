#ifndef JSONPARSER_FILM_DB_H
#define JSONPARSER_FILM_DB_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <sstream>
#include <optional>

#include "actor.h"
#include "director.h"
#include "film.h"
#include "../common/common.h"


struct Genre {
    int id;
    std::string name;
};

struct FilmDbStat
{
    int db_total_films;
    int db_total_actors;
    int db_total_directors;
    int db_total_genres;
    int db_index_tokens_count;
    double db_avg_films_per_actor;
};


class FilmDb {
public:
    void addActor(Actor a);
    void addDirector(Director d);
    void addGenre(Genre g);
    void addFilm(Film f);

    void loadDbFromJson(const std::string& filmsPath,
                          const std::string& actorsPath,
                          const std::string& directorsPath,
                          const std::string& genresPath);



    // Поиск с опциональными фильтрами (пустая строка = без фильтра)
    std::vector<Film> searchFilms(const std::string& titlePart,
                                  const std::string& actorName,
                                  const std::string& directorName,
                                  const std::string& genreName) const;
    size_t numberFilms() const { return m_filmsById.size(); }
    size_t numberActors() const { return m_actorsById.size(); }
    size_t numberDirectors() const { return m_directorsById.size(); }
    size_t numberGenres() const { return m_genresById.size(); }

    std::optional<Actor> getActorById(int id) const;
    std::optional<Director> getDirectorById(int id) const;
    std::optional<Genre> getGenreById(int id) const;
    void convertToFilmsAnswer(const std::vector<Film>& movies, std::vector<common::FilmAnswer>& result);
    std::string toLowerCopy(std::string s) const ;
    std::vector<std::string_view> tokenize(const std::string& s) const;
    std::optional<FilmDbStat> getFilmDbStat() const;

private:
    std::unordered_map<int, Film> m_filmsById;
    std::unordered_map<int, Actor> m_actorsById;
    std::unordered_map<int, Director> m_directorsById;
    std::unordered_map<int, Genre> m_genresById;

    // Индексы: нормализованное имя/токен -> список ID фильмов
    std::unordered_map<std::string, std::vector<int>> m_titleTokens;
    std::unordered_map<std::string, std::vector<int>> m_actorIndex;
    std::unordered_map<std::string, std::vector<int>> m_directorIndex;
    std::unordered_map<std::string, std::vector<int>> m_genreIndex;

    // Вспомогательный метод для пересечения отсортированных векторов
    static void intersectIds(const std::vector<int>& a,
                             const std::vector<int>& b,
                             std::vector<int>& out);
    void buildIndexes();
    std::string createFilmsAnswer(const std::vector<Film>& movies);
    std::optional<Genre> getGenreByName(const std::string &name) const;

    template<typename FieldChecker> std::vector<int> searchBySubstring(const std::string&,
                            const std::unordered_map<std::string, std::vector<int>>& ,
                            FieldChecker checkFieldContains) const;

    std::vector<int> searchTitlesBySubstring(const std::string& q) const;
    std::vector<int> searchActorsBySubstring(const std::string& q) const;
    std::vector<int> searchGenresBySubstring(const std::string& q) const;
    std::vector<int> searchDirectorsBySubstring(const std::string& q) const;

};
using FilmSharedPtr = std::shared_ptr<FilmDb>;

#endif //JSONPARSER_FILM_DB_H
