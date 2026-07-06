#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "json/parser_json.h"

json_data::JsonValuePtr loadConfig(const std::string& path);

int main()
{
    try
    {
        std::string file_name("test5.json") ;
        auto jsn_data = loadConfig(file_name);
    }
    catch (const parser::ParseError& e)
    {
        std::cout << "Parse error at position " << e.position()<< ": " << e.what() << std::endl;
    }
    catch (const parser::LexerError& e)
    {
        std::cout << "Parse error at position " << e.position()<< ": " << e.what() << std::endl;

    }
    catch (const parser::StreamError& e)
    {
        std::cout << "StremBuffer error - " << e.what() << std::endl;

    }
    catch (const std::exception& e)
    {
        std::cout << "Low-level parse error - " << e.what() << std::endl;
     }

    return 0;
}

json_data::JsonValuePtr loadConfig(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return nullptr;
    }
    std::size_t fileSize = file.tellg();

    auto jsn_data = std::make_unique<char[]>(fileSize + 1);

    file.seekg(0, std::ios::beg);
    file.read(jsn_data.get(), static_cast<std::streamsize>(fileSize));
    jsn_data[fileSize] = '\0' ;

    try
    {
        parser::StreamBuffer stream(std::move(jsn_data), fileSize);
        parser::ParserJson parser(std::move(stream));

        return parser.parse();
    }
    catch (const parser::ParseError& e)
    {
        std::cerr << "Parse error at position " << e.position()<< ": " << e.what() << std::endl;
        throw;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Low-level error: " << e.what() << std::endl;
        throw;
    }
}
