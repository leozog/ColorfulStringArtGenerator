#include "img.h"
#include "string_art_solver.h"

StringArtSolver::Builder::Builder()
    : background_color{ 1.0, 1.0, 1.0 } // default values
    , img_diameter_cm{ 0.0 }
    , nail_count{ 0 }
    , nail_diameter_cm{ 0.15 }
    , nail_img_dist_cm{ 1.0 }
    , string_diameter_cm{ 0.05 }
    , thread_pool{ std::nullopt }
{
}

StringArtSolver StringArtSolver::Builder::build()
{
    if (target_img.get_w() <= 0 || target_img.get_h() <= 0) {
        throw std::invalid_argument("input image is empty");
    }
    if (palette.size() == 0) {
        throw std::invalid_argument("palette is empty");
    }
    if (img_diameter_cm <= 0) {
        throw std::invalid_argument("image diameter must be greater than 0");
    }
    if (nail_count <= 0) {
        throw std::invalid_argument("nail count must be greater than 0");
    }
    if (nail_diameter_cm <= 0) {
        throw std::invalid_argument("nail diameter must be greater than 0");
    }
    if (nail_diameter_cm >= img_diameter_cm) {
        throw std::invalid_argument("nail diameter must be less than image diameter");
    }
    if (nail_img_dist_cm <= 0) {
        throw std::invalid_argument("nail-image distance must be greater than 0");
    }
    if (string_diameter_cm <= 0) {
        throw std::invalid_argument("string diameter must be greater than 0");
    }
    if (string_diameter_cm >= img_diameter_cm) {
        throw std::invalid_argument("string diameter must be less than image diameter");
    }
    if (!thread_pool.has_value()) {
        throw std::invalid_argument("thread pool is not set");
    }
    return { std::move(target_img), std::move(palette), background_color,   img_diameter_cm,          nail_count,
             nail_diameter_cm,      nail_img_dist_cm,   string_diameter_cm, thread_pool.value().get() };
}

StringArtSolver::Builder& StringArtSolver::Builder::set_target_img(Img&& target_img)
{
    this->target_img = std::move(target_img);
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_palette(std::vector<Color>&& palette)
{
    this->palette = std::move(palette);
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_background_color(Color background_color)
{
    this->background_color = background_color;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_img_diameter_cm(double diameter)
{
    this->img_diameter_cm = diameter;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_nail_count(uint32_t count)
{
    this->nail_count = count;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_nail_diameter_cm(double diameter)
{
    this->nail_diameter_cm = diameter;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_nail_img_dist_cm(double distance)
{
    this->nail_img_dist_cm = distance;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_string_diameter_cm(double diameter)
{
    this->string_diameter_cm = diameter;
    return *this;
}

StringArtSolver::Builder& StringArtSolver::Builder::set_thread_pool(ThreadPool& thread_pool)
{
    this->thread_pool = std::make_optional(std::ref(thread_pool));
    return *this;
}