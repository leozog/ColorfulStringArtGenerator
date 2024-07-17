#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <future>

class thread_pool
{
private:
    class task_base
    {
        const int priority;

    public:
        task_base(int priority) : priority(priority) {}
        virtual void run() = 0;
        virtual ~task_base() = default;
        bool operator<(const task_base &other) const
        {
            return priority < other.priority;
        }
    };

    template <class R, class... Args>
    class task : public task_base
    {
        const std::function<R(Args...)> func;
        std::promise<R> promise;
        std::tuple<Args...> args;

    public:
        task(int priority, std::function<R(Args...)> func, Args &&...args) : task_base(priority), func(std::move(func)), args(std::forward<Args>(args)...) {}
        void run() override
        {
            std::apply([this](Args &&...args)
                       { promise.set_value(func(std::forward<Args>(args)...)); },
                       std::move(args));
        }
        std::future<R> get_future()
        {
            return promise.get_future();
        }
    };

    std::priority_queue<std::shared_ptr<task_base>> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::vector<std::jthread> threads;

public:
    thread_pool(size_t n_threads);
    ~thread_pool();
    template <class R, class... Args>
    std::future<R> submit(int priority, std::function<R(Args...)> f, Args &&...args);

private:
    void thread_loop(std::stop_token stop_token);
};

thread_pool::thread_pool(size_t n_threads = std::thread::hardware_concurrency())
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    for (size_t i = 0; i < n_threads; i++)
    {
        threads.emplace_back(std::jthread(&thread_pool::thread_loop, this));
    }
}

thread_pool::~thread_pool()
{
    for (auto &t : threads)
        t.request_stop();
    tasks_cv.notify_all();
}

template <class R, class... Args>
std::future<R> thread_pool::submit(int priority, std::function<R(Args...)> f, Args &&...args)
{
    auto t = std::make_shared<task<R, Args...>>(priority, f, std::forward<Args>(args)...);
    auto fut = t->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.push(std::move(t));
        tasks_cv.notify_one();
    }
    return fut;
}

void thread_pool::thread_loop(std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        std::shared_ptr<task_base> t;
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