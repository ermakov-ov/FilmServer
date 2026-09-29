
#ifndef FILMSERVER_WEB_CLIENT_H
#define FILMSERVER_WEB_CLIENT_H
#include <atomic>
#include <condition_variable>
#include <curl/curl.h>
#include <string>
#include <mutex>
#include <unordered_map>
#include <list>
#include <functional>
#include <thread>

namespace web_client {

struct Result
{
  int code;
  std::string data;
};

using CallbackFn = std::function<void(Result result)>;

struct Request
{
  CallbackFn callback;
  std::string buffer;
};

class WebClient {
public:

  WebClient();
  ~WebClient();

  void performRequest(const std::string& url, CallbackFn cb) ;
  static size_t writeToBuffer(char* ptr, size_t, size_t nmemb, void* tab)
  {
    auto r = reinterpret_cast<Request*>(tab);
    r->buffer.append(ptr, nmemb);
    return nmemb;
  }

private:
  void shutdown();
  void runLoop();

  CURLM* m_multiHandle;
  std::unordered_map<CURL*, Request> m_requests;  // ← владение здесь
  std::vector<CURL*> m_pending;
  std::atomic_bool m_break{false};
  std::condition_variable m_cv;
  std::mutex              m_mtx;
  std::thread             m_thread;
};
}

#endif // FILMSERVER_WEB_CLIENT_H
