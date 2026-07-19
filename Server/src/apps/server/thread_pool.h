#ifndef HORSELINE_THREAD_POOL_H
#define HORSELINE_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>

namespace film_server {
    class ThreadPool {
    public:
        explicit ThreadPool(std::size_t num_threads);
        ~ThreadPool();

        template <typename F>
        void enqueue(F&& f)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_tasks.emplace(std::forward<F>(f));
            }
            m_cv.notify_one();
        }

        template <typename F> auto submit(F&& f)
        -> std::future<decltype(std::forward<F>(f)())>
        {
            using ReturnType = decltype(std::forward<F>(f)());

            auto packaged_task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::forward<F>(f)
            );
            auto result = packaged_task->get_future();

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_tasks.emplace([packaged_task]() { (*packaged_task)(); });
            }
            m_cv.notify_one();
            return result;
        }

    private:
        void worker_loop();

        std::vector<std::thread>           m_workers;
        std::queue<std::function<void()>>  m_tasks;
        std::mutex                         m_mutex;
        std::condition_variable            m_cv;
        std::atomic<bool>                  m_stop;
    };
}
#endif //HORSELINE_THREAD_POOL_H
