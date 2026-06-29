#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "common/parser.h"


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
    auto dd = buffer_data.getStartChar() ;
    buffer_data.nextPosition();

    const auto lang = "C++";
    std::cout << "Hello and welcome to " << lang << "!\n";

    for (int i = 1; i <= 5; i++)
    {
        // TIP Press <shortcut actionId="Debug"/> to start debugging your code. We have set one <icon src="AllIcons.Debugger.Db_set_breakpoint"/> breakpoint for you, but you can always add more by pressing <shortcut actionId="ToggleLineBreakpoint"/>.
        std::cout << "i = " << i << std::endl;
    }

    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}