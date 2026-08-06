
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_executable(Player
        main.cpp
        main_window.cpp
        load_config.cpp
        ../../common/logger.cpp
)

target_link_libraries(Player PRIVATE
        json_lib
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
)

set_target_properties(Player PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)

