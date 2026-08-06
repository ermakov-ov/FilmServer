add_executable(Server
        main.cpp
        load_json.cpp
        film_server.cpp
        film_server.h
        thread_pool.cpp
        file_streamer.cpp
        ../../common/logger.cpp
        response_builder.cpp
)
target_link_libraries(Server PRIVATE film_db_lib)
target_include_directories(Server PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ../../third_party/httplib
)