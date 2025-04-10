#pragma once
#include "array2d.h"
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
    const std::vector<Vec2<double>> nail_positions;
    const double nail_radius;
    const uint32_t start_id, end_id;
    Vec2<int32_t> start_pos, end_pos;
    std::optional<double> mse_delta;

public:
    StringSolver(const Array2d<float>& target,
                 Array2d<float>& current,
                 const std::function<double(double)>& line_function,
                 const double line_thickness,
                 const std::vector<Vec2<double>>& nail_positions,
                 const double nail_radius,
                 const uint32_t start_id,
                 const uint32_t end_id);
    void solve();
    void draw();
    [[nodiscard]] double get_mse_delta() const;
    [[nodiscard]] uint32_t get_start_id() const;
    [[nodiscard]] uint32_t get_end_id() const;
};