#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "json/parser.h"


int main()
{
    std::ifstream file("test_data.json", std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return 1;
    }
    std::size_t fileSize = file.tellg();


    parser::DataPtr jsn_data = std::make_unique<char[]>(fileSize + 1);
    file.seekg(0, std::ios::beg);
    file.read(jsn_data.get(), static_cast<std::streamsize>(fileSize));
    jsn_data[fileSize] = '\0' ;

    parser::BufferData buffer_data(jsn_data, fileSize);
    auto jsanPtrData = buffer_data.makeJsonData() ;

    return 0;
}