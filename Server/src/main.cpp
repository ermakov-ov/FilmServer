#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "json/parser.h"

bool loadConfig(const std::string& path, json_data::JsonValuePtr& outRoot) ;

int main()
{
    std::string file_name("test_data.json") ;
    json_data::JsonValuePtr ptr_json;

    if ( loadConfig(file_name, ptr_json) == false )
    {
        std::cerr<<"Couldn't create the JSON dat from file "<< file_name<<std::endl;
        return 1;
    }

    return 0;
}
bool loadConfig(const std::string& path, json_data::JsonValuePtr& outRoot)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return false;
    }
    std::size_t fileSize = file.tellg();

    parser::DataPtr jsn_data = std::make_unique<char[]>(fileSize + 1);

    file.seekg(0, std::ios::beg);
    file.read(jsn_data.get(), static_cast<std::streamsize>(fileSize));
    jsn_data[fileSize] = '\0' ;

    parser::BufferData buffer_data(std::move(jsn_data), fileSize);

    try {
        outRoot = buffer_data.makeJsonData();
        return true;
    }
    catch (const parser_common::SyntaxErrExp& e) {
        std::cerr << "Syntax error at position " << e.position() << ": " << e.what() << "\n";
        return false;
    }
    catch (const parser_common::OutOfRangeExp& e) {
        std::cerr << "Buffer error at position " << e.position() << "\n";
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return false;
    }
}