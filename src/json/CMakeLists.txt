
add_library(json_lib STATIC
        ../common/common.cpp
        json_data.cpp
        lexer.cpp
        parser_json.cpp
        stream.cpp
)
target_include_directories(json_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


