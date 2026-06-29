//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <memory>
#include <string>
#include "const.h"

namespace parser
{
    enum class LexemReadStatus
    {
        LRS_NO_ERROR=0,
        LRS_ERROR=1

    };

    using DataPtr = std::unique_ptr<char>;

    class BufferData
    {
    public:
        BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset);
        ~BufferData();

        std::string get_lexem() ;
    private:
        LexemReadStatus nextPosition() ;
        parser_const::LexemFirstType getStartChar() const ;

        LexemReadStatus get_lexem(std::string &ret_value) ;
        bool isDoubleQuotes(const char ch) const ;
        bool isSingleQuotes(const char ch) const ;
        bool isColon(const char ch) const ;
        bool isOpenBracket(const char ch) const ;
        bool isCloseBracket(const char ch) const ;
        bool isComma(const char ch) const ;
        bool isDigit(const char ch) const ;
        bool isChar(const char ch) const ;
        bool isCharDigit(const char ch) const ;
        
        
        
        DataPtr     m_data;
        std::size_t m_length;
        std::size_t m_offset;
    };
    class Lexem
    {
    public:
        Lexem();
    private:

    };

    class Parser {};
}
#endif //SERVER_PARSER_H
