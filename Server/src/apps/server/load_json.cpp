#include "load_json.h"

#include <fstream>
#include <stdexcept>

#include "../../json/json_data.h"
#include "../../json/parser_json.h"

namespace {
std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

} // namespace

void loadDataFromJson(FilmSharedPtr db, const std::string& filmsPath, const std::string& actorsPath)
{
    {
        auto jsonStr = readFile(actorsPath);
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

            // Получаем поля через find
            const json_data::JsonValue* idVal = obj->find("id");
            const json_data::JsonValue* nameVal = obj->find("name");

            if (!idVal || !nameVal || !idVal->isNumber() || !nameVal->isString()) {
                continue; // или throw, если хочешь строгую валидацию
            }

            int id = static_cast<int>(idVal->asNumber());
            std::string name = nameVal->asString();

            db->addActor(Actor{id, std::move(name)});
        }
    }
    {
        auto directors = getDefaultDirectors();
        for (const auto& d : directors) {
            db->addDirector(d);
        }
    }
    {
        auto jsonStr = readFile(filmsPath);
        parser::ParserJson parser(std::move(jsonStr));

        auto parsedRoot = parser.parse(); // <-- твой парсер
        const json_data::JsonValue* root = parsedRoot.get();

        if (!root || !root->isArray()) {
            throw std::runtime_error("films.json must be an array");
        }

        const auto& arr = root->asArray();
        for (const auto& itemPtr : arr) {
            if (!itemPtr || !itemPtr->isObject()) continue;
            const auto* obj = dynamic_cast<json_data::JsonObject*>(itemPtr.get());

            const json_data::JsonValue* idVal       = obj->find("id");
            const json_data::JsonValue* titleVal     = obj->find("title");
            const json_data::JsonValue* yearVal      = obj->find("releaseYear");
            const json_data::JsonValue* descVal      = obj->find("description");
            const json_data::JsonValue* genresVal    = obj->find("genres");
            const json_data::JsonValue* actorsVal    = obj->find("actorIds");
            const json_data::JsonValue* directorVal  = obj->find("directorId");

            if (!idVal || !titleVal || !yearVal || !descVal ||
                !genresVal || !actorsVal || !directorVal ||
                !idVal->isNumber() || !titleVal->isString() ||
                !yearVal->isNumber() || !descVal->isString() ||
                !genresVal->isArray() || !actorsVal->isArray() ||
                !directorVal->isNumber()) {
                continue; // или throw
            }

            Film f;
            f.m_id = static_cast<int>(idVal->asNumber());
            f.m_title = titleVal->asString();
            f.m_releaseYear = static_cast<int>(yearVal->asNumber());
            f.m_description = descVal->asString();

            // genres
            const auto& genresArr = genresVal->asArray();
            for (const auto& gPtr : genresArr) {
                if (gPtr && gPtr->isString()) {
                    f.m_genres.push_back(gPtr->asString());
                }
            }

            // actorIds
            const auto& actorsArr = actorsVal->asArray();
            for (const auto& aPtr : actorsArr) {
                if (aPtr && aPtr->isNumber()) {
                    f.m_actorIds.push_back(static_cast<int>(aPtr->asNumber()));
                }
            }

            f.m_directorId = static_cast<int>(directorVal->asNumber());

            db->addFilm(std::move(f));
        }
    }
}
std::vector<Director> getDefaultDirectors()
{
    return {
        Director{10, "Robert Zemeckis"},
        Director{11, "Frank Darabont"},
        Director{12, "Christopher Nolan"},
        Director{13, "Paul Thomas Anderson"}
    };
}