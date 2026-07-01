//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_CONST_H
#define SERVER_CONST_H
#include <string>
#include <vector>
#include <map>

namespace parser_const
{
    enum class LexemFirstType
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


    using DelimStrData = std::map<unsigned char, LexemFirstType>;
    using DelimStrDataCit = std::map<unsigned char, LexemFirstType>::const_iterator;
    using DelimStrDataIt = std::map<unsigned char, LexemFirstType>::iterator;
}



#endif //SERVER_CONST_H
