#pragma once
#include "array2d.h"
#include "vec.h"
#include "line.h"

#include <functional>
#include <stdint.h>
#include <optional>

class StringSolver
{
private:
    const Array2d<float> &img;
    const Vec2<int32_t> start, end;
    const std::function<double(double)> &line_function;
    const double line_thickness;
    std::optional<double> mse_delta;

public:
    StringSolver(const Array2d<float> &img,
                 const std::function<double(double)>&,
                 const double line_thickness,
                 const Vec2<int32_t> start,
                 const Vec2<int32_t> end);
    void solve();
    double get_mse_delta() const;
    Vec2<int32_t> get_start() const;
    Vec2<int32_t> get_end() const;
};