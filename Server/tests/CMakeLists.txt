add_executable(my_tests test_lexer.cpp)

target_link_libraries(
        my_tests
        PRIVATE
        json_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(my_tests)