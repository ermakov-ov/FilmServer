//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_CONST_H
#define SERVER_CONST_H
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace common
{
    bool isDoubleQuotes(char );
    bool isSingleQuotes(char );
    bool isColon(char ch);
    bool isOpenBracket(char ch);
    bool isCloseBracket(char ch);
    bool isPoint(char ch);
    bool isSlash(char ch);
    bool isDigit(char ch);
    bool isChar(char ch);
    bool isCharDigit(char ch);
    bool isBooleanValue(const std::string &str, bool &value);
    bool isNullValue(const std::string &str);
    bool convertToNumber(const std::string &str, double &num);
    bool isEndStrTabChar(char ch);
    bool isEmptyChar(char ch);
    std::string readFile(const std::string& path);


struct FilmAnswer {
    int                      m_filmId;
    std::string              m_title;
    int                      m_releaseYear;
    std::string              m_description;
    std::vector<std::string> m_genreNames;
    std::vector<std::string> m_actorNames;
    std::string              m_directorNames;
};
std::string readFile(const std::string& path);
 
}
namespace JsonKeys {
    constexpr inline std::string_view kId          = "id";
    constexpr inline std::string_view kName        = "name";
    constexpr inline std::string_view kTitle       = "title";
    constexpr inline std::string_view kYear        = "releaseYear";
    constexpr inline std::string_view kDescription = "description";
    constexpr inline std::string_view kGenres      = "genres";
    constexpr inline std::string_view kActorId     = "actorIds";
    constexpr inline std::string_view kDirectorId  = "directorId";
    constexpr inline std::string_view kActors      = "actors";
    constexpr inline std::string_view kFilms       = "films";
    constexpr inline std::string_view kVideo       = "video";
    constexpr inline std::string_view kDirectors   = "directors";
    constexpr inline std::string_view kGenresConfig= "genres";
    constexpr inline std::string_view kDb          = "db";
    constexpr inline std::string_view kPathes      = "pathes";
    constexpr inline std::string_view kPort        = "port";
    constexpr inline std::string_view kLogging     = "logging";
    constexpr inline std::string_view kConnection  = "connection";
    constexpr inline std::string_view kLogDir      = "log_dir";
//------------------------ player
    constexpr inline std::string_view kPlayerLogging = "logging";
    constexpr inline std::string_view kPlayerLogDir  = "log_dir";
    constexpr inline std::string_view kPlayerVideo   = "video";
    constexpr inline std::string_view kPlayerVideoPath  = "path";
    constexpr inline std::string_view kPlayerConnection  = "connection";
    constexpr inline std::string_view kPlayerConnectionAddress  = "address";
    constexpr inline std::string_view kPlayerConnectionPort  = "port";

}


#endif //SERVER_CONST_H
