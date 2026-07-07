cmake_minimum_required(VERSION 3.16)
project(JsonParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция: включать ли тесты (по умолчанию ON, чтобы my_tests появился)
option(BUILD_TESTS "Build tests" ON)

# --- Библиотека json_lib ---
add_library(json_lib STATIC
        src/common/common.cpp
        src/json/json_data.cpp
        src/json/lexer.cpp
        src/json/parser_json.cpp
        src/json/stream.cpp
        # добавь сюда остальные cpp файлы библиотеки, если есть
)
target_include_directories(json_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Приложение Server ---
add_executable(Server src/main.cpp
        src/film_db/actor.h
        src/film_db/director.h
        src/film_db/film.h
        src/film_db/film_db.h
        src/film_db/film_db.cpp)
target_link_libraries(Server PRIVATE json_lib)

# --- Тесты (только если BUILD_TESTS=ON) ---
if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
    # Для Windows может понадобиться, на Ubuntu не критично
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
