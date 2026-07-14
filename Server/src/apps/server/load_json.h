#ifndef JSONPARSER_LOAD_JSON_H
#define JSONPARSER_LOAD_JSON_H

#include <string>
#include "../../film_db/film_db.h"

void loadDataFromJson(FilmSharedPtr db, const std::string& filmsPath, const std::string& actorsPath);
std::vector<Director> getDefaultDirectors();

#endif //JSONPARSER_LOAD_JSON_H
