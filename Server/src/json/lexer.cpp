#include "lexer.h"

namespace parser
{
    LexerError::LexerError(const std::string& msg, std::size_t pos)
    : std::runtime_error(msg)
    , m_pos(pos)
    {
    }

    std::size_t LexerError::position() const
    {
        return m_pos;
    }

    LexerString::LexerString(StreamBuffer& stream)
    : m_stream(stream)
    {
    }

    TokenType LexerString::nextToken()
    {
        if (m_stream.isEof())
        {
            return TokenType::TT_Unknown;
        }
        m_stream.next();
        return peekToken() ;
    }

    TokenType LexerString::peekToken()
    {
        skipWhitespace();
        if (m_stream.isEof())
        {
            return TokenType::TT_Unknown;
        }
        char c = m_stream.peek();
        return tokenizeChar(c);
    }

    std::string LexerString::readString()
    {
        if (!parser_common::isDoubleQuotes(m_stream.peek()))
        {
            throw LexerError("Expected double quote", m_stream.position());
        }

        std::string result;
        while (m_stream.isEof() == false)
        {
            m_stream.next();
            char ch = m_stream.peek();
            if (parser_common::isDoubleQuotes(ch))
            {
                return result ;
            }

            if (ch == '\\')
            {
                m_stream.next();
                if (m_stream.isEof())
                {
                    throw LexerError("Unexpected end of stream in escape sequence", m_stream.position());
                }
                char escaped = m_stream.peek();
                result.push_back(parseEscape(escaped));
            }
            else
            {
                result.push_back(ch);
            }
        }

        throw LexerError("Unclosed string", m_stream.position());
    }
    std::string LexerString::readSymbols()
    {
        std::string ret_value;

        char ch = m_stream.peek() ;
        if (parser_common::isCharDigit(ch) == false)
        {
            throw LexerError("Invalid number format", m_stream.position() );
        }

        ret_value.push_back(ch);
        while ( m_stream.isEof() == false )
        {
            ch = m_stream.next();
            if ( parser_common::isCharDigit(ch) == true )
            {
                ret_value.push_back(ch);
            }
            else
            {
                m_stream.prev();
                return ret_value;
            }
        }
        throw LexerError("Trying to go outside the buffer", m_stream.position());
    }

    void LexerString::expect(TokenType expected)
    {
        TokenType t = nextToken();
        if (t != expected)
        {
            throw LexerError("Unexpected token", m_stream.position());
        }
    }

    void LexerString::skipWhitespace()
    {
        while (!m_stream.isEof())
        {
            char c = m_stream.peek();
            if (!parser_common::isEmptyChar(c))
            {
                break;
            }
            m_stream.next();
        }
    }

    TokenType LexerString::tokenizeChar(char c)
    {
        switch (c)
        {
            case '{':
                return TokenType::TT_CurlyOpen;
            case '}':
                return TokenType::TT_CurlyClose;
            case '[':
                return TokenType::TT_BracketOpen;
            case ']':
                return TokenType::TT_BracketClose;
            case ':':
                return TokenType::TT_Colon;
            case ',':
                return TokenType::TT_Comma;
            case '"':
                return TokenType::TT_String;
            case '\'':
                return TokenType::TT_QuoteSingle;

            default:
                if (parser_common::isCharDigit(c) || c == '-')
                {
                    return TokenType::TT_Number;
                }
                return TokenType::TT_Unknown;
        }
    }

    char LexerString::parseEscape(char c)
    {
        switch (c)
        {
            case '"':
                return '"';
            case '\\':
                return '\\';
            case '/':
                return '/';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 't':
                return '\t';
            default:
                return c;
        }
    }
};

