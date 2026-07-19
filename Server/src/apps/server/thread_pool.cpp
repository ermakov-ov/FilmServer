#include "thread_pool.h"

namespace film_server {

ThreadPool::ThreadPool(std::size_t num_threads) : m_stop(false) {
    for (std::size_t i = 0; i < num_threads; ++i) {
        m_workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();

    for (auto& worker : m_workers) {
        worker.join();
    }
}

void ThreadPool::worker_loop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this] { return m_stop || !m_tasks.empty(); });

            if (m_stop && m_tasks.empty()) {
                return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        task();
    }
}

} // namespace film_server
