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
    class task
    {
        const int priority;

    public:
        task(int priority) : priority(priority) {}
        virtual void run() {}
        bool operator<(const task &t) const
        {
            return priority < t.priority;
        }
    };

    template <class R, class... Args>
    class specific_task : public task
    {
        const std::function<R(Args...)> f;
        std::promise<R> p;
        const std::tuple<Args...> args;

    public:
        specific_task(int priority, std::function<R(Args...)> f, Args... args) : task(priority), f(f), args(args...) {}
        void run() override
        {
            std::apply([&](Args... args)
                       { p.set_value(f(args...)); },
                       args);
        }
        std::future<R> get_future()
        {
            return p.get_future();
        }
    };

    std::priority_queue<std::shared_ptr<task>> tasks;
    std::mutex tasks_mutex;
    std::vector<std::jthread> threads;

public:
    thread_pool(size_t n_threads);
    template <class R, class... Args>
    std::future<R> submit(int priority, std::function<R(Args...)> f, Args... args);

private:
    void thread_loop(std::stop_token stop_token);
};

thread_pool::thread_pool(size_t n_threads = std::thread::hardware_concurrency())
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    for (size_t i = 0; i < n_threads; i++)
    {
        threads.emplace_back(std::jthread(&thread_pool::thread_loop, this));
    }
}

template <class R, class... Args>
std::future<R> thread_pool::submit(int priority, std::function<R(Args...)> f, Args... args)
{
    auto t = std::make_shared<specific_task<R, Args...>>(priority, f, args...);
    auto fut = t->get_future();
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        tasks.push(std::move(t));
    }
    return fut;
}

void thread_pool::thread_loop(std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        std::shared_ptr<task> t;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            if (tasks.empty())
                continue;
            t = tasks.top();
            tasks.pop();
        }
        t->run();
    }
}