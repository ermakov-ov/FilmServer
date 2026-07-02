//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_CONST_H
#define SERVER_CONST_H
#include <string>
#include <map>
#include <stdexcept>

namespace parser_common
{
    class ParserException : public std::runtime_error
    {
    public:
        explicit ParserException(const std::string &msg, size_t pos): std::runtime_error(msg) {}
        size_t position() const noexcept { return m_pos; }
    private:
        size_t m_pos;

    };
    class EmptyBufferExp : public ParserException
    {
    public:
        explicit EmptyBufferExp(size_t pos): ParserException("empty buffer", pos) {}
    };

    class OutOfRangeExp : public ParserException
    {
    public:
        explicit OutOfRangeExp(size_t pos): ParserException("try to go out of range", pos) {}
    };

    class SyntaxErrExp : public ParserException
    {
    public:
        explicit SyntaxErrExp(size_t pos): ParserException("syntax error", pos) {}
        explicit SyntaxErrExp(const std::string &msg, size_t pos): ParserException(msg, pos) {}
    };

    enum class LexemeFirstType
    {
        LFT_DoubleQuotes       = 0,
        LFT_isSingleQuotes     = 1,
        LFT_isColon            = 2,
        LFT_isOpenBracket      = 3,
        LFT_isCloseBracket     = 4,
        LFT_isComma            = 5,
        LFT_isDigitChar        = 6,
        LFT_isString           = 7,
        LFT_isCurlyOpenBracket = 8,
        LFT_isCurlyCloseBracket = 9,
        LFT_Unknown
    };

    using DelimStrData = std::map<unsigned char, LexemeFirstType>;
    using DelimStrDataCit = std::map<unsigned char, LexemeFirstType>::const_iterator;
    using DelimStrDataIt = std::map<unsigned char, LexemeFirstType>::iterator;
    
    
    bool isDoubleQuotes(char );
    bool isSingleQuotes(char );
    bool isColon(char ch);
    bool isOpenBracket(char ch);
    bool isCloseBracket(char ch);
    bool isPoint(char ch);
    bool isSlash(char ch);
    bool isDigit(char ch);
    bool isChar(char ch);
    bool isCharDigit(char ch);
    bool isBooleanValue(const std::string &str, bool &value);
    bool isNullValue(const std::string &str);
    bool convertToNumber(const std::string &str, double &num);
    bool isEndStrTabChar(char ch);
    bool isEmptyChar(char ch);
 
}



#endif //SERVER_CONST_H
