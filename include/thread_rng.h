#pragma once
#include <random>

class ThreadRng
{
public:
    static thread_local std::mt19937 rng;

    static double uniform_real(double min, double max);
    static int uniform_int(int min, int max);
};