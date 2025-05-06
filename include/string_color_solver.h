#pragma once
#include "array2d.h"
#include "img.h"
#include "string_line.h"
#include "string_solver.h"
#include "thread_pool.h"

#include <memory>
#include <optional>
#include <vector>

class StringColorSolver
{
private:
    Array2d<StringSolver::pixel_t> target;
    Array2d<StringSolver::pixel_t> current;
    const std::vector<Vec2<double>>& nail_positions;
    const double nail_radius;
    const double string_radius;
    const Color color;
    ThreadPool& thread_pool;
    std::unique_ptr<std::vector<StringLine>> sequence;

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
    std::unique_ptr<std::vector<StringLine>> get_sequence();
    std::unique_ptr<Img> get_img() const;
};
