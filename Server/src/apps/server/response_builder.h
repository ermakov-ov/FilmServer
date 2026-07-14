#ifndef HORSELINE_RESPONSE_BUILDER_H
#define HORSELINE_RESPONSE_BUILDER_H

#include <string>
#include "../json/json_data.h"

namespace response {

    inline std::string stats(size_t totalFilms)
    {
        json_data::JsonObject obj;
        json_data::JsonValuePtr jsn_totalFilms = std::make_unique<json_data::JsonNumber>(totalFilms);
        obj.setMember("total_films", std::move(jsn_totalFilms));
        return obj.toString();
    }

    inline std::string actors(size_t totalActors)
    {
        json_data::JsonObject obj;
        json_data::JsonValuePtr jsn_numActors = std::make_unique<json_data::JsonNumber>(totalActors);

        obj.setMember("total_actors", std::move(jsn_numActors));
        return obj.toString();
    }

    inline std::string films(const std::vector<Film>& movies)
    {
        json_data::JsonArray arr;
        for (const auto& m : movies) {

            json_data::JsonValuePtr obj = std::make_unique<json_data::JsonObject>();
            json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject *>(obj.get()) ;

            json_data::JsonValuePtr jsn_id = std::make_unique<json_data::JsonNumber>(m.m_id);
            json_data::JsonValuePtr jsn_title = std::make_unique<json_data::JsonString>(m.m_title);
            json_data::JsonValuePtr jsn_jear = std::make_unique<json_data::JsonNumber>(m.m_releaseYear);

            if ( obj_ptr == nullptr || jsn_id == nullptr || jsn_title == nullptr || jsn_jear == nullptr ) {
                continue ;

            }
            obj_ptr->setMember("id", std::move(jsn_id));
            obj_ptr->setMember("title", std::move(jsn_title));
            obj_ptr->setMember("year", std::move(jsn_jear));

            json_data::JsonValuePtr jsn_janres = std::make_unique<json_data::JsonArray>();
            json_data::JsonArray *janres = static_cast<json_data::JsonArray *>(jsn_janres.get()) ;
            for ( const auto tmp_genre : m.m_genres)
            {
                json_data::JsonValuePtr jsn_janre_item = std::make_unique<json_data::JsonString>(tmp_genre);

                janres->pushBack(std::move(jsn_janre_item)) ;
            }
            obj_ptr->setMember("genres", std::move(jsn_janres));
            arr.pushBack(std::move(obj));
        }
        return arr.toString();
    }

    inline std::string error(int code, const std::string& message)
    {
        json_data::JsonObject obj;
        json_data::JsonValuePtr jsn_numCode = std::make_unique<json_data::JsonNumber>(code);
        json_data::JsonValuePtr jsn_ErrMassage = std::make_unique<json_data::JsonString>(message);

        if (jsn_numCode == nullptr || jsn_ErrMassage == nullptr)
        {
            return "{\"error\" : \"unknown error\"}" ;
        }

        obj.setMember("error", std::move(jsn_ErrMassage));
        obj.setMember("code", std::move(jsn_numCode));
        return obj.toString();
    }
}

#endif //HORSELINE_RESPONSE_BUILDER_H
