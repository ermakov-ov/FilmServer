#ifndef HORSELINE_THREAD_POOL_H
#define HORSELINE_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace film_server {
    class ThreadPool {
    public:
        explicit ThreadPool(std::size_t num_threads) ;
        ~ThreadPool();

        template <typename F> void enqueue(F&& f)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_tasks.emplace(std::forward<F>(f));
            }
            m_cv.notify_one();
        }

    private:
        std::vector<std::thread>           m_workers;
        std::queue<std::function<void()>>  m_tasks;
        std::mutex                         m_mutex ;
        std::condition_variable            m_cv    ;
        std::atomic<bool>                  m_stop  ;
    };
}
#endif //HORSELINE_THREAD_POOL_H
