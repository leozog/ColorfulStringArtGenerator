#include "string_solver.h"
#include "line.h"
#include <numbers>

StringSolver::StringSolver(const Array2d<float>& target,
                           Array2d<float>& current,
                           const std::function<double(double)>& line_function,
                           const double line_thickness,
                           const std::vector<Vec<double, 2>>& nail_positions,
                           const double nail_radius,
                           const uint32_t start_id,
                           const uint32_t end_id)
    : target(target)
    , current(current)
    , line_function(line_function)
    , line_thickness(line_thickness)
    , nail_positions(nail_positions)
    , nail_radius(nail_radius)
    , start_id(start_id)
    , end_id(end_id)
    , mse_delta(std::nullopt)
{
    // start_pos = nail_positions[start_id / 2] +
    //             Vec<double, 2>{ nail_radius * (start_id % 2 == 0 ? -1 : 1), 0 }.rotate(std::numbers::pi / 2);
}

void StringSolver::solve()
{
    double mse_delta_tmp{ 0.0 };
    line(start_pos[0], end_pos[0], start_pos[1], end_pos[1], line_thickness, [&](int32_t x, int32_t y, double d) {
        mse_delta_tmp += std::pow(std::fmin(1.0, line_function(d) + current(x, y)) - target(x, y), 2) -
                         std::pow(current(x, y) - target(x, y), 2);
    });
    mse_delta = mse_delta_tmp;
}

void StringSolver::draw()
{
    line(start_pos[0], end_pos[0], start_pos[1], end_pos[1], line_thickness, [&](int32_t x, int32_t y, double d) {
        current(x, y) = std::fmin(1.0, current(x, y) + line_function(d));
    });
}

double StringSolver::get_mse_delta() const
{
    return mse_delta.value();
}

int32_t StringSolver::get_start_id() const
{
    return start_id;
}

int32_t StringSolver::get_end_id() const
{
    return end_id;
}