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
    ThreadPool(int n_threads);
    ~ThreadPool();
    template <class R, class... Args>
    std::future<R> submit(int priority, std::function<R(Args...)> f, Args &&...args);
    int get_n_threads() const;

private:
    void thread_loop(std::stop_token stop_token);
};

// ThreadPool
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
// ThreadPool

// ThreadPool::Task
template <class R, class... Args>
ThreadPool::Task<R, Args...>::Task(int priority, std::function<R(Args...)> func, Args &&...args)
    : AbstractTask(priority), func{std::move(func)}, args{std::forward<Args>(args)...} {}

template <class R, class... Args>
void ThreadPool::Task<R, Args...>::run()
{
    if constexpr (std::is_void_v<R>)
    {
        std::apply(func, args);
        promise.set_value();
    }
    else
    {
        promise.set_value(std::apply(func, args));
    }
}

template <class R, class... Args>
std::future<R> ThreadPool::Task<R, Args...>::get_future()
{
    return promise.get_future();
}
// ThreadPool::Task