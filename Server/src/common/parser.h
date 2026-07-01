//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "const.h"
#include "json_data.h"

namespace parser
{
    class ParserException : public std::exception
    {
        public:
            ParserException(const std::string &msg): m_text{msg} {}

        const char* what() const noexcept override
        {
            return m_text.c_str();
        }
        private:
            std::string m_text;

    };
    class EmptyBufferExp : public ParserException
    {
    public:
        EmptyBufferExp(): ParserException("empty buffer") {}

    };
    class OutOfRangeExp : public ParserException
    {
    public:
        OutOfRangeExp(): ParserException("try to go out of range") {}

    };
    class SyntaxErrExp : public ParserException
    {
    public:
        SyntaxErrExp(): ParserException("syntax error") {}
        SyntaxErrExp(const std::string &msg): ParserException(msg) {}

    };
    enum class LexemReadStatus
    {
        LRS_NO_ERROR=0,
        LRS_ERROR=1

    };

    using DataPtr = std::unique_ptr<char[]>;

    class BufferData
    {
    public:
        BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset=0);
        ~BufferData();

        //void getEnterChar() ;
        parser_const::LexemFirstType getEnterChar(std::string &ret_value) ;
        json_data::JsonValuePtr createObjectData() ;
        json_data::JsonValuePtr createRValueData();
        json_data::JsonValuePtr createArrayData();

        std::string get_lexem() ;
    private:
        const char nextBuffPosition() ;
        const char prevBuffPosition();
        const char currentPosition() const ;
        bool isEndStrTabChar(const char ch) const;
        bool isEmptyChar(const char ch) const ;
        parser_const::LexemFirstType stepToNextChar() ;
        parser_const::LexemFirstType stepToPrevChar();
        void getDirectLexeme(std::string &ret_value) ;
        void getStrLexeme(std::string &ret_value);
        parser_const::LexemFirstType checkEnterChar() const;
        void readEnterStr(parser_const::LexemFirstType type, std::string &lexeme) ;

        LexemReadStatus get_lexem(std::string &ret_value) ;
        bool isDoubleQuotes(const char ch) const ;
        bool isSingleQuotes(const char ch) const ;
        bool isColon(const char ch) const ;
        bool isOpenBracket(const char ch) const ;
        bool isCloseBracket(const char ch) const ;
        bool isComma(const char ch) const ;
        bool isPoint(const char ch) const;
        bool isSlash(const char ch) const ;
        bool isDigit(const char ch) const ;
        bool isChar(const char ch) const ;
        bool isCharDigit(const char ch) const ;
        bool isBooleanValue(const std::string &str, bool &value) const ;
        bool isNullValue(const std::string &str) const;
        bool convertToNumber(const std::string str, double &num) const ;
        

        DataPtr     m_data;
        char        *ptr_for_debug;
        std::size_t m_length;
        std::size_t m_offset;
    };


}
#endif //SERVER_PARSER_H
