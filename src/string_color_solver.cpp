#include "string_color_solver.h"

StringColorSolver::StringColorSolver(const Img& full_img,
                                     const Color color,
                                     const std::vector<Vec<double, 2>>& nail_positions,
                                     ThreadPool& thread_pool)
    : target(full_img->get_w(), full_img->get_h())
    , current(full_img->get_w(), full_img->get_h())
    , color{ color }
    , color_img(std::nullopt)
    , nail_positions(nail_positions)
    , thread_pool{ thread_pool }
{
    std::transform(
        full_img->begin(), full_img->end(), target.begin(), [this](const Color& c) { return c.dist(this->color); });
}

void StringColorSolver::solve() {}

Img StringColorSolver::get_img() const
{
    if (!color_img.has_value()) {
        Img color_img_tmp(current.get_w(), current.get_h());

        std::transform(current.begin(), current.end(), color_img_tmp->begin(), [this](float v) {
            return Color(color.r(), color.g(), color.b(), v);
        });

        color_img = std::make_optional(std::move(color_img_tmp));
    }
    return color_img.value();
}
