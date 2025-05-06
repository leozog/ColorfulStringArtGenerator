#include "string_art_solver.h"
#include "annealing_optimizer.h"
#include "color.h"
#include "logger.h"
#include "string_color_solver.h"
#include "string_sequence.h"
#include "thread_rng.h"
#include "vec.h"

#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <memory>
#include <numbers>
#include <utility>
#include <vector>

StringArtSolver::StringArtSolver(Img&& target_img,
                                 std::vector<Color>&& palette,
                                 Color background_color,
                                 double img_diameter_cm,
                                 uint32_t nail_count,
                                 double nail_diameter_cm,
                                 double nail_img_dist_cm,
                                 double string_diameter_cm,
                                 ThreadPool& thread_pool)
    : target_img{ std::move(target_img) }
    , palette{ std::move(palette) }
    , background_color{ background_color }
    , img_scale{ static_cast<double>(target_img.get_w()) / img_diameter_cm }
    , nail_radius{ nail_diameter_cm / 2.0 * img_scale }
    , string_radius{ string_diameter_cm / 2.0 * img_scale }
    , thread_pool{ thread_pool }
    , nail_positions{ make_nail_positions(Vec2<double>(this->target_img.get_w(), this->target_img.get_h()) / 2.0,
                                          (img_diameter_cm / 2.0 + nail_img_dist_cm) * img_scale,
                                          nail_count) }
{
}

void StringArtSolver::solve()
{
    std::vector<ColorSolverResult> color_solver_results = solve_colors();
    if (color_solver_results.size() > 1) {
        rearrange_colors(color_solver_results);
    }

    sequence = std::make_unique<StringSequence>();
    output_img = std::make_unique<Img>(target_img.get_w(), target_img.get_h(), background_color);

    for (ColorSolverResult& result : color_solver_results) {
        sequence->add(result.color, std::move(result.sequence));
        *output_img += *result.img;
    }
}

std::unique_ptr<StringSequence> StringArtSolver::get_sequence()
{
    return std::move(sequence);
}

std::unique_ptr<Img> StringArtSolver::get_img()
{
    return std::move(output_img);
}

const std::vector<Vec2<double>>& StringArtSolver::get_nail_positions()
{
    return nail_positions;
}

double StringArtSolver::get_nail_radius_px() const
{
    return nail_radius;
}

std::vector<StringArtSolver::ColorSolverResult> StringArtSolver::solve_colors()
{
    ThreadPool solver_thread_pool(4);
    std::function<ColorSolverResult(Color)> f = [this](Color color) -> ColorSolverResult {
        Logger::info(
            "Solving for color: ( {:.0f}, {:.0f}, {:.0f} )", 255 * color.r(), 255 * color.g(), 255 * color.b());
        StringColorSolver solver{ target_img, background_color, nail_positions, nail_radius, string_radius,
                                  color,      thread_pool };
        solver.solve();
        return { color, std::move(solver.get_sequence()), std::move(solver.get_img()) };
    };

    std::vector<std::future<ColorSolverResult>> futures;
    futures.reserve(palette.size());
    for (const Color& color : palette) {
        futures.push_back(solver_thread_pool.submit(0, f, color));
    }

    std::vector<ColorSolverResult> results;
    results.reserve(futures.size());
    for (auto& f : futures) {
        results.push_back(f.get());
    }

    return results;
}

void StringArtSolver::rearrange_colors(std::vector<ColorSolverResult>& color_solver_results)
{
    Logger::info("Rearranging colors");

    using Solution = std::vector<int>;
    using NeighborFunc = std::function<Solution(const Solution&)>;
    using EnergyFunc = std::function<double(const Solution&)>;

    struct SolutionHash
    {
        size_t operator()(const Solution& solution) const
        {
            size_t hash{ 0 };
            for (int i : solution) {
                hash ^= std::hash<int>{}(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    NeighborFunc neighbor_func = [](const Solution& solution) -> Solution {
        Solution neighbor = solution;
        const int max = static_cast<int>(neighbor.size() - 1);
        const int a = ThreadRng::uniform_int(0, max);
        const int b = ThreadRng::uniform_int(0, max);
        std::swap(neighbor[a], neighbor[b]);
        return neighbor;
    };

    EnergyFunc energy_func = [this, &color_solver_results](const Solution& solution) -> double {
        std::function<double(Img::ConstRegion region)> f =
            [this, &color_solver_results, solution](Img::ConstRegion target_img_region) {
                double mse{ 0.0 };
                std::for_each(target_img_region.cbegin(), target_img_region.cend(), [&](const auto& target) {
                    Color color = background_color;
                    for (int i : solution) {
                        const Img& result_img{ *color_solver_results[i].img };
                        color += result_img(target.get_x(), target.get_y());
                    }

                    const Color diff{ *target - color };
                    mse += diff.r() * diff.r() + diff.g() * diff.g() + diff.b() * diff.b();
                });
                return mse;
            };

        const uint32_t n_tasks{ thread_pool.get_n_threads() * 64 };
        const size_t rows_per_task{ target_img.get_h() / n_tasks > 0 ? target_img.get_h() / n_tasks : 1 };

        std::vector<std::future<double>> futures;
        futures.reserve(n_tasks);
        for (size_t y_start{ 0 }; y_start < target_img.get_h(); y_start += rows_per_task) {
            size_t y_end{ std::min(y_start + rows_per_task, target_img.get_h()) };
            futures.push_back(thread_pool.submit(1, f, target_img.get_cregion(0, y_start, target_img.get_w(), y_end)));
        }

        double mse{ 0.0 };
        for (auto& f : futures) {
            mse += f.get();
        }

        return mse;
    };

    const double initial_temp{ 100.0 };
    const double cooling_rate{ 0.99 };
    const int max_iter{ 500 };

    AnnealingOptimizer<Solution, SolutionHash> optimizer{
        neighbor_func, energy_func, initial_temp, cooling_rate, max_iter
    };

    Solution initial_solution;
    for (int i = 0; i < color_solver_results.size(); ++i) {
        initial_solution.push_back(i);
    }

    Solution optimized_solution = optimizer.optimize(initial_solution);

    std::vector<ColorSolverResult> rearranged_results;
    rearranged_results.reserve(color_solver_results.size());
    for (int i : optimized_solution) {
        rearranged_results.push_back(std::move(color_solver_results[i]));
    }
    color_solver_results = std::move(rearranged_results);
}

std::vector<Vec2<double>> StringArtSolver::make_nail_positions(Vec2<double> center, double radius, uint32_t n)
{
    std::vector<Vec2<double>> nail_positions;
    for (uint32_t i = 0; i < n; ++i) {
        double a{ i * 2 * std::numbers::pi / n };
        nail_positions.push_back(center + Vec2<double>(radius, 0.0).rotate(a));
    }
    return nail_positions;
}
