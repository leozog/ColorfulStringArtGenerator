#include "string_art_solver.h"

StringArtSolver::StringArtSolver(Img&& input_img,
                                 std::vector<Color>&& palette,
                                 std::string&& output_name,
                                 ThreadPool& thread_pool)
    : input_img{ std::move(input_img) }
    , palette{ std::move(palette) }
    , output_name{ std::move(output_name) }
    , thread_pool{ thread_pool }
{
}

StringArtSolver::Builder::Builder()
    : thread_pool{ std::nullopt }
{
}

StringArtSolver StringArtSolver::Builder::build()
{
    if (input_img->get_w() <= 0 || input_img->get_h() <= 0) {
        throw std::invalid_argument("input image is empty");
    }
    if (palette.size() == 0) {
        throw std::invalid_argument("palette is empty");
    }
    if (output_name.size() == 0) {
        throw std::invalid_argument("output name is empty");
    }
    if (!thread_pool.has_value()) {
        throw std::invalid_argument("thread pool is not set");
    }
    return { std::move(input_img), std::move(palette), std::move(output_name), thread_pool.value().get() };
}

StringArtSolver::Builder& StringArtSolver::Builder::set_input_img(Img&& input_img)
{
    this->input_img = std::move(input_img);
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_palette(std::vector<Color>&& palette)
{
    this->palette = std::move(palette);
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_output_name(std::string&& output_name)
{
    this->output_name = std::move(output_name);
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_thread_pool(ThreadPool& thread_pool)
{
    this->thread_pool = std::make_optional(std::ref(thread_pool));
    return *this;
}