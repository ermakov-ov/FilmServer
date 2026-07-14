//
// Created by eow on 07/07/2026.
//

#ifndef JSONPARSER_FILM_H
#define JSONPARSER_FILM_H
#include <string>
#include <vector>

struct Film {
    int                      m_id;
    std::string              m_title;
    int                      m_releaseYear;
    std::string              m_description;

    std::vector<std::string> m_genres;
    std::vector<int>         m_actorIds;
    int                      m_directorId;
};
#endif //JSONPARSER_FILM_H
