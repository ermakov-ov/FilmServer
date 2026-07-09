//
// Created by eow on 08/07/2026.
//

#ifndef JSONPARSER_FILM_DB_VIEW_H
#define JSONPARSER_FILM_DB_VIEW_H
#include "film_db.h"
#include <memory>

// Показывает опыт работы с shared_ptr и понимание компромиссов.
// Не трогает ядро FilmDb, даёт отдельный API для сценариев совместного владения.
class FilmDbSharedView {
    const FilmDb& db_;

public:
    explicit FilmDbSharedView(const FilmDb& db) noexcept : db_(db) {}

    // Отдаёт shared_ptr: клиент может хранить сколько угодно, даже после смерти FilmDb
    std::shared_ptr<const Film> getFilmById(int id) const {
        auto opt = db_.findFilmById(id);
        if (!opt) return nullptr;
        // Создаём shared_ptr из копии. Это тот самый опыт: мы осознанно платим
        // за shared_ptr только там, где это действительно нужно.
        return std::make_shared<const Film>(*opt);
    }

    // Если хочешь показать ещё более продвинутый вариант — можно добавить кэш shared_ptr,
    // чтобы не создавать их каждый раз. Но и так уже видно, что ты понимаешь, где и зачем
    // использовать shared_ptr.
};

#endif //JSONPARSER_FILM_DB_VIEW_H
