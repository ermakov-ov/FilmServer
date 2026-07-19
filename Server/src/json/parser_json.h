#ifndef SERVER_PARSER_NEW_H
#define SERVER_PARSER_NEW_H

#include <string>

#include "lexer.h"
#include "json_data.h"

namespace parser
{

class ParseError : public std::runtime_error
{
public:
    ParseError(const std::string& msg, std::size_t pos) ;

    std::size_t position() const;

private:
    std::size_t m_pos;
};

class ParserJson
{
public:
    explicit ParserJson(StreamBuffer stream);
    explicit ParserJson(const std::string &data_str);

    json_data::JsonValuePtr parse();

private:

    json_data::JsonValuePtr parseObject();
    json_data::JsonValuePtr parseArray();
    json_data::JsonValuePtr parseValue();

    StreamBuffer m_stream;
    LexerString  m_lexer;

};

} // namespace parser

#endif //SERVER_PARSER_NEW_H
