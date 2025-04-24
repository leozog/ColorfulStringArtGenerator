#include "string_solver.h"
#include "line.h"

StringSolver::StringSolver(const Array2d<float>& target,
                           Array2d<float>& current,
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
            mse_delta_tmp += std::pow(std::fmin(1.0, string_function(d) + current(x, y)) - target(x, y), 2) -
                             std::pow(current(x, y) - target(x, y), 2);
        }
    });
    mse_delta = mse_delta_tmp;
}

void StringSolver::draw()
{
    line(string_line.get_start_pos(), string_line.get_end_pos(), string_radius, [&](int32_t x, int32_t y, double d) {
        if (current.is_in(x, y)) {
            current(x, y) = static_cast<float>(std::fmin(1.0, current(x, y) + string_function(d)));
        }
    });
}

double StringSolver::string_function(double d) const
{
    return (1.0 - std::fmin(1.0, d * d / string_radius)) * 0.20;
}

StringLine StringSolver::get_string_line() const
{
    return string_line;
}

double StringSolver::get_mse_delta() const
{
    return mse_delta.value();
}
