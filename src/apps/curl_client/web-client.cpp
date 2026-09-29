#include "web_client.h"

#include <iostream>

namespace web_client {

WebClient::WebClient()
:m_multiHandle(curl_multi_init())
,m_thread(&WebClient::runLoop, this)
{
}

WebClient::~WebClient()
{
  shutdown() ;

  if (m_multiHandle) {
    curl_multi_cleanup(m_multiHandle);
    m_multiHandle = nullptr;
  }
}

void WebClient::performRequest(const std::string& url, CallbackFn cb)
{
  std::lock_guard<std::mutex> lk(m_mtx);

  CURL* handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WebClient::writeToBuffer);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, 5000);

  // Создаём Request прямо в контейнере
  auto [it, inserted] = m_requests.emplace(handle, Request{std::move(cb), {}});
  if (!inserted) {
    curl_easy_cleanup(handle);  // не забываем почистить
    return; // handle уже есть — редкая ситуация, но лучше проверить
  }

  Request& req = it->second;
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &req);
  curl_easy_setopt(handle, CURLOPT_PRIVATE, &req);

  //curl_multi_add_handle(m_multiHandle, handle);
  m_pending.push_back(handle);
  m_cv.notify_one();
}

void WebClient::shutdown()
{
  {
    std::unique_lock<std::mutex> lk(m_mtx);
    m_break = true;
    m_cv.notify_all();
  }

  if ( m_thread.joinable() ) {
    m_thread.join();
  }
}


void WebClient::runLoop()
{
  int msgs_left;

  while (!m_break) {
    int still_running = 0;
    {
      std::lock_guard<std::mutex> lk(m_mtx);
      for (CURL* h : m_pending) {
        curl_multi_add_handle(m_multiHandle, h);
      }
      m_pending.clear();
    }

    curl_multi_perform(m_multiHandle, &still_running);

    // 2. Сразу разбираем готовые ответы (они могли появиться во время perform)
    CURLMsg* msg;
    while ((msg = curl_multi_info_read(m_multiHandle, &msgs_left))) {
      if (msg->msg == CURLMSG_DONE) {
        CURL* handle = msg->easy_handle;
        Request* reqPtr = nullptr;
        long httpCode = 0;

        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &reqPtr);
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpCode);

        reqPtr->callback({static_cast<int>(httpCode), std::move(reqPtr->buffer)});

        curl_multi_remove_handle(m_multiHandle, handle);
        curl_easy_cleanup(handle);
        m_requests.erase(handle);
      }
    }

    // 3. Если активных передач нет — ждём, когда добавят новый запрос
    if (!still_running) {
      std::unique_lock<std::mutex> lk(m_mtx);
      m_cv.wait(lk, [this]() { return !m_requests.empty() || m_break; });
      if (m_break || !m_multiHandle) return;
      continue; // Сразу идём на следующий perform, без poll
    }

    // 4. Есть активные передачи, но прямо сейчас нечего делать — ждём события от сети
    long timeout_ms = -1;
    curl_multi_timeout(m_multiHandle, &timeout_ms);

    if (timeout_ms < 0) {
      // curl говорит «ждать вечно», но у нас есть m_break, поэтому ставим большой, но конечный таймаут
      timeout_ms = 1000;
    }

    int numfds = 0;
    curl_multi_poll(m_multiHandle, nullptr, 0, timeout_ms, &numfds);
  }
}


}

