#include "thread_pool.h"
#include <stop_token>

// ThreadPool
ThreadPool::ThreadPool(unsigned int n_threads)
    : tasks([](const std::unique_ptr<AbstractTask>& a, const std::unique_ptr<AbstractTask>& b) {
        return a->get_priority() < b->get_priority();
    })
{
    n_threads = n_threads == MAX_N_THREADS ? std::thread::hardware_concurrency() : n_threads;
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        for (unsigned int i = 0; i < n_threads; i++) {
            threads.emplace_back([this](std::stop_token stop_token) { thread_loop(std::move(stop_token)); });
        }
    }
}

ThreadPool::~ThreadPool()
{
    for (auto& t : threads) {
        t.request_stop();
    }
    tasks_cv.notify_all();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

unsigned int ThreadPool::get_n_threads() const
{
    return static_cast<unsigned int>(threads.size());
}

void ThreadPool::thread_loop(std::stop_token stop_token)
{
    while (!stop_token.stop_requested()) {
        std::unique_ptr<AbstractTask> t;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            tasks_cv.wait(lock, [this, &stop_token] { return stop_token.stop_requested() || !tasks.empty(); });
            if (stop_token.stop_requested() || tasks.empty()) {
                continue;
            }
            t = std::move(tasks.top_pop());
        }
        t->run();
    }
}
// ThreadPool

// ThreadPool::AbstractTask
ThreadPool::AbstractTask::AbstractTask(int priority)
    : priority{ priority }
{
}

bool ThreadPool::AbstractTask::operator<(const AbstractTask& other) const
{
    return priority < other.priority;
}
// ThreadPool::AbstractTask