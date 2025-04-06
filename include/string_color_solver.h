#pragma once
#include "array2d.h"
#include "img.h"
#include "thread_pool.h"

#include <optional>

class StringColorSolver
{
private:
    Array2d<float> img;
    const Color color;
    std::optional<Img> color_img;
    ThreadPool& thread_pool;

public:
    StringColorSolver(const Img& full_img, const Color color, ThreadPool& thread_pool);
    void solve();
    Img get_img() const;
};

StringColorSolver::StringColorSolver(const Img& full_img, const Color color, ThreadPool& thread_pool)
    : img(full_img->get_w(), full_img->get_h())
    , color{ color }
    , color_img(std::nullopt)
    , thread_pool{ thread_pool }
{
}

void StringColorSolver::solve()
{
    Img color_img_tmp(img.get_w(), img.get_h());
    auto color_img_tmp_it = color_img_tmp->begin();

    for (Array2d<float>::Element value : img) {
        *color_img_tmp_it++ = Color(color.r, color.g, color.b, *value);
    }

    color_img = std::make_optional(std::move(color_img_tmp));
}

Img StringColorSolver::get_img() const
{
    return color_img.value();
}