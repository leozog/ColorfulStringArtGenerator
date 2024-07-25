#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <future>

class ThreadPool
{
private:
    class AbstractTask
    {
        const int priority;

    public:
        AbstractTask(int priority);
        virtual void run() = 0;
        virtual ~AbstractTask() = default;
        bool operator<(const AbstractTask &other) const;
    };

    template <class R, class... Args>
    class Task : public AbstractTask
    {
        const std::function<R(Args...)> func;
        std::promise<R> promise;
        std::tuple<Args...> args;

    public:
        Task(int priority, std::function<R(Args...)> func, Args &&...args);
        void run() override;
        std::future<R> get_future();
    };

    std::priority_queue<std::shared_ptr<AbstractTask>> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::vector<std::jthread> threads;

public:
    ThreadPool(size_t n_threads);
    ~ThreadPool();
    template <class R, class... Args>
    std::future<R> submit(int priority, std::function<R(Args...)> f, Args &&...args);

private:
    void thread_loop(std::stop_token stop_token);
};

// ThreadPool
ThreadPool::ThreadPool(size_t n_threads)
{
    n_threads = n_threads == -1 ? std::thread::hardware_concurrency() : std::max<size_t>(1, n_threads);
    std::unique_lock<std::mutex> lock(tasks_mutex);
    for (size_t i = 0; i < n_threads; i++)
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

template <class R, class... Args>
std::future<R> ThreadPool::submit(int priority, std::function<R(Args...)> f, Args &&...args)
{
    auto t = std::make_shared<Task<R, Args...>>(priority, f, std::forward<Args>(args)...);
    auto fut = t->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.push(std::move(t));
        tasks_cv.notify_one();
    }
    return fut;
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

// ThreadPool::Task
template <class R, class... Args>
ThreadPool::Task<R, Args...>::Task(int priority, std::function<R(Args...)> func, Args &&...args)
    : AbstractTask(priority), func{std::move(func)}, args{std::forward<Args>(args)...} {}

template <class R, class... Args>
void ThreadPool::Task<R, Args...>::run()
{
    std::apply([this](Args &&...args)
               { promise.set_value(func(std::forward<Args>(args)...)); },
               std::move(args));
}

template <class R, class... Args>
std::future<R> ThreadPool::Task<R, Args...>::get_future()
{
    return promise.get_future();
}
// ThreadPool::Task