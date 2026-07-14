#ifndef JSONPARSER_FILM_DB_VIEW_H
#define JSONPARSER_FILM_DB_VIEW_H

#include <memory>
#include "film_db.h"

class FilmDbSharedView {
public:
    explicit FilmDbSharedView(const FilmDb& db) noexcept : db_(db) {}

    std::shared_ptr<const Film> getFilmById(int id) const {
        auto opt = db_.findFilmById(id);
        if (!opt) return nullptr;

        return std::make_shared<const Film>(*opt);
    }
private:
    const FilmDb& db_;
};

#endif //JSONPARSER_FILM_DB_VIEW_H
