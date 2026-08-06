#include "response_builder.h"
#include "version.h"

#include "film_server.h"

namespace response {

json_data::JsonValuePtr stats_answer(film_server::FilmServerStat &film_server_stat, FilmDbStat &film_db_stat)
{

    json_data::JsonValuePtr obj = std::make_unique<json_data::JsonObject>();
    json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject *>(obj.get()) ;
    {
        json_data::JsonValuePtr service = std::make_unique<json_data::JsonObject>();
        json_data::JsonObject *service_ptr = static_cast<json_data::JsonObject *>(service.get()) ;

        json_data::JsonValuePtr jsn_name = std::make_unique<json_data::JsonString>(std::string(server_version::name_app));
        json_data::JsonValuePtr jsn_version = std::make_unique<json_data::JsonString>(server_version::makeServerVersionString());

        service_ptr->setMember("name", std::move(jsn_name));
        service_ptr->setMember("version", std::move(jsn_version));

        obj_ptr->setMember("service", std::move(service));
    }
    {
        json_data::JsonValuePtr data_db = std::make_unique<json_data::JsonObject>();
        json_data::JsonObject *data_db_ptr = static_cast<json_data::JsonObject *>(data_db.get()) ;
        
        json_data::JsonValuePtr jsn_total_films = std::make_unique<json_data::JsonNumber>(film_db_stat.db_total_films);
        json_data::JsonValuePtr jsn_total_actors = std::make_unique<json_data::JsonNumber>(film_db_stat.db_total_actors);
        json_data::JsonValuePtr jsn_total_directors = std::make_unique<json_data::JsonNumber>(film_db_stat.db_total_directors);
        json_data::JsonValuePtr jsn_total_genres = std::make_unique<json_data::JsonNumber>(film_db_stat.db_total_genres);
        json_data::JsonValuePtr jsn_index_tokens_count = std::make_unique<json_data::JsonNumber>(film_db_stat.db_index_tokens_count);
        json_data::JsonValuePtr jsn_avg_films_per_actor = std::make_unique<json_data::JsonNumber>(film_db_stat.db_avg_films_per_actor);
        
        data_db_ptr->setMember("total_films",         std::move(jsn_total_films));
        data_db_ptr->setMember("total_actors",        std::move(jsn_total_actors));
        data_db_ptr->setMember("total_directors",     std::move(jsn_total_directors));
        data_db_ptr->setMember("total_genres",        std::move(jsn_total_genres));
        data_db_ptr->setMember("index_tokens_count",  std::move(jsn_index_tokens_count));
        data_db_ptr->setMember("avg_films_per_actor", std::move(jsn_avg_films_per_actor));

        obj_ptr->setMember("data", std::move(data_db));
    }
    {
        json_data::JsonValuePtr perfomance = std::make_unique<json_data::JsonObject>();
        json_data::JsonObject *perfomance_ptr = static_cast<json_data::JsonObject *>(perfomance.get()) ;

        json_data::JsonValuePtr jsn_queue_length = std::make_unique<json_data::JsonNumber>(film_server_stat.pf_queue_length);
        json_data::JsonValuePtr jsn_active_workers = std::make_unique<json_data::JsonNumber>(film_server_stat.pf_active_workers);
        json_data::JsonValuePtr jsn_requests_total = std::make_unique<json_data::JsonNumber>(film_server_stat.pf_requests_total);
        json_data::JsonValuePtr jsn_avg_response_time_ms = std::make_unique<json_data::JsonNumber>(film_server_stat.pf_avg_response_time_ms);
        json_data::JsonValuePtr jsn_errors_total = std::make_unique<json_data::JsonNumber>(film_server_stat.pf_errors_total);

        perfomance_ptr->setMember("queue_length", std::move(jsn_queue_length));
        perfomance_ptr->setMember("active_workers", std::move(jsn_active_workers));
        perfomance_ptr->setMember("requests_total", std::move(jsn_requests_total));
        perfomance_ptr->setMember("avg_response_time_ms", std::move(jsn_avg_response_time_ms));
        perfomance_ptr->setMember("errors_total", std::move(jsn_errors_total));

        obj_ptr->setMember("performance", std::move(perfomance));
    }
    return obj;
}

json_data::JsonValuePtr actors_answer(size_t totalActors)
{
    json_data::JsonValuePtr obj = std::make_unique<json_data::JsonObject>();
    json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject *>(obj.get()) ;

    json_data::JsonValuePtr jsn_numActors = std::make_unique<json_data::JsonNumber>(totalActors);

    obj_ptr->setMember("total_actors", std::move(jsn_numActors));
    return create_base_answer(std::move(obj), 1, 1, 0, 0,"");
}

json_data::JsonValuePtr films_answer(const std::vector<common::FilmAnswer>& movies)
{
    json_data::JsonValuePtr answer = std::make_unique<json_data::JsonArray>();
    json_data::JsonArray *answer_ptr = static_cast<json_data::JsonArray *>(answer.get()) ;

    for (const auto& m : movies) {

        json_data::JsonValuePtr obj = std::make_unique<json_data::JsonObject>();
        json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject *>(obj.get()) ;
        {
            json_data::JsonValuePtr jsn_title = std::make_unique<json_data::JsonString>(m.m_title);
            json_data::JsonValuePtr jsn_jear = std::make_unique<json_data::JsonNumber>(m.m_releaseYear);
            json_data::JsonValuePtr jsn_fid = std::make_unique<json_data::JsonNumber>(m.m_filmId);

            if ( obj_ptr == nullptr || jsn_title == nullptr || jsn_jear == nullptr || jsn_fid == nullptr ) {
                continue ;

            }
            obj_ptr->setMember("film_id", std::move(jsn_fid));
            obj_ptr->setMember("title", std::move(jsn_title));
            obj_ptr->setMember("year", std::move(jsn_jear));
        }
        //------ Actors
        {
            json_data::JsonValuePtr arr_acters = std::make_unique<json_data::JsonArray>();
            json_data::JsonArray *arr_ptr = static_cast<json_data::JsonArray *>(arr_acters.get()) ;

            for (const auto& actor_name : m.m_actorNames) {

                json_data::JsonValuePtr jsn_actor_item = std::make_unique<json_data::JsonString>(actor_name);
                arr_ptr->pushBack(std::move(jsn_actor_item)) ;

            }
            obj_ptr->setMember("actors", std::move(arr_acters)) ;
        }
        //------ Directors
        {
            json_data::JsonValuePtr jsn_director_item = std::make_unique<json_data::JsonString>(m.m_directorNames);
            obj_ptr->setMember("director", std::move(jsn_director_item)) ;
        }
        //------ Genders
        {
            json_data::JsonValuePtr arr_gen = std::make_unique<json_data::JsonArray>();
            json_data::JsonArray *gen_ptr = static_cast<json_data::JsonArray *>(arr_gen.get()) ;

            for (const auto& genre_name : m.m_genreNames) {
                json_data::JsonValuePtr jsn_genre_item = std::make_unique<json_data::JsonString>(genre_name);
                gen_ptr->pushBack(std::move(jsn_genre_item)) ;
            }
            obj_ptr->setMember("genre", std::move(arr_gen)) ;
        }
        answer_ptr->pushBack(std::move(obj));

    }
    return create_base_answer(std::move(answer), movies.size(), movies.size(), 0, 0,"");;
}

json_data::JsonValuePtr error_answer(int code, const std::string& message)
{
    json_data::JsonValuePtr empty_arr = std::make_unique<json_data::JsonArray>();
    return  create_base_answer(std::move(empty_arr), 1, 1, 0, code,message);
}

json_data::JsonValuePtr create_base_answer(json_data::JsonValuePtr result,
    int total,
    int limit,
    int offset,
    int codeError, const std::string &err_message)
{
    json_data::JsonValuePtr full_answer = std::make_unique<json_data::JsonObject>();
    json_data::JsonObject *full_answer_ptr = static_cast<json_data::JsonObject *>(full_answer.get()) ;

    if ( result.get() == nullptr || full_answer.get() == nullptr ) {
        return nullptr ;
    }
    full_answer_ptr->setMember("results", std::move(result)) ;

    json_data::JsonValuePtr jsn_total = std::make_unique<json_data::JsonNumber>(total);
    json_data::JsonValuePtr jsn_limit = std::make_unique<json_data::JsonNumber>(limit);
    json_data::JsonValuePtr jsn_offset = std::make_unique<json_data::JsonNumber>(offset);

    if (jsn_total == nullptr || jsn_limit == nullptr || jsn_offset == nullptr) {
        return nullptr ;
    }
    full_answer_ptr->setMember("total", std::move(jsn_total)) ;
    full_answer_ptr->setMember("limit", std::move(jsn_limit)) ;
    full_answer_ptr->setMember("offset", std::move(jsn_offset)) ;

    json_data::JsonValuePtr obj_error = std::make_unique<json_data::JsonObject>();
    json_data::JsonObject *obj_error_ptr = static_cast<json_data::JsonObject *>(obj_error.get()) ;

    json_data::JsonValuePtr jsn_err_code = std::make_unique<json_data::JsonNumber>(codeError);
    json_data::JsonValuePtr jsn_err_text = std::make_unique<json_data::JsonString>(err_message);

    if (obj_error == nullptr || jsn_err_code == nullptr || jsn_err_text == nullptr) {
        return nullptr ;
    }
    obj_error_ptr->setMember("error_code", std::move(jsn_err_code)) ;
    obj_error_ptr->setMember("error_text", std::move(jsn_err_text)) ;
    full_answer_ptr->setMember("error", std::move(obj_error)) ;

    return full_answer ;
}


}



