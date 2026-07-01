
#include "parser.h"
#include "../common/common.h"

parser_common::DelimStrData delimStr = {
    {'"', parser_common::LexemeFirstType::LFT_DoubleQuotes   },
    {'\'', parser_common::LexemeFirstType::LFT_isSingleQuotes },
    {':', parser_common::LexemeFirstType::LFT_isColon        },
    {'[', parser_common::LexemeFirstType::LFT_isOpenBracket  },
    {']', parser_common::LexemeFirstType::LFT_isCloseBracket },
    {',', parser_common::LexemeFirstType::LFT_isComma        },
    {'{', parser_common::LexemeFirstType::LFT_isCurlyOpenBracket },
    {'}', parser_common::LexemeFirstType::LFT_isCurlyCloseBracket}
};

namespace parser
{
    BufferData::BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset)
    :m_data(std::move(ex_data))
    ,m_length(length)
    ,m_offset(offset)
    {
    }

    char BufferData::nextBuffPosition()
    {
        if (m_length <= 0 )
        {
            throw parser_common::EmptyBufferExp() ;
        }
        if ( m_offset + 1 >= m_length)
        {
            throw parser_common::OutOfRangeExp() ;
        }

        return m_data[++m_offset] ;
    }
    char BufferData::prevBuffPosition()
    {
        if (m_length <= 0 )
        {
            throw parser_common::EmptyBufferExp() ;
        }
        if ( m_offset == 0)
        {
            throw parser_common::OutOfRangeExp() ;
        }

        return m_data[--m_offset] ;
    }
    char BufferData::currentPosition() const
    {
        if (m_length <= 0 )
        {
            throw parser_common::EmptyBufferExp();
        }
        return m_data[m_offset] ;
    }
    parser_common::LexemeFirstType BufferData::stepToNextChar()
    {
        do
        {
            nextBuffPosition();
        }
        while (parser_common::isEmptyChar(currentPosition()) == true);

        return checkEnterChar() ;
    }
    parser_common::LexemeFirstType BufferData::stepToPrevChar()
    {
        do
        {
            prevBuffPosition();
        }
        while (parser_common::isEmptyChar(currentPosition()) == true);

        return checkEnterChar() ;
    }

    void BufferData::getStrLexeme(std::string &ret_value)
    {
        ret_value.clear();

        char ch = currentPosition();
        if (parser_common::isDoubleQuotes(ch) == false)
        {
            throw parser_common::SyntaxErrExp() ;
        }

        do
        {
            nextBuffPosition();
            ch = currentPosition();
            if (parser_common::isSlash(ch) == true)
            {
                nextBuffPosition();
                ret_value.push_back(currentPosition());
            }
            else
            {
                if (parser_common::isDoubleQuotes(ch) == true)
                {
                    return ;
                }
                ret_value.push_back(ch);
            }

        }
        while (true);
    }
    void BufferData::getDirectLexeme(std::string &ret_value)
    {
        ret_value.clear();

        char ch = currentPosition();
        if (parser_common::isCharDigit(ch) == false)
        {
            throw parser_common::SyntaxErrExp() ;
        }
        ret_value.push_back(ch);

        do
        {
            ch = nextBuffPosition();
            if ( parser_common::isCharDigit(ch) == true || parser_common::isPoint(ch))
            {
                ret_value.push_back(ch);
            }
            else
            {
                prevBuffPosition();
                return ;
            }
        }
        while (true);
    }

    json_data::JsonValuePtr BufferData::makeJsonData()
    {
        try
        {
            return createObjectData() ;
        }
        catch (parser_common::EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        return nullptr;
    }

    json_data::JsonValuePtr BufferData::createObjectData()
    {
        auto root_obj = json_data::JsonFactory::makeObject();
        auto& obj = dynamic_cast<json_data::JsonObject&>(*root_obj);

        parser_common::LexemeFirstType current_type_lexeme = checkEnterChar() ;

        if ( current_type_lexeme != parser_common::LexemeFirstType::LFT_isCurlyOpenBracket)
        {
            throw parser_common::SyntaxErrExp() ;
        }

        do
        {
            std::string title_object;
            //--------- "_______" -----------
            current_type_lexeme = stepToNextChar();
            if ( current_type_lexeme != parser_common::LexemeFirstType::LFT_DoubleQuotes)
            {
                throw parser_common::SyntaxErrExp("not found '\"'") ;
            }
            readEnterStr(current_type_lexeme, title_object);
            //--------- : -----------------
            if ( stepToNextChar() != parser_common::LexemeFirstType::LFT_isColon)
            {
                throw parser_common::SyntaxErrExp("not found ':'") ;
            }
            //-------- right value -----------------
            obj.setMember(title_object, createRValueData());
            //-------- , -----------
            current_type_lexeme = stepToNextChar(); ;
            if ( current_type_lexeme != parser_common::LexemeFirstType::LFT_isComma && current_type_lexeme != parser_common::LexemeFirstType::LFT_isCurlyCloseBracket)
            {
                throw parser_common::SyntaxErrExp("not found ','") ;
            }
        }
        while (current_type_lexeme != parser_common::LexemeFirstType::LFT_isCurlyCloseBracket);

        return root_obj ;
    }

    json_data::JsonValuePtr BufferData::createArrayData()
    {
        auto root_obj = json_data::JsonFactory::makeArray();
        auto& array_obj = dynamic_cast<json_data::JsonArray&>(*root_obj);

        parser_common::LexemeFirstType current_type_lexeme = checkEnterChar() ;
        if ( current_type_lexeme != parser_common::LexemeFirstType::LFT_isOpenBracket)
        {
            return nullptr ;
        }

        do
        {
            array_obj.pushBack(createRValueData());
            current_type_lexeme = stepToNextChar();

            if ( current_type_lexeme != parser_common::LexemeFirstType::LFT_isComma && current_type_lexeme != parser_common::LexemeFirstType::LFT_isCloseBracket)
            {
                throw parser_common::SyntaxErrExp() ;
            }
        }
        while (current_type_lexeme != parser_common::LexemeFirstType::LFT_isCloseBracket);

        return root_obj ;
    }

    json_data::JsonValuePtr BufferData::createRValueData()
    {
        std::string work_object;

        auto type = stepToNextChar();
        switch (type)
        {
            case parser_common::LexemeFirstType::LFT_isCurlyOpenBracket:
                return createObjectData() ;

            case parser_common::LexemeFirstType::LFT_isOpenBracket:
                return createArrayData() ;

            case parser_common::LexemeFirstType::LFT_DoubleQuotes:
            case parser_common::LexemeFirstType::LFT_isSingleQuotes:
            {
                readEnterStr(type, work_object);
                return json_data::JsonFactory::makeString(work_object) ;
            }
            case parser_common::LexemeFirstType::LFT_isDigitChar:
            {
                bool value = false ;
                readEnterStr(type, work_object);
                if ( parser_common::isBooleanValue(work_object, value)  == true)
                {
                    return json_data::JsonFactory::makeBool(value) ;
                }
                if ( parser_common::isNullValue(work_object) == true)
                {
                    return json_data::JsonFactory::makeNull() ;
                }
                double num = 0.0 ;
                if ( parser_common::convertToNumber(work_object, num) == true)
                {
                    return json_data::JsonFactory::makeNumber(num) ;
                }
                return nullptr;
            }
        }

        return nullptr ;
    }

    void BufferData::readEnterStr(parser_common::LexemeFirstType type, std::string &lexeme)
    {
        lexeme.clear();
        try
        {
            switch (type)
            {
                case parser_common::LexemeFirstType::LFT_isDigitChar:
                {
                    getDirectLexeme(lexeme) ;
                    break ;
                }
                case parser_common::LexemeFirstType::LFT_DoubleQuotes:
                case parser_common::LexemeFirstType::LFT_isSingleQuotes:
                {
                    getStrLexeme(lexeme) ;
                    break ;
                }
                case parser_common::LexemeFirstType::LFT_isColon:
                case parser_common::LexemeFirstType::LFT_isOpenBracket:
                case parser_common::LexemeFirstType::LFT_isCloseBracket:
                case parser_common::LexemeFirstType::LFT_isComma:
                case parser_common::LexemeFirstType::LFT_isCurlyOpenBracket:
                case parser_common::LexemeFirstType::LFT_isCurlyCloseBracket:
                {
                    lexeme.push_back(currentPosition()) ;
                    break;
                }

            }
        }
        catch (parser_common::EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
    }

    parser_common::LexemeFirstType BufferData::checkEnterChar() const
    {
        try
        {
            parser_common::DelimStrDataCit itDelim = delimStr.find(currentPosition());

            if ( itDelim == delimStr.end())
            {
                if ( parser_common::isCharDigit(currentPosition()))
                {
                    return parser_common::LexemeFirstType::LFT_isDigitChar ;
                }
                return parser_common::LexemeFirstType::LFT_Unknown ;
            }
            return itDelim->second ;
        }
        catch (parser_common::EmptyBufferExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::OutOfRangeExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }
        catch (parser_common::SyntaxErrExp &e)
        {
            std::cout<<e.what()<<std::endl;
        }

        return parser_common::LexemeFirstType::LFT_Unknown ;
    }
}