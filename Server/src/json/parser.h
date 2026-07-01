//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <memory>
#include <string>
#include <iostream>
#include "../common/common.h"
#include "json_data.h"

namespace parser
{
    using DataPtr = std::unique_ptr<char[]>;

    class BufferData
    {
    public:
        BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset=0);
        ~BufferData() = default;

        json_data::JsonValuePtr makeJsonData();


    private:
        json_data::JsonValuePtr createObjectData() ;
        json_data::JsonValuePtr createRValueData();
        json_data::JsonValuePtr createArrayData();

        char nextBuffPosition();
        char prevBuffPosition();
        char currentPosition() const ;

        parser_common::LexemeFirstType stepToNextChar() ;
        parser_common::LexemeFirstType stepToPrevChar();

        void getDirectLexeme(std::string &ret_value) ;
        void getStrLexeme(std::string &ret_value);

        parser_common::LexemeFirstType checkEnterChar() const;
        void readEnterStr(parser_common::LexemeFirstType type, std::string &lexeme) ;

        DataPtr     m_data;
        std::size_t m_length;
        std::size_t m_offset;
    };


}
#endif //SERVER_PARSER_H
