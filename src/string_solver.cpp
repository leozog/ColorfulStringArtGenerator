#include "string_solver.h"

StringSolver::StringSolver(const Array2d<float>& target,
                           Array2d<float>& current,
                           const std::function<double(double)>& line_function,
                           const double line_thickness,
                           const Vec2<int32_t> start,
                           const int32_t start_id,
                           const Vec2<int32_t> end,
                           const int32_t end_id)
    : target{ target }
    , current{ current }
    , line_function{ line_function }
    , line_thickness{ line_thickness }
    , start{ start }
    , start_id{ start_id }
    , end{ end }
    , end_id{ end_id }
    , mse_delta(std::nullopt)
{
}

void StringSolver::solve()
{
    double mse_delta_tmp{ 0.0 };
    line(start.x, end.x, start.y, end.y, line_thickness, [&](int32_t x, int32_t y, double D) {
        mse_delta_tmp += std::pow(std::fmin(1.0, line_function(D) + current(x, y)) - target(x, y), 2) -
                         std::pow(current(x, y) - target(x, y), 2);
    });
    mse_delta = mse_delta_tmp;
}

void StringSolver::draw()
{
    line(start.x, end.x, start.y, end.y, line_thickness, [&](int32_t x, int32_t y, double D) {
        current(x, y) = std::fmin(1.0, line_function(D) + current(x, y));
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