#ifndef SERVER_PARSER_NEW_H
#define SERVER_PARSER_NEW_H

#include "lexer.h"
#include "json_data.h"
#include <string>

namespace parser
{

class ParseError : public std::runtime_error
{
public:
    ParseError(const std::string& msg, std::size_t pos)
        : std::runtime_error(msg)
        , m_pos(pos)
    {
    }

    std::size_t position() const
    {
        return m_pos;
    }

private:
    std::size_t m_pos;
};

class ParserJson
{
public:
    explicit ParserJson(StreamBuffer stream);

    json_data::JsonValuePtr parse();

private:

    json_data::JsonValuePtr parseObject();
    json_data::JsonValuePtr parseArray();
    json_data::JsonValuePtr parseValue();

    StreamBuffer m_stream;      // Владеет данными (буфером)
    LexerString  m_lexer;        // Не владеет, просто смотрит на поток

};

} // namespace parser

#endif //SERVER_PARSER_NEW_H
