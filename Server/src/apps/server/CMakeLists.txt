add_executable(Server
        main.cpp
        load_json.cpp
)
target_link_libraries(Server PRIVATE film_db_lib)
target_include_directories(Server PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ../../third_party/httplib
)