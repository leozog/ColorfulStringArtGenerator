#pragma once
#include "array2d.h"
#include "img.h"
#include "string_line.h"
#include "thread_pool.h"

#include <optional>
#include <vector>

class StringColorSolver
{
private:
    Array2d<float> target;
    Array2d<float> current;
    const std::vector<Vec2<double>>& nail_positions;
    const double nail_radius;
    const double string_radius;
    const Color color;
    ThreadPool& thread_pool;
    std::vector<StringLine> sequence;
    mutable std::optional<Img> color_img;

public:
    StringColorSolver(const Img& full_img,
                      const Color& background_color,
                      const std::vector<Vec2<double>>& nail_positions,
                      const double nail_radius,
                      const double string_radius,
                      const Color& color,
                      ThreadPool& thread_pool);
    void solve();
    double solve_step();
    std::vector<StringLine> get_sequence() const;
    Img get_img() const;
};
