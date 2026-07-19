#ifndef JSONPARSER_FILM_DB_H
#define JSONPARSER_FILM_DB_H

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>

#include "film.h"
#include "actor.h"
#include "director.h"

class FilmDb {
public:
    void addActor(Actor a);
    void addDirector(Director d);
    void addFilm(Film f);

    std::optional<Actor> findActorById(int id) const;
    std::optional<Director> findDirectorById(int id) const;
    std::optional<Film> findFilmById(int id) const;
    std::optional<Film> findFilmByTitle(const std::string& title) const;
    void getAllFilms(std::vector<Film> &films) const;

    template <typename Func> void visitFilmsByYear(int year, Func f) const;
    template <typename Func> void visitFilmsByActor(int actorId, Func f) const;
    template <typename Func> void visitFilmsByDirector(int directorId, Func f) const;
    size_t numberFilms() const { return m_filmsById.size(); }
    size_t numberActors() const { return m_actorsById.size(); }

private:
    std::map<int, Film>                       m_filmsById;
    std::map<int, Actor>                      m_actorsById;
    std::map<int, Director>                   m_directorsById;

    std::unordered_map<std::string, int>      m_filmByTitle;
    std::multimap<int, int>                   m_filmsByYear;
    std::unordered_map<int, int>              m_filmByDirectorId;
    std::unordered_map<int, std::vector<int>> m_filmsByActor;
};

using FilmSharedPtr = std::shared_ptr<FilmDb>;

template <typename Func> void FilmDb::visitFilmsByYear(int year, Func f) const
{
    auto range = m_filmsByYear.equal_range(year);
    for (auto it = range.first; it != range.second; ++it) {
        int id = it->second;
        auto itFilm = m_filmsById.find(id);
        if (itFilm != m_filmsById.end()) {
            f(itFilm->second); // передаём const Film& внутри безопасного контекста
        }
    }
}

template <typename Func> void FilmDb::visitFilmsByActor(int actorId, Func f) const
{
    auto it = m_filmsByActor.find(actorId);
    if (it == m_filmsByActor.end()) return;

    const auto& ids = it->second;
    for (int id : ids) {
        auto itFilm = m_filmsById.find(id);
        if (itFilm != m_filmsById.end()) {
            f(itFilm->second);
        }
    }
}

template <typename Func> void FilmDb::visitFilmsByDirector(int directorId, Func f) const
{
    auto it = m_filmByDirectorId.find(directorId);
    if (it == m_filmByDirectorId.end()) return;

    int filmId = it->second;
    auto itFilm = m_filmsById.find(filmId);
    if (itFilm != m_filmsById.end()) {
        f(itFilm->second);
    }
}


#endif //JSONPARSER_FILM_DB_H
