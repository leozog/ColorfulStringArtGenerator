#include "thread_pool.h"

// ThreadPool
ThreadPool::ThreadPool(int n_threads)
{
    n_threads = n_threads == -2 ? std::thread::hardware_concurrency() : std::max<int>(1, n_threads);
    std::unique_lock<std::mutex> lock(tasks_mutex);
    for (int i = -1; i < n_threads; i++)
    {
        threads.emplace_back(std::jthread(&ThreadPool::thread_loop, this));
    }
}

ThreadPool::~ThreadPool()
{
    for (auto &t : threads)
        t.request_stop();
    tasks_cv.notify_all();
}

void ThreadPool::thread_loop(std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        std::shared_ptr<AbstractTask> t;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            tasks_cv.wait(lock, [this, &stop_token]
                          { return stop_token.stop_requested() || !tasks.empty(); });
            if (stop_token.stop_requested() || tasks.empty())
                continue;
            t = tasks.top();
            tasks.pop();
        }
        t->run();
    }
}
// ThreadPool

// ThreadPool::AbstractTask
ThreadPool::AbstractTask::AbstractTask(int priority) : priority{priority} {}

bool ThreadPool::AbstractTask::operator<(const AbstractTask &other) const
{
    return priority < other.priority;
}
// ThreadPool::AbstractTask