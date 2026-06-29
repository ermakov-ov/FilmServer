//
// Created by eow on 23/06/2026.
//
#include <cctype>
#include "parser.h"
#include "const.h"

parser_const::DelimStrData delimStr = {
    {'"', parser_const::LexemFirstType::LFT_DoubleQuotes   },
    {'\'', parser_const::LexemFirstType::LFT_isSingleQuotes },
    {':', parser_const::LexemFirstType::LFT_isColon        },
    {'[', parser_const::LexemFirstType::LFT_isOpenBracket  },
    {']', parser_const::LexemFirstType::LFT_isCloseBracket },
    {',', parser_const::LexemFirstType::LFT_isComma        }
};

namespace parser
{
    BufferData::BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset)
    :m_data(std::move(ex_data))
    ,m_length(length)
    ,m_offset(offset)
    {
    }
    LexemReadStatus BufferData::nextPosition()
    {
        if (m_length <= 0 || m_offset + 1 >= m_length)
        {
            return LexemReadStatus::LRS_ERROR;
        }
    }
    BufferData::~BufferData()
    {

    }

    parser_const::LexemFirstType BufferData::getStartChar() const
    {
        parser_const::DelimStrDataCit itDelim = delimStr.find(m_data.get()[m_offset]);
        if ( itDelim != delimStr.end())
        {
            return itDelim->second ;
        }
        if ( isDigit(m_data.get()[m_offset]))
        {
            return parser_const::LexemFirstType::LFT_isDigit ;
        }
        if ( isChar(m_data.get()[m_offset]))
        {
            return parser_const::LexemFirstType::LFT_isChar ;
        }
        return parser_const::LexemFirstType::LFT_Unknown;
    }



    LexemReadStatus BufferData::get_lexem(std::string &ret_value)
    {
        ret_value.clear();



    }
    bool BufferData::isDoubleQuotes(const char ch) const
    {
        return ch == '"' || ch == '\'';

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
    bool BufferData::isComma(const char ch) const
    {
        return ch == ',';
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

}