#include "film_db.h"
#include <algorithm>

void FilmDb::addActor(Actor a)
{
    m_actorsById.try_emplace(a.id(), a.id(), std::move(a.name()));
}

void FilmDb::addDirector(Director d)
{
    m_directorsById.try_emplace(d.id(), d.id(), std::move(d.name()));
}

void FilmDb::addFilm(Film f)
{
    m_filmsById[f.m_id] = std::move(f);

    m_filmByTitle[f.m_title] = f.m_id;
    m_filmsByYear.emplace(f.m_releaseYear, f.m_id);
    m_filmByDirectorId[f.m_directorId] = f.m_id;

    for (int actorId : f.m_actorIds) {
        m_filmsByActor[actorId].push_back(f.m_id);
    }
}

std::optional<Actor> FilmDb::findActorById(int id) const
{
    auto it = m_actorsById.find(id);
    if (it == m_actorsById.end()) return std::nullopt;
    return it->second;
}

std::optional<Director> FilmDb::findDirectorById(int id) const
{
    auto it = m_directorsById.find(id);
    if (it == m_directorsById.end()) return std::nullopt;
    return it->second;
}

std::optional<Film> FilmDb::findFilmById(int id) const
{
    auto it = m_filmsById.find(id);
    if (it == m_filmsById.end()) return std::nullopt;
    return it->second;
}

std::optional<Film> FilmDb::findFilmByTitle(const std::string& title) const
{
    auto it = m_filmByTitle.find(title);
    if (it == m_filmByTitle.end()) return std::nullopt;
    int id = it->second;
    auto itFilm = m_filmsById.find(id);
    if (itFilm == m_filmsById.end()) return std::nullopt;
    return itFilm->second;
}
void FilmDb::getAllFilms(std::vector<Film> &films) const
{
    films.clear();
    films.reserve(m_filmsById.size());

    for (const auto& [id, film] : m_filmsById) {
        films.emplace_back(film);
    }
}

