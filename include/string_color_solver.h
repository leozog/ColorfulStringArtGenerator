#pragma once
#include "array2d.h"
#include "img.h"
#include "thread_pool.h"

#include <algorithm> // For std::transform
#include <optional>
#include <utility>

class StringColorSolver
{
private:
    Array2d<float> target;
    Array2d<float> current;
    const Color color;
    mutable std::optional<Img> color_img;
    const std::vector<Vec<double, 2>>& nail_positions;
    ThreadPool& thread_pool;

public:
    StringColorSolver(const Img& full_img,
                      const Color color,
                      const std::vector<Vec<double, 2>>& nail_positions,
                      ThreadPool& thread_pool);
    void solve();
    Img get_img() const;
};
