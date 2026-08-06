add_library(film_db_lib STATIC
        film_db.cpp
        )
target_link_libraries(film_db_lib PUBLIC json_lib)
target_include_directories(film_db_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})