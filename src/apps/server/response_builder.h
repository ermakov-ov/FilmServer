#ifndef HORSELINE_RESPONSE_BUILDER_H
#define HORSELINE_RESPONSE_BUILDER_H

#include "../json/json_data.h"
#include "../common/common.h"
#include "../film_db/film_db.h"
#include "film_server.h"

namespace response {
    json_data::JsonValuePtr stats_answer(film_server::FilmServerStat &, FilmDbStat &) ;
    json_data::JsonValuePtr actors_answer(size_t totalActors);
    json_data::JsonValuePtr films_answer(const std::vector<common::FilmAnswer>& movies);
    json_data::JsonValuePtr error_answer(int code, const std::string& message);
    json_data::JsonValuePtr create_base_answer(json_data::JsonValuePtr, int, int, int, int, const std::string &);

};

#endif //HORSELINE_RESPONSE_BUILDER_H
