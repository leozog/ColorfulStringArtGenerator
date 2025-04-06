#pragma once
#include "img.h"
#include "thread_pool.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

class StringArtSolver
{
private:
    const Img input_img;
    const std::vector<Color> palette;
    const std::string output_name;
    ThreadPool& thread_pool;

    StringArtSolver(Img&& input_img, std::vector<Color>&& palette, std::string&& output_name, ThreadPool& thread_pool);

public:
    void solve();

    class Builder
    {
    private:
        Img input_img;
        std::vector<Color> palette;
        std::string output_name;
        std::optional<std::reference_wrapper<ThreadPool>> thread_pool;

    public:
        Builder();
        StringArtSolver build();
        Builder& set_input_img(Img&& input_img);
        Builder& set_palette(std::vector<Color>&& palette);
        Builder& set_output_name(std::string&& output_name);
        Builder& set_thread_pool(ThreadPool& thread_pool);
    };
};