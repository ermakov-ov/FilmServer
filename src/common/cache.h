
#ifndef FILMSERVER_CACHE_H
#define FILMSERVER_CACHE_H

#include <unordered_map>
#include <chrono>
#include <optional>

template<typename Key, typename Value>
class TimedCache {
public:
  explicit TimedCache(std::chrono::milliseconds ttl) : m_ttl(ttl) {}

  void set(const Key& key, const Value& value)
  {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data[key] = {value, now};
  }

  std::optional<Value> get(const Key& key)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_data.find(key);
    if (it == m_data.end()) {
      return std::nullopt;
    }

    auto now = std::chrono::steady_clock::now();
    if (now - it->second.timestamp >= m_ttl) {
      m_data.erase(it);
      return std::nullopt;
    }

    return it->second.value;
  }

private:
  struct Entry {
    Value value;
    std::chrono::steady_clock::time_point timestamp;
  };

  std::unordered_map<Key, Entry> m_data;
  std::chrono::milliseconds m_ttl;
  mutable std::mutex m_mutex; // mutable, чтобы можно было брать lock в const-методах
};



#endif // FILMSERVER_CACHE_H
