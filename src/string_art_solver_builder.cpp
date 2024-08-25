#include "string_art_solver.h"

StringArtSolver::Builder::Builder() : input_img{}, palette{}, output_name{""}, n_threads{-1}
{
}

StringArtSolver StringArtSolver::Builder::build()
{
    if (input_img->get_w() <= 0 || input_img->get_h() <= 0)
        throw std::invalid_argument("input image is empty");
    if (palette.size() == 0)
        throw std::invalid_argument("palette is empty");
    if (n_threads <= -1 || n_threads == 0)
        throw std::invalid_argument("invalid number of threads: " + std::to_string(n_threads));
    return StringArtSolver(
        std::move(input_img),
        std::move(palette),
        std::move(output_name),
        n_threads);
}

StringArtSolver::Builder &StringArtSolver::Builder::set_input_img(Img &&input_img)
{
    this->input_img = std::move(input_img);
    return *this;
}

StringArtSolver::Builder &StringArtSolver::Builder::set_palette(std::vector<Color> &&palette)
{
    this->palette = std::move(palette);
    return *this;
}

StringArtSolver::Builder &StringArtSolver::Builder::set_output_name(std::string &&output_name)
{
    this->output_name = std::move(output_name);
    return *this;
}

StringArtSolver::Builder &StringArtSolver::Builder::set_n_threads(int n_threads)
{
    this->n_threads = n_threads;
    return *this;
}