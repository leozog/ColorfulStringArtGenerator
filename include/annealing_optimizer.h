#pragma once
#include "logger.h"
#include "thread_rng.h"

#include <functional>
#include <unordered_map>

template<typename Solution, class SolutionHash = std::hash<Solution>>
class AnnealingOptimizer
{
public:
    using NeighborFunc = std::function<Solution(const Solution&)>;
    using EnergyFunc = std::function<double(const Solution&)>;

private:
    const NeighborFunc neighbor_func;
    const EnergyFunc energy_func;
    double T;
    const double cooling_rate;
    const int max_iter;
    std::unordered_map<Solution, double, SolutionHash> energy_map;

public:
    AnnealingOptimizer(NeighborFunc neighbor_func,
                       EnergyFunc energy_func,
                       double initial_temp,
                       double cooling_rate,
                       int max_iter);

    Solution optimize(const Solution& initial_solution);
};

template<typename Solution, class SolutionHash>
AnnealingOptimizer<Solution, SolutionHash>::AnnealingOptimizer(NeighborFunc neighbor_func,
                                                               EnergyFunc energy_func,
                                                               double initial_temp,
                                                               double cooling_rate,
                                                               int max_iter)
    : energy_func(energy_func)
    , neighbor_func(neighbor_func)
    , T(initial_temp)
    , cooling_rate(cooling_rate)
    , max_iter(max_iter)
{
}

template<typename Solution, class SolutionHash>
Solution AnnealingOptimizer<Solution, SolutionHash>::optimize(const Solution& initial_solution)
{
    struct SolutionEnergy
    {
        Solution solution;
        double energy;

        SolutionEnergy(AnnealingOptimizer<Solution, SolutionHash>& ao, const Solution& solution)
            : solution(solution)
            , energy(ao.energy_map.contains(solution) ? ao.energy_map[solution] : ao.energy_func(solution))
        {
            ao.energy_map[solution] = energy;
        }
    };

    SolutionEnergy current{ *this, initial_solution };
    energy_map[current.solution] = current.energy;

    SolutionEnergy best{ current };

    for (int i = 0; i < max_iter; ++i) {
        const SolutionEnergy neighbor{ *this, neighbor_func(current.solution) };

        const double delta{ neighbor.energy - current.energy };

        if (delta < 0 || ThreadRng::uniform_real(0.0, 1.0) < exp(-delta / T)) {
            current = neighbor;

            if (current.energy < best.energy) {
                best = current;
            }
        }

        T *= cooling_rate;

        Logger::info(
            "Temperature: {:.2f}, Current Energy: {:.2f}, Best Energy: {:.2f}", T, current.energy, best.energy);
    }
    return best.solution;
}