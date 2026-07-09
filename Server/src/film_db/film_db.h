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
#include <vector>
#include <optional>


// TODO (индексы):
// Выбран подход "Вариант 2" (инкапсуляция в FilmDb).
// Индексы (byYear, byDirector) будут храниться приватно.
// Обновление индексов происходит ТОЛЬКО внутри addFilm/removeFilm.
// Наружу индексы не выставляются — только методы findByYear/findByDirector.


class FilmDb {
    // Хранение по ID: стабильные адреса элементов, без реаллокаций при вставке других
    std::map<int, Film> filmsById_;
    std::map<int, Actor> actorsById_;
    std::map<int, Director> directorsById_;

    // Вторичные индексы
    std::unordered_map<std::string, int> filmByTitle_;
    std::multimap<int, int> filmsByYear_;
    std::unordered_map<int, int> filmByDirectorId_;
    std::unordered_map<int, std::vector<int>> filmsByActor_;

public:
    void addActor(Actor a);
    void addDirector(Director d);
    void addFilm(Film f);

    // Безопасный поиск одиночных объектов: копия в optional
    std::optional<Actor> findActorById(int id) const;
    std::optional<Director> findDirectorById(int id) const;
    std::optional<Film> findFilmById(int id) const;
    std::optional<Film> findFilmByTitle(const std::string& title) const;

    // Visitor-методы для коллекций: без копирования, без указателей
    template <typename Func>
    void visitFilmsByYear(int year, Func f) const;

    template <typename Func>
    void visitFilmsByActor(int actorId, Func f) const;

    template <typename Func>
    void visitFilmsByDirector(int directorId, Func f) const;
};

// --- Реализация шаблонов прямо в заголовке (чтобы компилировалось без .cpp) ---
template <typename Func>
void FilmDb::visitFilmsByYear(int year, Func f) const {
    auto range = filmsByYear_.equal_range(year);
    for (auto it = range.first; it != range.second; ++it) {
        int id = it->second;
        auto itFilm = filmsById_.find(id);
        if (itFilm != filmsById_.end()) {
            f(itFilm->second); // передаём const Film& внутри безопасного контекста
        }
    }
}

template <typename Func>
void FilmDb::visitFilmsByActor(int actorId, Func f) const {
    auto it = filmsByActor_.find(actorId);
    if (it == filmsByActor_.end()) return;

    const auto& ids = it->second;
    for (int id : ids) {
        auto itFilm = filmsById_.find(id);
        if (itFilm != filmsById_.end()) {
            f(itFilm->second);
        }
    }
}

template <typename Func>
void FilmDb::visitFilmsByDirector(int directorId, Func f) const {
    auto it = filmByDirectorId_.find(directorId);
    if (it == filmByDirectorId_.end()) return;

    int filmId = it->second;
    auto itFilm = filmsById_.find(filmId);
    if (itFilm != filmsById_.end()) {
        f(itFilm->second);
    }
}


#endif //JSONPARSER_FILM_DB_H
