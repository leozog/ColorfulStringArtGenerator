#include "string_art_solver.h"

#include <iostream>
#include <algorithm>


void StringArtSolver::solve()
{
    std::atomic<int> counter = 0;
    std::function<void()> f = [&counter](){
        std::cout<<counter++<<std::endl;
    };
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 100; i++)
        futures.push_back(thread_pool.submit(100-i, f));
    for (auto &f : futures)
        f.wait();
}