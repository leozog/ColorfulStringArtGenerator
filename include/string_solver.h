#pragma once
#include "array2d.h"
#include "line.h"
#include "vec.h"

#include <functional>
#include <optional>
#include <stdint.h>

class StringSolver
{
private:
    const Array2d<float>& target;
    Array2d<float>& current;
    const std::function<double(double)>& line_function;
    const double line_thickness;
    const Vec2<int32_t> start, end;
    const int32_t start_id, end_id;
    std::optional<double> mse_delta;

public:
    StringSolver(const Array2d<float>& target,
                 Array2d<float>& current,
                 const std::function<double(double)>&,
                 const double line_thickness,
                 const Vec2<int32_t> start,
                 const int32_t start_id,
                 const Vec2<int32_t> end,
                 const int32_t end_id);
    void solve();
    void draw();
    double get_mse_delta() const;
    int32_t get_start_id() const;
    int32_t get_end_id() const;
};