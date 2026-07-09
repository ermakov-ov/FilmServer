cmake_minimum_required(VERSION 3.16)
project(HorseLine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция: включать ли тесты (по умолчанию ON, чтобы my_tests появился)
option(BUILD_TESTS "Build tests" ON)

# --- Общие библиотеки ---

# json_lib
add_subdirectory(src/json)          # внутри: add_library(json_lib ...)

# film_db_lib
add_subdirectory(src/film_db)       # внутри: add_library(film_db_lib ...), link json_lib

# common (если там есть .cpp)
#add_subdirectory(src/common)       # если только .h — можно просто добавить include dir

# --- Приложения ---

# Server (HTTP API)
add_subdirectory(src/apps/server)       # внутри: add_executable(Server ...), link film_db_lib

# Player (проигрыватель)
add_subdirectory(src/apps/player)       # внутри: add_executable(Player ...), link common, json_lib и т.д.


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
