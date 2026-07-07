//
// Created by eow on 07/07/2026.
//

#ifndef JSONPARSER_FILM_DB_H
#define JSONPARSER_FILM_DB_H

#include "film.h"
#include "actor.h"
#include "director.h"

#include <map>
#include <unordered_map>
#include <multimap>
#include <vector>

class FilmDb {
    // Деревья по ID: O(log N), упорядоченность, без инвалидации указателей на ключи
    std::map<int, Film> filmsById_;
    std::map<int, Actor> actorsById_;
    std::map<int, Director> directorsById_;

    // Вторичные индексы (хранят ID — безопасно при реаллокациях)
    std::unordered_map<std::string, int> filmByTitle_;      // title -> filmId
    std::multimap<int, int> filmsByYear_;                   // year -> [filmId]
    std::unordered_map<int, int> filmByDirectorId_;         // directorId -> filmId (быстрый поиск по режиссёру)
    std::unordered_map<int, std::vector<int>> filmsByActor_;// actorId -> [filmId]

public:
    void addActor(Actor a);
    void addDirector(Director d);
    void addFilm(Film f);

    const Actor* findActorById(int id) const;
    const Director* findDirectorById(int id) const;
    const Film* findFilmById(int id) const;
    const Film* findFilmByTitle(const std::string& title) const;

    std::vector<const Film*> findFilmsByYear(int year) const;
    std::vector<const Film*> findFilmsByActor(int actorId) const;
    std::vector<const Film*> findFilmsByDirector(int directorId) const;
};

#endif //JSONPARSER_FILM_DB_H
