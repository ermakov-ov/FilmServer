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
    }

    BufferData::~BufferData()
    {

    }


    const char BufferData::nextPosition()
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
    const char BufferData::prevPosition()
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

    void BufferData::skipEmptyChars()
    {
        while (1)
        {
            if ( isEmptyChar(currentPosition()) == false)
            {
                break ;
            }
            nextPosition();
        }

    }

    void BufferData::getStrLexeme(std::string &ret_value)
    {
        ret_value.clear();

        char ch = currentPosition();
        if (isDoubleQuotes(ch) == false)
        {
            throw SyntaxErrExp() ;
        }
        nextPosition();

        while (1)
        {
            ch = currentPosition();
            if (isSlash(ch) == true)
            {
                nextPosition();
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

            nextPosition();
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
        nextPosition();

        while (1)
        {
            ch = currentPosition();
            if ( isCharDigit(ch) == true  || isPoint(ch))
            {
                ret_value.push_back(ch);
            }
            else
            {
                skipEmptyChars();
                prevPosition();
                return ;
            }
            nextPosition();
        }
    }
    void BufferData::getEnterChar()
    {
        try
        {
            while (1)
            {
                skipEmptyChars() ;
                parser_const::DelimStrDataCit itDelim = delimStr.find(currentPosition());
                if ( itDelim == delimStr.end() && isCharDigit(currentPosition()) == false )
                {
                    std::cout<<"Erroneous behavior"<<std::endl;
                    return ;
                }
                if ( isCharDigit(currentPosition()))
                {
                    std::string q_str ;
                    getDirectLexeme(q_str) ;
                    std::cout<<q_str<<std::endl;
                }
                else
                {
                    switch (itDelim->second)
                    {
                        case parser_const::LexemFirstType::LFT_DoubleQuotes:
                        case parser_const::LexemFirstType::LFT_isSingleQuotes:
                        {
                            std::string q_str ;
                            getStrLexeme(q_str) ;
                            std::cout<<q_str<<std::endl;
                            break ;
                        }
                        case parser_const::LexemFirstType::LFT_isColon:
                        case parser_const::LexemFirstType::LFT_isOpenBracket:
                        case parser_const::LexemFirstType::LFT_isCloseBracket:
                        case parser_const::LexemFirstType::LFT_isComma:
                        case parser_const::LexemFirstType::LFT_isCurlyOpenBracket:
                        case parser_const::LexemFirstType::LFT_isCurlyCloseBracket:
                        {
                            std::cout<<"Symbol - "<<currentPosition()<<std::endl;
                            break;
                        }

                    }
                }
                nextPosition() ;
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

}