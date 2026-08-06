#ifndef JSONPARSER_FILM_H
#define JSONPARSER_FILM_H

#include <string>
#include <vector>

class Film {
public:
    Film()
    :m_id(0)
    ,m_title("")
    ,m_releaseYear(0)
    ,m_description("")
    ,m_directorId(0)
    {}

    int id() const noexcept { return m_id; }
    void setId(int id) noexcept { m_id = id; }

    const std::string& title() const noexcept { return m_title; }
    void setTitle(const std::string& title) noexcept { m_title = title; }

    int year() const noexcept { return m_releaseYear; }
    void setYear(int year) noexcept { m_releaseYear = year; }

    const std::string& description() const noexcept { return m_description; }
    void setDescription(const std::string& description) noexcept { m_description = description; }

    int directorId() const noexcept { return m_directorId; }
    void setDirectorId(int id) noexcept { m_directorId = id; }

    const std::vector<int> &genres() const { return m_genres;}
    void addGenre(const int genre_id)
    {
        m_genres.push_back(genre_id);
    }

    const std::vector<int> &actors() const { return m_actorIds; }
    void addActor(const int actor_id)
    {
        m_actorIds.push_back(actor_id);
    }

private:
    int                      m_id;
    std::string              m_title;
    int                      m_releaseYear;
    std::string              m_description;

    std::vector<int>         m_genres;
    std::vector<int>         m_actorIds;
    int                      m_directorId;
};

#endif //JSONPARSER_FILM_H
