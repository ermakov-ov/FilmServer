#include "film_db.h"

void FilmDb::addActor(Actor a)
{
    //actorsById_[a.id()] = std::move(a);
    actorsById_.try_emplace(a.id(), a.id(), std::move(a.name()));
}

void FilmDb::addDirector(Director d)
{
    //directorsById_[d.id()] = std::move(d);
    directorsById_.try_emplace(d.id(), d.id(), std::move(d.name()));
}

void FilmDb::addFilm(Film f)
{
    filmsById_[f.id] = std::move(f);

    filmByTitle_[f.title] = f.id;
    filmsByYear_.emplace(f.releaseYear, f.id);
    filmByDirectorId_[f.directorId] = f.id;

    for (int actorId : f.actorIds) {
        filmsByActor_[actorId].push_back(f.id);
    }
}

std::optional<Actor> FilmDb::findActorById(int id) const
{
    auto it = actorsById_.find(id);
    if (it == actorsById_.end()) return std::nullopt;
    return it->second; // копирование одного Actor — дёшево и безопасно
}

std::optional<Director> FilmDb::findDirectorById(int id) const
{
    auto it = directorsById_.find(id);
    if (it == directorsById_.end()) return std::nullopt;
    return it->second;
}

std::optional<Film> FilmDb::findFilmById(int id) const
{
    auto it = filmsById_.find(id);
    if (it == filmsById_.end()) return std::nullopt;
    return it->second;
}

std::optional<Film> FilmDb::findFilmByTitle(const std::string& title) const
{
    auto it = filmByTitle_.find(title);
    if (it == filmByTitle_.end()) return std::nullopt;
    int id = it->second;
    auto itFilm = filmsById_.find(id);
    if (itFilm == filmsById_.end()) return std::nullopt;
    return itFilm->second;
}

