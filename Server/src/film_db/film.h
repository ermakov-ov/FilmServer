//
// Created by eow on 07/07/2026.
//

#ifndef JSONPARSER_FILM_H
#define JSONPARSER_FILM_H
#include <string>
#include <vector>

struct Film {
    int id;
    std::string title;
    int releaseYear;
    std::string description;

    std::vector<std::string> genres;
    std::vector<int> actorIds;
    int directorId;
};
#endif //JSONPARSER_FILM_H
