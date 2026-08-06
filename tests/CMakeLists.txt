add_executable(my_tests test_lexer.cpp
        test_parser.cpp
        test_films.cpp
        ../src/apps/server/load_json.cpp
        test_json_serialize.cpp
        ../src/common/logger.cpp
)

target_link_libraries(
        my_tests
        PRIVATE
        json_lib
        film_db_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(my_tests)