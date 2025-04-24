#pragma once
#include "array2d.h"
#include "string_line.h"

#include <optional>

class StringSolver
{
public:
    using pixel_t = uint8_t;

private:
    const Array2d<pixel_t>& target;
    Array2d<pixel_t>& current;
    const double string_radius;
    const StringLine string_line;
    std::optional<double> mse_delta;

public:
    StringSolver(const Array2d<pixel_t>& target,
                 Array2d<pixel_t>& current,
                 const double string_radius,
                 const StringLine&& string_line);
    void solve();
    void draw();
    [[nodiscard]] pixel_t string_function(double d) const;
    [[nodiscard]] StringLine get_string_line() const;
    [[nodiscard]] double get_mse_delta() const;
};