#pragma once
#include "array2d.h"
#include "img.h"
#include "thread_pool.h"

#include <optional>

class StringColorSolver
{
private:
    Array2d<float> target;
    Array2d<float> current;
    const Color color;
    mutable std::optional<Img> color_img;
    const std::vector<Vec2<double>>& nail_positions;
    ThreadPool& thread_pool;

public:
    StringColorSolver(const Img& full_img,
                      const Color& color,
                      const std::vector<Vec2<double>>& nail_positions,
                      ThreadPool& thread_pool);
    void solve(int task_quota);
    Img get_img() const;
};
