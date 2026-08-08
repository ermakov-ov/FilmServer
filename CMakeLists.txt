cmake_minimum_required(VERSION 3.16)
project(FilmServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(BUILD_TESTS "Build tests" ON)


# json_lib
add_subdirectory(src/json)

# film_db_lib
add_subdirectory(src/film_db)

# common (если там есть .cpp)
#add_subdirectory(src/common)

# Server (HTTP API)
add_subdirectory(src/apps/server)

# Player (проигрыватель)
add_subdirectory(src/apps/player)

# --- Тесты (только если BUILD_TESTS=OFF) ---
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
