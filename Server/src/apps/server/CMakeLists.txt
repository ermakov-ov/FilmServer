add_executable(Server
        main.cpp
        load_json.cpp
)
target_link_libraries(Server PRIVATE film_db_lib)