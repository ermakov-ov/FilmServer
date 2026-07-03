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
