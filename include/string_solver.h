#pragma once
#include "array2d.h"
#include "string_line.h"

#include <optional>

class StringSolver
{
private:
    const Array2d<float>& target;
    Array2d<float>& current;
    const double string_radius;
    const StringLine string_line;
    std::optional<double> mse_delta;

public:
    StringSolver(const Array2d<float>& target,
                 Array2d<float>& current,
                 const double string_radius,
                 const StringLine&& string_line);
    void solve();
    void draw();
    [[nodiscard]] double string_function(double d) const;
    [[nodiscard]] StringLine get_string_line() const;
    [[nodiscard]] double get_mse_delta() const;
};