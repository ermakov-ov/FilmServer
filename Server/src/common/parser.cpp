//
// Created by eow on 23/06/2026.
//
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "parser.h"
#include "const.h"

parser_const::DelimStrData delimStr = {
    {'"', parser_const::LexemFirstType::LFT_DoubleQuotes   },
    {'\'', parser_const::LexemFirstType::LFT_isSingleQuotes },
    {':', parser_const::LexemFirstType::LFT_isColon        },
    {'[', parser_const::LexemFirstType::LFT_isOpenBracket  },
    {']', parser_const::LexemFirstType::LFT_isCloseBracket },
    {',', parser_const::LexemFirstType::LFT_isComma        },
    {'{', parser_const::LexemFirstType::LFT_isCurlyOpenBracket },
    {'}', parser_const::LexemFirstType::LFT_isCurlyCloseBracket}
};

namespace parser
{
    BufferData::BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset)
    :m_data(std::move(ex_data))
    ,m_length(length)
    ,m_offset(offset)
    {
        ptr_for_debug = m_data.get();
    }

    BufferData::~BufferData()
    {

    }


    const char BufferData::nextBuffPosition()
    {
        if (m_length <= 0 )
        {
            throw EmptyBufferExp() ;
        }
        if ( m_offset + 1 >= m_length)
        {
            throw OutOfRangeExp() ;
        }

        return m_data[++m_offset] ;
    }
    const char BufferData::prevBuffPosition()
    {
        if (m_length <= 0 )
        {
            throw EmptyBufferExp() ;
        }
        if ( m_offset == 0)
        {
            throw OutOfRangeExp() ;
        }

        return m_data[--m_offset] ;
    }
    const char BufferData::currentPosition() const
    {
        if (m_length <= 0 )
        {
            throw EmptyBufferExp();
        }
        return m_data[m_offset] ;
    }


    bool BufferData::isEndStrTabChar(const char ch) const
    {
        return ch == '\n' || ch == '\t' || ch == '\r' ;
    }

    bool BufferData::isEmptyChar(const char ch) const
    {
        return ch == ' ' || isEndStrTabChar(ch) == true ;
    }

    parser_const::LexemFirstType BufferData::stepToNextChar()
    {
        while (1)
        {
            nextBuffPosition();
            if ( isEmptyChar(currentPosition()) == false)
            {
                break ;
            }
        }
        return checkEnterChar() ;
    }
    parser_const::LexemFirstType BufferData::stepToPrevChar()
    {
        while (1)
        {
            prevBuffPosition();
            if ( isEmptyChar(currentPosition()) == false)
            {
                break ;
            }
        }
        return checkEnterChar() ;
    }

    void BufferData::getStrLexeme(std::string &ret_value)
    {
        ret_value.clear();

        char ch = currentPosition();
        if (isDoubleQuotes(ch) == false)
        {
            throw SyntaxErrExp() ;
        }
        nextBuffPosition();

        while (1)
        {
            ch = currentPosition();
            if (isSlash(ch) == true)
            {
                nextBuffPosition();
                ret_value.push_back(currentPosition());
            }
            else
            {
                if (isDoubleQuotes(ch) == true)
                {
                    return ;
                }
                ret_value.push_back(ch);
            }

            nextBuffPosition();
        }
    }
    void BufferData::getDirectLexeme(std::string &ret_value)
    {
        ret_value.clear();

        char ch = currentPosition();
        if (isCharDigit(ch) == false)
        {
            throw SyntaxErrExp() ;
        }
        ret_value.push_back(ch);
        nextBuffPosition();

        while (1)
        {
            ch = currentPosition();
            if ( isCharDigit(ch) == true || isPoint(ch))
            {
                ret_value.push_back(ch);
            }
            else
            {
                prevBuffPosition();
                return ;
            }
            nextBuffPosition();
        }
    }

    json_data::JsonValuePtr BufferData::createObjectData()
    {
        auto root_obj = json_data::JsonFactory::makeObject();
        auto& obj = dynamic_cast<json_data::JsonObject&>(*root_obj);

        parser_const::LexemFirstType current_type_lexeme = checkEnterChar() ;

        if ( current_type_lexeme != parser_const::LexemFirstType::LFT_isCurlyOpenBracket)
        {
            throw SyntaxErrExp() ;
        }

        do
        {
            std::string title_object;
            //--------- "_______" -----------
            current_type_lexeme = stepToNextChar();
            if ( current_type_lexeme != parser_const::LexemFirstType::LFT_DoubleQuotes)
            {
                throw SyntaxErrExp("not found '\"'") ;
            }
            readEnterStr(current_type_lexeme, title_object);
            //--------- : -----------------
            if ( stepToNextChar() != parser_const::LexemFirstType::LFT_isColon)
            {
                throw SyntaxErrExp("not found ':'") ;
            }
            //-------- right value -----------------
            obj.setMember(title_object, createRValueData());
            //-------- , -----------
            current_type_lexeme = stepToNextChar(); ;
            if ( current_type_lexeme != parser_const::LexemFirstType::LFT_isComma && current_type_lexeme != parser_const::LexemFirstType::LFT_isCurlyCloseBracket)
            {
                throw SyntaxErrExp("not found ','") ;
            }

        }
        while (current_type_lexeme != parser_const::LexemFirstType::LFT_isCurlyCloseBracket);

        return root_obj ;
    }

    json_data::JsonValuePtr BufferData::createArrayData()
    {
        auto root_obj = json_data::JsonFactory::makeArray();
        auto& array_obj = dynamic_cast<json_data::JsonArray&>(*root_obj);

        parser_const::LexemFirstType current_type_lexeme = checkEnterChar() ;
        if ( current_type_lexeme != parser_const::LexemFirstType::LFT_isOpenBracket)
        {
            return nullptr ;
        }

        do
        {
            array_obj.pushBack(createRValueData());
            current_type_lexeme = stepToNextChar();

            if ( current_type_lexeme != parser_const::LexemFirstType::LFT_isComma && current_type_lexeme != parser_const::LexemFirstType::LFT_isCloseBracket)
            {
                throw SyntaxErrExp() ;
            }
        }
        while (current_type_lexeme != parser_const::LexemFirstType::LFT_isCloseBracket);

        return root_obj ;
    }

    json_data::JsonValuePtr BufferData::createRValueData()
    {
        std::string work_object;

        auto type = stepToNextChar();
        switch (type)
        {
            case parser_const::LexemFirstType::LFT_isCurlyOpenBracket:
            {
                return createObjectData() ;
            }
            case parser_const::LexemFirstType::LFT_isOpenBracket:
            {
                return createArrayData() ;
            }
            case parser_const::LexemFirstType::LFT_DoubleQuotes:
            case parser_const::LexemFirstType::LFT_isSingleQuotes:
            {
                readEnterStr(type, work_object);
                return json_data::JsonFactory::makeString(work_object) ;
            }
            case parser_const::LexemFirstType::LFT_isDigitChar:
            {
                bool value = false ;
                readEnterStr(type, work_object);
                if ( isBooleanValue(work_object, value)  == true)
                {
                    return json_data::JsonFactory::makeBool(value) ;
                }
                if ( isNullValue(work_object) == true)
                {
                    return json_data::JsonFactory::makeNull() ;
                }
                double num = 0.0 ;
                if ( convertToNumber(work_object, num) == true)
                {
                    return json_data::JsonFactory::makeNumber(num) ;
                }
                return nullptr;
            }
        }

        return nullptr ;
    }

/*
    parser_const::LexemFirstType BufferData::getEnterChar(std::string &lexeme)
    {
        parser_const::LexemFirstType ret_value = parser_const::LexemFirstType::LFT_Unknown;

        lexeme.clear();
        try
        {
            skipEmptyChars() ;
            parser_const::DelimStrDataCit itDelim = delimStr.find(currentPosition());
            if ( itDelim == delimStr.end() && isCharDigit(currentPosition()) == false )
            {
                return ret_value;
            }
            if ( isCharDigit(currentPosition()))
            {
                getDirectLexeme(lexeme) ;
                ret_value = parser_const::LexemFirstType::LFT_isDigitChar ;
            }
            else
            {
                ret_value = itDelim->second ;
                switch (itDelim->second)
                {
                    case parser_const::LexemFirstType::LFT_DoubleQuotes:
                    case parser_const::LexemFirstType::LFT_isSingleQuotes:
                    {
                        getStrLexeme(lexeme) ;
                        ret_value = parser_const::LexemFirstType::LFT_isString ;
                        break ;
                    }
                    case parser_const::LexemFirstType::LFT_isColon:
                    case parser_const::LexemFirstType::LFT_isOpenBracket:
                    case parser_const::LexemFirstType::LFT_isCloseBracket:
                    case parser_const::LexemFirstType::LFT_isComma:
                    case parser_const::LexemFirstType::LFT_isCurlyOpenBracket:
                    case parser_const::LexemFirstType::LFT_isCurlyCloseBracket:
                    {
                        lexeme.push_back(currentPosition()) ;
                        break;
                    }

                }
            }
        }
        catch (EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        return ret_value ;
    }
*/
    void BufferData::readEnterStr(parser_const::LexemFirstType type, std::string &lexeme)
    {
        parser_const::LexemFirstType ret_value = parser_const::LexemFirstType::LFT_Unknown;

        lexeme.clear();

        try
        {
            switch (type)
            {
                case parser_const::LexemFirstType::LFT_isDigitChar:
                {
                    getDirectLexeme(lexeme) ;
                    break ;
                }
                case parser_const::LexemFirstType::LFT_DoubleQuotes:
                case parser_const::LexemFirstType::LFT_isSingleQuotes:
                {
                    getStrLexeme(lexeme) ;
                    break ;
                }
                case parser_const::LexemFirstType::LFT_isColon:
                case parser_const::LexemFirstType::LFT_isOpenBracket:
                case parser_const::LexemFirstType::LFT_isCloseBracket:
                case parser_const::LexemFirstType::LFT_isComma:
                case parser_const::LexemFirstType::LFT_isCurlyOpenBracket:
                case parser_const::LexemFirstType::LFT_isCurlyCloseBracket:
                {
                    lexeme.push_back(currentPosition()) ;
                    break;
                }

            }
        }
        catch (EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
    }

    parser_const::LexemFirstType BufferData::checkEnterChar() const
    {
        try
        {
            parser_const::DelimStrDataCit itDelim = delimStr.find(currentPosition());

            if ( itDelim == delimStr.end())
            {
                if ( isCharDigit(currentPosition()))
                {
                    return parser_const::LexemFirstType::LFT_isDigitChar ;
                }
                else
                {
                    return parser_const::LexemFirstType::LFT_Unknown ;
                }
            }
            return itDelim->second ;
        }
        catch (EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        catch (SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        return parser_const::LexemFirstType::LFT_Unknown ;
    }


    bool BufferData::isDoubleQuotes(const char ch) const
    {
        return ch == '"';

    }
    bool BufferData::isSingleQuotes(const char ch) const
    {
        return ch == '\'';

    }
    bool BufferData::isColon(const char ch) const
    {
        return ch == ':';
    }
    bool BufferData::isOpenBracket(const char ch) const
    {
        return ch == '[';
    }
    bool BufferData::isCloseBracket(const char ch) const
    {
        return ch == ']';
    }
    bool BufferData::isPoint(const char ch) const
    {
        return ch == '.';
    }
    bool BufferData::isSlash(const char ch) const
    {
        return ch == '\\';
    }
    bool BufferData::isDigit(const char ch) const
    {
        return isxdigit(ch) != 0 ;
    }
    bool BufferData::isChar(const char ch) const
    {
        return std::isalpha(ch);

    }
    bool BufferData::isCharDigit(const char ch) const
    {
        return isChar(ch) == true ||isDigit(ch) == true ;
    }
    bool BufferData::isBooleanValue(const std::string &str, bool &value) const
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
    bool BufferData::isNullValue(const std::string &str) const
    {
        return str == "null" ;
    }
    bool BufferData::convertToNumber(const std::string str, double &num) const
    {
        char* endptr;
        num = strtod(str.c_str(), &endptr); // endptr указывает на '\0' — строка полностью согласована
        return *endptr == '\0' ;
    }

}