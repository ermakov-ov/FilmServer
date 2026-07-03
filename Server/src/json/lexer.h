
#ifndef SERVER_LEXER_H
#define SERVER_LEXER_H

#include "stream.h"
#include "../common/common.h"
#include <string>
#include <stdexcept>

namespace parser
{

enum class TokenType
{
    TT_Unknown,
    TT_CurlyOpen,
    TT_CurlyClose,
    TT_BracketOpen,
    TT_BracketClose,
    TT_Colon,
    TT_Comma,
    TT_String,
    TT_Number,
    TT_Bool,
    TT_Null,
    TT_QuoteSingle
};

class LexerError : public std::runtime_error
{
public:
    LexerError(const std::string& msg, std::size_t pos)
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

class LexerString
{
public:
    explicit LexerString(StreamBuffer& stream);

    TokenType nextToken();
    TokenType peekToken();

    std::string readString();
    std::string readSymbols();

    void expect(TokenType expected);

private:
    void skipWhitespace();
    TokenType tokenizeChar(char c) ;
    char parseEscape(char c);

    StreamBuffer& m_stream;
};

} // namespace parser

#endif //SERVER_LEXER_H
