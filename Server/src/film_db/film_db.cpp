#include "film_db.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "../json/json_data.h"
#include "../json/parser_json.h"
#include "../common/logger.h"


template std::vector<int> FilmDb::searchBySubstring(
    const std::string&,
    const std::unordered_map<std::string, std::vector<int>>&,
    bool (*)(const Film&, const std::string&)) const;

std::string FilmDb::toLowerCopy(std::string s) const
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
    {
        return std::tolower(c);
    });
    return s;
}

std::vector<std::string_view> FilmDb::tokenize(const std::string& s) const
{
    std::vector<std::string_view> tokens;
    size_t start = 0;
    while (start < s.size()) {
        while (start < s.size() && s[start] == ' ') ++start;
        if (start >= s.size()) break;
        size_t end = start;
        while (end < s.size() && s[end] != ' ') ++end;
        tokens.emplace_back(s.data() + start, end - start);
        start = end;
    }
    return tokens;
}

template<typename FieldChecker>
std::vector<int> FilmDb::searchBySubstring(
    const std::string& query,
    const std::unordered_map<std::string, std::vector<int>>& index,
    FieldChecker checkFieldContains) const
{
    if (query.empty()) return {};

    const std::string q = toLowerCopy(query);
    auto tokens = tokenize(q);
    if (tokens.empty()) return {};

    std::unordered_set<int> candidateIds;

    // 1. Быстрый фильтр по индексу (по целым токенам)
    for (const auto& t : tokens) {
        std::string key(t);
        auto it = index.find(key);
        if (it != index.end()) {
            for (int id : it->second) {
                candidateIds.insert(id);
            }
        }
    }

    // 2. Fallback: если индекс ничего не дал — линейный скан по всем фильмам
    if (candidateIds.empty()) {
        for (const auto& [id, film] : m_filmsById) {
            if (checkFieldContains(film, q)) {
                candidateIds.insert(id);
            }
        }
        if (candidateIds.empty()) return {};
    }

    // 3. Финальная проверка: все токены должны быть подстроками
    std::vector<int> result;
    result.reserve(candidateIds.size());

    for (int id : candidateIds) {
        const auto& film = m_filmsById.at(id);
        bool ok = true;
        for (const auto& t : tokens) {
            std::string tStr(t);
            if (!checkFieldContains(film, tStr)) {
                ok = false;
                break;
            }
        }
        if (ok) result.push_back(id);
    }

    return result;
}

// Поиск по названию
std::vector<int> FilmDb::searchTitlesBySubstring(const std::string& q) const
{
    return searchBySubstring(q, m_titleTokens,
        [this](const Film& f, const std::string& sub) {
            const std::string titleLower = toLowerCopy(f.title());
            return titleLower.find(sub) != std::string::npos;
        });
}

// Поиск по актёру
std::vector<int> FilmDb::searchActorsBySubstring(const std::string& q) const
{
    return searchBySubstring(q, m_actorIndex,
        [this](const Film& f, const std::string& sub) {
            for (int actorId : f.actors()) {
                auto it = m_actorsById.find(actorId);
                if (it == m_actorsById.end()) continue;
                const std::string nameLower = toLowerCopy(it->second.name()); // предполагаю, что у Actor есть поле .name
                if (nameLower.find(sub) != std::string::npos) return true;
            }
            return false;
        });
}

// Поиск по жанру
std::vector<int> FilmDb::searchGenresBySubstring(const std::string& q) const
{
    return searchBySubstring(q, m_genreIndex,
        [this](const Film& f, const std::string& sub) {
            for (int genreId : f.genres()) {
                auto it = m_genresById.find(genreId);
                if (it == m_genresById.end()) continue;
                const std::string nameLower = toLowerCopy(it->second.name);
                if (nameLower.find(sub) != std::string::npos) return true;
            }
            return false;
        });
}

// Поиск по режиссёру
std::vector<int> FilmDb::searchDirectorsBySubstring(const std::string& q) const
{
    return searchBySubstring(q, m_directorIndex,
        [this](const Film& f, const std::string& sub) {
            auto it = m_directorsById.find(f.directorId());
            if (it == m_directorsById.end()) return false;
            const std::string nameLower = toLowerCopy(it->second.name());
            return nameLower.find(sub) != std::string::npos;
        });
}

void FilmDb::addActor(Actor a)
{
    m_actorsById.insert_or_assign(a.id(), std::move(a));
}

void FilmDb::addDirector(Director d)
{
    m_directorsById.insert_or_assign(d.id(), std::move(d));
}

void FilmDb::addGenre(Genre g)
{
    m_genresById.insert_or_assign(g.id, std::move(g));
}

void FilmDb::addFilm(Film f)
{
    m_filmsById.insert_or_assign(f.id(), std::move(f));
}

std::optional<Actor> FilmDb::getActorById(int id) const
{
    auto actor_id = m_actorsById.find(id);

    if (actor_id != m_actorsById.end()) {
        return actor_id->second;
    }
    return std::nullopt;
}

std::optional<Director> FilmDb::getDirectorById(int id) const
{
    auto director_id = m_directorsById.find(id);

    if (director_id != m_directorsById.end()) {
        return director_id->second;
    }
    return std::nullopt;
}

std::optional<Genre> FilmDb::getGenreById(int id) const
{
    auto genre_id = m_genresById.find(id);

    if (genre_id != m_genresById.end()) {
        return genre_id->second;
    }
    return std::nullopt;
}

void FilmDb::loadDbFromJson(const std::string& filmsPath,
                          const std::string& actorsPath,
                          const std::string& directorsPath,
                          const std::string& genresPath)
{
    //------------- Actors
    {
        auto jsonStr = common::readFile(actorsPath);
        parser::ParserJson parser(jsonStr);

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("actors.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal = obj->find(std::string(JsonKeys::kId));
            const json_data::JsonValue* nameVal = obj->find(std::string(JsonKeys::kName));

            if (!idVal || !nameVal || !idVal->isNumber() || !nameVal->isString()) {
                continue;
            }

            int id = static_cast<int>(idVal->asNumber());
            std::string name = nameVal->asString();

            addActor(Actor{id, std::move(name)});
        }
    }
    //----------------- Directors
    {
        auto jsonStr = common::readFile(directorsPath);
        parser::ParserJson parser(jsonStr);

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("directors.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal = obj->find(std::string(JsonKeys::kId));
            const json_data::JsonValue* nameVal = obj->find(std::string(JsonKeys::kName));

            if (!idVal || !nameVal || !idVal->isNumber() || !nameVal->isString()) {
                continue;
            }

            int id = static_cast<int>(idVal->asNumber());
            std::string name = nameVal->asString();

            addDirector(Director{id, std::move(name)});
        }
    }
    //----------------- Genres
    {
        auto jsonStr = common::readFile(genresPath);
        parser::ParserJson parser(jsonStr);

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("genres.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal = obj->find(std::string(JsonKeys::kId));
            const json_data::JsonValue* nameVal = obj->find(std::string(JsonKeys::kName));

            if (!idVal || !nameVal || !idVal->isNumber() || !nameVal->isString()) {
                continue;
            }

            int id = static_cast<int>(idVal->asNumber());
            std::string name = nameVal->asString();

            addGenre(Genre{id, std::move(name)});
        }
    }
    //------------ Films
    {
        auto jsonStr = common::readFile(filmsPath);
        parser::ParserJson parser(std::move(jsonStr));

        auto parsedRoot = parser.parse();
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("films.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal       = obj->find(std::string(JsonKeys::kId));
            const json_data::JsonValue* titleVal     = obj->find(std::string(JsonKeys::kTitle));
            const json_data::JsonValue* yearVal      = obj->find(std::string(JsonKeys::kYear));
            const json_data::JsonValue* descVal      = obj->find(std::string(JsonKeys::kDescription));
            const json_data::JsonValue* genresVal    = obj->find(std::string(JsonKeys::kGenres));
            const json_data::JsonValue* actorsVal    = obj->find(std::string(JsonKeys::kActorId));
            const json_data::JsonValue* directorVal  = obj->find(std::string(JsonKeys::kDirectorId));

            if (!idVal || !titleVal || !yearVal || !descVal ||
                !genresVal || !actorsVal || !directorVal ||
                !idVal->isNumber() || !titleVal->isString() ||
                !yearVal->isNumber() || !descVal->isString() ||
                !genresVal->isArray() || !actorsVal->isArray() ||
                !directorVal->isNumber()) {
                continue; // или throw
                }

            Film f;
            f.setId(static_cast<int>(idVal->asNumber()));
            f.setTitle(titleVal->asString());
            f.setYear(static_cast<int>(yearVal->asNumber()));
            f.setDescription(descVal->asString());


            // actorIds
            const auto& actorsArr = actorsVal->asArray();
            for (const auto& aPtr : actorsArr) {
                if (aPtr && aPtr->isNumber()) {
                    f.addActor(static_cast<int>(aPtr->asNumber()));
                }
            }

            // genres
            const auto& genresArr = genresVal->asArray();
            for (const auto& gPtr : genresArr) {
                if (gPtr && gPtr->isString()) {
                    auto genre = getGenreByName(gPtr->asString());
                    if ( genre.has_value() ) {
                        f.addGenre(genre.value().id);
                    }
                }
            }

            f.setDirectorId(static_cast<int>(directorVal->asNumber()));

            addFilm(std::move(f));
        }
    }

    buildIndexes();
}

void FilmDb::buildIndexes()
{
    m_titleTokens.clear();
    m_actorIndex.clear();
    m_directorIndex.clear();
    m_genreIndex.clear();

    auto tokenizeTitle = [&](const std::string& title, int filmId) {
        std::string t = toLowerCopy(title);
        std::istringstream iss(t);
        std::string token;
        while (iss >> token) {
            m_titleTokens[token].push_back(filmId);
        }
    };

    for (const auto& [filmId, film] : m_filmsById) {
        tokenizeTitle(film.title(), filmId);

        for (int genreId : film.genres()) {
            auto it = m_genresById.find(genreId);
            if (it == m_genresById.end()) continue;

            std::string g = toLowerCopy(it->second.name);
            m_genreIndex[g].push_back(filmId);
        }

        for (int actorId : film.actors()) {
            auto it = m_actorsById.find(actorId);
            if (it == m_actorsById.end()) continue;
            std::string a = toLowerCopy(it->second.name());
            m_actorIndex[a].push_back(filmId);
        }

        auto dirIt = m_directorsById.find(film.directorId());
        if (dirIt != m_directorsById.end()) {
            std::string d = toLowerCopy(dirIt->second.name());
            m_directorIndex[d].push_back(filmId);
        }
    }

    auto sortMapVecs = [](auto& m) {
        for (auto& [_, ids] : m) {
            std::sort(ids.begin(), ids.end());
        }
    };
    sortMapVecs(m_titleTokens);
    sortMapVecs(m_actorIndex);
    sortMapVecs(m_directorIndex);
    sortMapVecs(m_genreIndex);
}

// Вспомогательное пересечение двух отсортированных векторов (O(N+M))
void FilmDb::intersectIds(const std::vector<int>& a,
                          const std::vector<int>& b,
                          std::vector<int>& out) {
    out.clear();
    std::set_intersection(a.begin(), a.end(),
                           b.begin(), b.end(),
                           std::back_inserter(out));
}

std::vector<Film> FilmDb::searchFilms(
    const std::string& titlePart,
    const std::string& actorName,
    const std::string& directorName,
    const std::string& genreName) const
{
    std::vector<int> titleCands;
    std::vector<int> actorCands;
    std::vector<int> directorCands;

    std::vector<int> genreCands;

    if (!titlePart.empty())
        titleCands = searchTitlesBySubstring(titlePart);
    if (!actorName.empty())
        actorCands = searchActorsBySubstring(actorName);
    if (!directorName.empty())
        directorCands = searchDirectorsBySubstring(directorName);
    if (!genreName.empty())
        genreCands = searchGenresBySubstring(genreName);

    // Если какой-то фильтр дал пустой результат — пересечение будет пустым
    if ((!titlePart.empty() && titleCands.empty()) ||
        (!actorName.empty() && actorCands.empty()) ||
        (!directorName.empty() && directorCands.empty()) ||
        (!genreName.empty() && genreCands.empty())) {
        return {};
    }
    if (titlePart.empty() && actorName.empty() && directorName.empty() && genreName.empty()) {
        return {};
    }

    // Собираем непустые векторы кандидатов
    std::vector<const std::vector<int>*> sets;
    if (!titleCands.empty()) sets.push_back(&titleCands);
    if (!actorCands.empty()) sets.push_back(&actorCands);
    if (!directorCands.empty()) sets.push_back(&directorCands);
    if (!genreCands.empty()) sets.push_back(&genreCands);

    // Сортируем по размеру: начинаем с самого маленького множества
    std::sort(sets.begin(), sets.end(),
        [](const std::vector<int>* a, const std::vector<int>* b) {
            return a->size() < b->size();
        });

    // Пересечение: берём самое маленькое множество и фильтруем по остальным
    std::unordered_set<int> resultIds(sets[0]->begin(), sets[0]->end());

    for (size_t i = 1; i < sets.size(); ++i) {
        std::unordered_set<int> next;
        next.reserve(std::min(resultIds.size(), sets[i]->size()));
        for (int id : resultIds) {
            if (std::find(sets[i]->begin(), sets[i]->end(), id) != sets[i]->end()) {
                next.insert(id);
            }
        }
        resultIds = std::move(next);
        if (resultIds.empty()) break;
    }

    // Превращаем ID в объекты Film
    std::vector<Film> result;
    result.reserve(resultIds.size());
    for (int id : resultIds) {
        auto it = m_filmsById.find(id);
        if (it != m_filmsById.end()) result.push_back(it->second);
    }

    // По желанию: сортировка по ID для стабильности
    std::sort(result.begin(), result.end(),
        [](const Film& a, const Film& b) { return a.id() < b.id(); });

    return result;
}

std::string FilmDb::createFilmsAnswer(const std::vector<Film>& movies)
{
    json_data::JsonArray arr;
    for (const auto& m : movies) {

        json_data::JsonValuePtr obj = std::make_unique<json_data::JsonObject>();
        json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject *>(obj.get()) ;
        {
            json_data::JsonValuePtr jsn_id = std::make_unique<json_data::JsonNumber>(m.id());
            json_data::JsonValuePtr jsn_title = std::make_unique<json_data::JsonString>(m.title());
            json_data::JsonValuePtr jsn_jear = std::make_unique<json_data::JsonNumber>(m.year());

            if ( obj_ptr == nullptr || jsn_id == nullptr || jsn_title == nullptr || jsn_jear == nullptr ) {
                continue ;
            }
            obj_ptr->setMember("title", std::move(jsn_title));
            obj_ptr->setMember("year", std::move(jsn_jear));
            obj_ptr->setMember("id", std::move(jsn_id));
        }
        //------ Actors
        {
            json_data::JsonValuePtr arr_acters = std::make_unique<json_data::JsonArray>();
            json_data::JsonArray *arr_ptr = static_cast<json_data::JsonArray *>(arr_acters.get()) ;

            for (const auto& actorId : m.actors()) {
                auto actorItem = getActorById(actorId) ;

                if ( actorItem.has_value() == true ) {
                    json_data::JsonValuePtr jsn_actor_item = std::make_unique<json_data::JsonString>(actorItem.value().name());
                    arr_ptr->pushBack(std::move(jsn_actor_item)) ;
                }
            }
            obj_ptr->setMember("actors", std::move(arr_acters)) ;
        }
        //------ Directors
        {
            auto directorItem = getDirectorById(m.directorId()) ;

            if ( directorItem.has_value() == true ) {
                json_data::JsonValuePtr jsn_director_item = std::make_unique<json_data::JsonString>(directorItem.value().name());
                obj_ptr->setMember("actors", std::move(jsn_director_item)) ;
            }
        }
        //------ Genders
        {
            json_data::JsonValuePtr arr_gen = std::make_unique<json_data::JsonArray>();
            json_data::JsonArray *gen_ptr = static_cast<json_data::JsonArray *>(arr_gen.get()) ;

            for (const auto& genreId : m.genres()) {
                auto genreItem = getGenreById(genreId) ;

                if ( genreItem.has_value() == true ) {
                    json_data::JsonValuePtr jsn_genre_item = std::make_unique<json_data::JsonString>(genreItem.value().name);
                    gen_ptr->pushBack(std::move(jsn_genre_item)) ;
                }
            }
            obj_ptr->setMember("genre", std::move(arr_gen)) ;
        }
        arr.pushBack(std::move(obj));
    }
    return arr.toString();
}
void FilmDb::convertToFilmsAnswer(const std::vector<Film>& movies, std::vector<common::FilmAnswer>& result)
{
    if (movies.size() == 0 )
        return;

    result.reserve(movies.size()) ;
    std::size_t offset = 0;

    for (const auto& m : movies) {
        common::FilmAnswer temp_result= {m.id(), m.title(), m.year(), m.description(), {}, {}, ""};

        temp_result.m_actorNames.reserve(m.actors().size());

        for (const auto& actorId : m.actors()) {
            auto actorItem = getActorById(actorId) ;

            if ( actorItem.has_value() == true ) {
                temp_result.m_actorNames.push_back(std::move(actorItem.value().name()));
            }
        }
        auto directorItem = getDirectorById(m.directorId()) ;
        if ( directorItem.has_value() == true ) {
            temp_result.m_directorNames = directorItem.value().name();
        }
        for (const auto& genreId : m.genres()) {
            auto genreItem = getGenreById(genreId) ;

            if ( genreItem.has_value() == true ) {
                temp_result.m_genreNames.push_back(genreItem.value().name);
            }
        }
        result.push_back(std::move(temp_result)) ;
    }
}
std::optional<Genre> FilmDb::getGenreByName(const std::string &name) const
{
    const auto it_search = find_if(m_genresById.begin(), m_genresById.end(),[&name](const auto& genre_data) {
        return genre_data.second.name == name;
    });
    if (it_search != m_genresById.end()) {
        return it_search->second;
    }
    return std::nullopt;
}

std::optional<FilmDbStat> FilmDb::getFilmDbStat() const
{
    FilmDbStat stat ;

    stat.db_total_films = numberFilms() ;
    stat.db_total_actors = numberActors() ;
    stat.db_total_directors = numberDirectors() ;
    stat.db_total_genres    = numberGenres() ;
    stat.db_index_tokens_count = m_titleTokens.size();
    stat.db_avg_films_per_actor = 0.0;

    if (numberActors() > 0) {
        stat.db_avg_films_per_actor = static_cast<double>(numberFilms() / numberActors());
    }


    return stat;
}