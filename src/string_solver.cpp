#include "string_solver.h"
#include "line.h"
#include <cstdint>
#include <limits>

StringSolver::StringSolver(const Array2d<pixel_t>& target,
                           Array2d<pixel_t>& current,
                           const double string_radius,
                           const StringLine&& string_line)
    : target(target)
    , current(current)
    , string_radius(string_radius)
    , string_line(string_line)
    , mse_delta(std::nullopt)
{
}

void StringSolver::solve()
{
    const double min_string_length{ 100.0 }; // TODO: make this a parameter
    if (string_line.get_length() < min_string_length) {
        mse_delta = std::numeric_limits<double>::max();
        return;
    }

    double mse_delta_tmp{ 0.0 };
    line(string_line.get_start_pos(), string_line.get_end_pos(), string_radius, [&](int32_t x, int32_t y, double d) {
        if (target.is_in(x, y)) {
            mse_delta_tmp +=
                std::pow(std::min(static_cast<int32_t>(std::numeric_limits<pixel_t>::max()),
                                  static_cast<int32_t>(string_function(d)) + static_cast<int32_t>(current(x, y))) -
                             static_cast<int32_t>(target(x, y)),
                         2) -
                std::pow(static_cast<int32_t>(current(x, y)) - static_cast<int32_t>(target(x, y)), 2);
        }
    });
    mse_delta = mse_delta_tmp;
}

void StringSolver::draw()
{
    line(string_line.get_start_pos(), string_line.get_end_pos(), string_radius, [&](int32_t x, int32_t y, double d) {
        if (current.is_in(x, y)) {
            current(x, y) = std::min(static_cast<int32_t>(std::numeric_limits<pixel_t>::max()),
                                     static_cast<int32_t>(string_function(d)) + static_cast<int32_t>(current(x, y)));
        }
    });
}

StringSolver::pixel_t StringSolver::string_function(double d) const
{
    return static_cast<pixel_t>((1.0 - std::fmin(1.0, d * d / string_radius)) * 0.20 *
                                std::numeric_limits<pixel_t>::max());
}

StringLine StringSolver::get_string_line() const
{
    return string_line;
}

double StringSolver::get_mse_delta() const
{
    return mse_delta.value();
}
