#include <curl/curl.h>
#include <iostream>
#include <string>
#include "web_client.h"

int finish_flag = 0 ;

int main(int argc, char* argv[])
{
  std::vector<std::pair<std::string,std::string>> testLinks = {
    {"Search by a film name", "http://127.0.0.1:8080/api/v1/search?by_title=Inception"},
    {"Search by a actor name","http://127.0.0.1:8080/api/v1/search?by_actor=Leonardo%20DiCaprio"},
    {"Got a films statistic", "http://127.0.0.1:8080/api/v1/stats"}};

  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    return 1;
  }
  try {
    web_client::WebClient client;

    for ( int i = 0; i < testLinks.size(); i++ ) {
      std::string testName = testLinks[i].first;
      client.performRequest(testLinks[i].second, [testName](web_client::Result res)
        {
          if (res.code == 200) {
            std::cout << "Got answer for "<< testName<<" request" << std::endl;
            std::cout << res.data << std::endl;
          }
          else {
            std::cerr << "Got error for "<<testName<<". Code answer - "<<res.code << std::endl;
          }
        finish_flag++ ;
        });

    }

    while (finish_flag != testLinks.size()) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }

  }
  catch (std::exception& e) {

  }
  curl_global_cleanup();
  return 0;
}