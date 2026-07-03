#include "parser_json.h"

namespace parser
{
    ParseError::ParseError(const std::string& msg, std::size_t pos)
    : std::runtime_error(msg)
    , m_pos(pos)
    {
    }

    std::size_t ParseError::position() const
    {
        return m_pos;
    }
    //--------------------------------------
    ParserJson::ParserJson(StreamBuffer stream)
        : m_stream(std::move(stream))
        , m_lexer(m_stream)
    {
    }

    json_data::JsonValuePtr ParserJson::parse()
    {
        TokenType t = m_lexer.peekToken();
        if (t == TokenType::TT_CurlyOpen)
        {
            return parseObject();
        }
        if (t == TokenType::TT_BracketOpen)
        {
            return parseArray();
        }

        throw ParseError("Expected JSON object or array at root level", m_stream.position());
    }

    json_data::JsonValuePtr ParserJson::parseObject()
    {
        TokenType token_type = m_lexer.peekToken();

        if (token_type != TokenType::TT_CurlyOpen)
        {
            throw ParseError("Expected '{'", m_stream.position());
        }
        auto obj = json_data::JsonFactory::makeObject();
        auto& objRef = dynamic_cast<json_data::JsonObject&>(*obj);

        do
        {
            token_type = m_lexer.nextToken() ;

            //--------- "_______" -----------
            if ( token_type != TokenType::TT_String)
            {
                throw ParseError("Expected '\"' ", m_stream.position());
            }

            std::string title_object = m_lexer.readString();
            //--------- : -----------------
            m_lexer.expect(TokenType::TT_Colon);
            //-------- right value -----------------
            m_lexer.nextToken();
            json_data::JsonValuePtr value = parseValue();
            objRef.setMember(title_object, std::move(value));
            token_type = m_lexer.nextToken();
            //-------- , -----------
            if ( token_type != TokenType::TT_Comma && token_type != TokenType::TT_CurlyClose)
            {
                throw ParseError("Expected ',' or '}'", m_stream.position());
            }
        }
        while (token_type != TokenType::TT_CurlyClose) ;

        return obj ;
    }

    json_data::JsonValuePtr ParserJson::parseArray()
    {
        TokenType token_type = m_lexer.peekToken();
        if (token_type != TokenType::TT_BracketOpen)
        {
            throw ParseError("Expected '['", m_stream.position());
        }

        auto arr = json_data::JsonFactory::makeArray();
        auto& arrRef = dynamic_cast<json_data::JsonArray&>(*arr);

        do
        {
            m_lexer.nextToken();
            json_data::JsonValuePtr value = parseValue();
            arrRef.pushBack(std::move(value));
            token_type = m_lexer.nextToken();

            if ( token_type != TokenType::TT_Comma && token_type != TokenType::TT_BracketClose)
            {
                throw ParseError("Expected ',' or ']'", m_stream.position());
            }
        }
        while (token_type != TokenType::TT_BracketClose) ;

        return arr ;
    }

    json_data::JsonValuePtr ParserJson::parseValue()
    {
        TokenType token_type = m_lexer.peekToken();

        switch (token_type)
        {
            case TokenType::TT_CurlyOpen:
                return parseObject();

            case TokenType::TT_BracketOpen:
                return parseArray();

            case TokenType::TT_String:
            {
                std::string s = m_lexer.readString();
                return json_data::JsonFactory::makeString(s);
            }

            case TokenType::TT_Number:
            case TokenType::TT_Bool:
            case TokenType::TT_Null:
            {
                std::string number_str = m_lexer.readSymbols();
                {
                    bool value = false ;
                    if ( parser_common::isBooleanValue(number_str, value)  == true)
                    {
                        return json_data::JsonFactory::makeBool(value) ;
                    }
                }
                {
                    if ( parser_common::isNullValue(number_str)  == true)
                    {
                        return json_data::JsonFactory::makeNull() ;
                    }
                }
                {
                    double num = 0.0 ;
                    if ( parser_common::convertToNumber(number_str, num) == true)
                    {
                        return json_data::JsonFactory::makeNumber(num) ;
                    }
                }
                throw ParseError("Error with parsing number", m_stream.position());
            }

            default:
            {
                throw ParseError("Unexpected token in value context", m_stream.position());
            }
        }
    }
};
