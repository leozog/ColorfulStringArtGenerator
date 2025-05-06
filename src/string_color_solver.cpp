#include "string_color_solver.h"
#include "img.h"
#include "logger.h"
#include "string_line.h"
#include "string_solver.h"
#include "vec.h"
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <vector>

StringColorSolver::StringColorSolver(const Img& full_img,
                                     const Color& background_color,
                                     const std::vector<Vec2<double>>& nail_positions,
                                     const double nail_radius,
                                     const double string_radius,
                                     const Color& color,
                                     ThreadPool& thread_pool)
    : target(full_img.get_w(), full_img.get_h())
    , current(full_img.get_w(), full_img.get_h())
    , nail_positions(nail_positions)
    , nail_radius(nail_radius)
    , string_radius(string_radius)
    , color{ color }
    , thread_pool{ thread_pool }
{
    constexpr double max_dist = Vec3<double>{ 1.0, 1.0, 1.0 }.len();
    std::transform(full_img.cbegin(), full_img.cend(), target.begin(), [&color, &background_color](const auto c) {
        return std::clamp(std::pow(c->dist(background_color) / max_dist, 0.8) *
                              (1.0 - std::pow(c->dist(color) / max_dist, 0.4)),
                          0.0,
                          1.0) *
               std::numeric_limits<StringSolver::pixel_t>::max();
    });
}

void StringColorSolver::solve()
{
    sequence = std::make_unique<std::vector<StringLine>>();
    int max_iterations{ 1000 };
    for (int i = 0; i < max_iterations; ++i) {
        double mse_delta = solve_step();
        Logger::debug("MSE delta: {}", mse_delta);
        if (mse_delta > -0.001) {
            return;
        }
    }
    Logger::warn("StringColorSolver: max iterations reached: {}", max_iterations);
}

double StringColorSolver::solve_step()
{
    nail_id_t last_nail_id{ sequence->empty() ? 0 : sequence->back().get_end_nail_id() };
    StringLine::Wrap last_wrap{ sequence->empty() ? StringLine::Wrap::CLOKWISE : sequence->back().get_end_wrap() };

    std::function<StringSolver(nail_id_t, StringLine::Wrap)> f =
        [this, last_nail_id, last_wrap](nail_id_t next_nail_id, StringLine::Wrap next_wrap) {
            StringSolver solver{
                target,
                current,
                string_radius,
                StringLine(nail_positions, nail_radius, string_radius, last_nail_id, last_wrap, next_nail_id, next_wrap)
            };
            solver.solve();
            return solver;
        };

    std::vector<std::future<StringSolver>> futures;
    futures.reserve(2 * (nail_positions.size() - 1));
    for (nail_id_t next_nail_id{ 0 }; next_nail_id < nail_positions.size(); ++next_nail_id) {
        if (next_nail_id == last_nail_id) {
            continue;
        }
        for (auto next_wrap : { StringLine::Wrap::CLOKWISE, StringLine::Wrap::ANTICLOCKWISE }) {
            futures.push_back(std::move(thread_pool.submit(1, f, next_nail_id, next_wrap)));
        }
    }

    std::vector<StringSolver> solvers;
    solvers.reserve(futures.size());
    for (auto& f : futures) {
        solvers.push_back(f.get());
    }

    auto best_solver = std::min_element(std::begin(solvers), std::end(solvers), [](const auto& a, const auto& b) {
        return a.get_mse_delta() < b.get_mse_delta();
    });

    sequence->push_back(best_solver->get_string_line());
    best_solver->draw();

    return best_solver->get_mse_delta();
}

std::unique_ptr<std::vector<StringLine>> StringColorSolver::get_sequence()
{
    return std::move(sequence);
}

std::unique_ptr<Img> StringColorSolver::get_img() const
{
    auto color_img{ std::make_unique<Img>(target.get_w(), target.get_h()) };
    std::transform(current.cbegin(), current.cend(), color_img->begin(), [this](const auto v) {
        return Color(color.r(), color.g(), color.b(), static_cast<float>(*v) / std::numeric_limits<uint8_t>::max());
    });
    return color_img;
}
