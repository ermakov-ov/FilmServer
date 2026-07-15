add_executable(Server
        main.cpp
        load_json.cpp
        film_server.cpp
        film_server.h
        response_builder.h
        thread_pool.cpp
        thread_pool.h
        file_streamer.cpp
        file_streamer.h
        ../../common/logger.cpp
)
target_link_libraries(Server PRIVATE film_db_lib)
target_include_directories(Server PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ../../third_party/httplib
)