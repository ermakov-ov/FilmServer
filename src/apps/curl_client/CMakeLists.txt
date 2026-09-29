find_package(CURL REQUIRED)

add_executable(curl_client main.cpp
        web-client.cpp)
target_link_libraries(curl_client PRIVATE CURL::libcurl)