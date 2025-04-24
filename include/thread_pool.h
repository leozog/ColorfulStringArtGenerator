#pragma once
#include "priority_queue.h"
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <vector>

class ThreadPool
{
private:
    class AbstractTask
    {
        int priority;

    public:
        AbstractTask(int priority);
        AbstractTask(const AbstractTask&) = delete;
        AbstractTask& operator=(const AbstractTask&) = delete;
        AbstractTask(AbstractTask&&) = default;
        AbstractTask& operator=(AbstractTask&&) = default;
        virtual ~AbstractTask() = default;
        [[nodiscard]] int get_priority() const { return priority; }
        virtual void run() = 0;
        bool operator<(const AbstractTask& other) const;
    };

    template<class R, class... Args>
    class Task : public AbstractTask
    {
        const std::function<R(Args...)> func;
        std::promise<R> promise;
        std::tuple<Args...> args;

    public:
        Task(int priority, std::function<R(Args...)> func, Args&&... args);
        void run() override;
        std::future<R> get_future();
    };

    PriorityQueue<std::unique_ptr<AbstractTask>,
                  std::vector<std::unique_ptr<AbstractTask>>,
                  std::function<bool(const std::unique_ptr<AbstractTask>&, const std::unique_ptr<AbstractTask>&)>>
        tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::vector<std::jthread> threads;

public:
    constexpr static int MAX_N_THREADS = 0;
    ThreadPool(unsigned int n_threads = MAX_N_THREADS);
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    ~ThreadPool();
    template<class R, class... Args>
    std::future<R> submit(int priority, std::function<R(Args...)> func, Args... args);
    [[nodiscard]] unsigned int get_n_threads() const;

private:
    void thread_loop(std::stop_token stop_token);
};

// ThreadPool
template<class R, class... Args>
std::future<R> ThreadPool::submit(int priority, std::function<R(Args...)> func, Args... args)
{
    auto t = std::make_unique<Task<R, Args...>>(priority, func, std::forward<Args>(args)...);
    auto fut = t->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.push(std::move(t));
    }
    tasks_cv.notify_one();
    return fut;
}
// ThreadPool

// ThreadPool::Task
template<class R, class... Args>
ThreadPool::Task<R, Args...>::Task(int priority, std::function<R(Args...)> func, Args&&... args)
    : AbstractTask(priority)
    , func{ std::move(func) }
    , args{ std::forward<Args>(args)... }
{
}

template<class R, class... Args>
void ThreadPool::Task<R, Args...>::run()
{
    try {
        if constexpr (std::is_void_v<R>) {
            std::apply(func, args);
            promise.set_value();
        } else {
            promise.set_value(std::move(std::apply(func, args)));
        }
    } catch (...) {
        promise.set_exception(std::current_exception());
    }
}

template<class R, class... Args>
std::future<R> ThreadPool::Task<R, Args...>::get_future()
{
    return promise.get_future();
}
// ThreadPool::Task