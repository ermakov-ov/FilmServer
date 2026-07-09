add_executable(Player
        main.cpp
        player_logic.cpp
        http_client.cpp
)
# Подключаем только то, что реально нужно плееру
target_link_libraries(Player PRIVATE common json_lib)