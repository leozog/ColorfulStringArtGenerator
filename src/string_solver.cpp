#include "string_solver.h"

StringSolver::StringSolver(const Array2d<float> &img,
                           const std::function<double(double)> &line_function,
                           const double line_thickness,
                           const Vec2<int32_t> start,
                           const Vec2<int32_t> end)
    : img{img}, line_function{line_function}, line_thickness{line_thickness}, start{start}, end{end}, mse_delta(std::nullopt)
{
}

void StringSolver::solve()
{
    double mse_delta_tmp = 0;
    line(start.x, end.x, start.y, end.y, line_thickness, [&](int32_t x, int32_t y, double D)
         { if (img.is_in(x, y)) mse_delta_tmp += std::pow(line_function(D) - img(x, y), 2); });
    mse_delta = mse_delta_tmp;
}

double StringSolver::get_mse_delta() const { return mse_delta.value(); }

Vec2<int32_t> StringSolver::get_start() const { return start; }

Vec2<int32_t> StringSolver::get_end() const { return end; }