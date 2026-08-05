#include "common.h"
#include <filesystem>
#include <fstream>



namespace common
{
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
namespace JsonKeys
{
    const char *kId          = "id";
    const char *kName        = "name";
    const char *kTitle       = "title";
    const char *kYear        = "releaseYear";
    const char *kDescription = "description";
    const char *kGenres      = "genres";
    const char *kActorId     = "actorIds";
    const char *kDirectorId  = "directorId";
    const char *kActors      = "actors";
    const char *kFilms       = "films";
    const char *kVideo       = "video";
    const char *kDb          = "db";
    const char *kPathes      = "pathes";
    const char *kPort        = "port";
    const char *kLogging     = "logging";
    const char *kConnection  = "connection";
    const char *kLogDir      = "log_dir";
};
bool isDoubleQuotes(const char ch)
{
    return ch == '"';
}
bool isSingleQuotes(const char ch)
{
    return ch == '\'';
}
bool isColon(const char ch)
{
    return ch == ':';
}
bool isOpenBracket(const char ch)
{
    return ch == '[';
}
bool isCloseBracket(const char ch)
{
    return ch == ']';
}
bool isPoint(const char ch)
{
    return ch == '.';
}
bool isSlash(const char ch)
{
    return ch == '\\';
}
bool isDigit(const char ch)
{
    return isxdigit(ch) != 0 ;
}
bool isChar(const char ch)
{
    return std::isalpha(ch);
}
bool isCharDigit(const char ch)
{
    return isChar(ch) == true || isDigit(ch) == true || ch == '.' || ch == 'e' || ch == 'E' || ch == '-' || ch == '+' ;
}
bool isBooleanValue(const std::string &str, bool &value)
{
    if (str == "true")
    {
        value = true ;
        return true ;
    }
    if (str == "false")
    {
        value = false ;
        return true ;

    }
    return false ;

}
bool isNullValue(const std::string &str)
{
    return str == "null" ;
}
bool convertToNumber(const std::string &str, double &num)
{
    char* endptr;
    num = strtod(str.c_str(), &endptr);
    return *endptr == '\0' ;
}
bool isEndStrTabChar(const char ch)
{
    return ch == '\n' || ch == '\t' || ch == '\r' ;
}

bool isEmptyChar(const char ch)
{
    return ch == ' ' || isEndStrTabChar(ch) == true ;
}

}