#include "thread_rng.h"

thread_local std::mt19937 ThreadRng::rng{ std::random_device{}() };

double ThreadRng::uniform_real(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

int ThreadRng::uniform_int(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}