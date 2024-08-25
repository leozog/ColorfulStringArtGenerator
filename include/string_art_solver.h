#pragma once
#include "img.h"
#include "thread_pool.h"

class StringArtSolver
{
private:
    const Img input_img;
    const std::vector<Color> palette;
    const std::string output_name;
    ThreadPool pool;

    StringArtSolver(Img &&input_img, std::vector<Color> &&palette, std::string &&output_name, int n_threads);

public:
    void solve();

    class Builder
    {
    private:
        Img input_img;
        std::vector<Color> palette;
        int n_threads;
        std::string output_name;

    public:
        Builder();
        StringArtSolver build();
        Builder &set_input_img(Img &&input_img);
        Builder &set_palette(std::vector<Color> &&palette);
        Builder &set_output_name(std::string &&output_name);
        Builder &set_n_threads(int n_threads);
    };
};