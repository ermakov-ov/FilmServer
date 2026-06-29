#include <iostream>
#include <fstream>
#include "common/parser.h"


int main()
{
    std::ifstream file("test_data.json");
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return 1;
    }
    file.seekg(0, std::ios::end); // Переводим указатель в конец
    std::streampos file_size = file.tellg(); // Получаем позицию — это и есть размер
    file.seekg(0, std::ios::beg); // Переводим указатель в конец


    char *lp = new char[file_size];
    //parser::DataPtr buffer(new char)
    //file.read()

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