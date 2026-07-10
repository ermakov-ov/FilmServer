//
// Created by eow on 08/07/2026.
//

#ifndef JSONPARSER_LOAD_JSON_H
#define JSONPARSER_LOAD_JSON_H

#include "../../film_db/film_db.h"
#include <string>

// Предполагаем, что у тебя есть json_lib::parseFile(path) -> JsonValue
// и методы для доступа к полям: asInt, asString, asArray, asObject и т.д.
void loadDataFromJson(FilmDb& db, const std::string& filmsPath, const std::string& actorsPath);
std::vector<Director> getDefaultDirectors();

#endif //JSONPARSER_LOAD_JSON_H
