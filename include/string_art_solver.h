#pragma once
#include "img.h"
#include "string_line.h"
#include "thread_pool.h"

#include <functional>
#include <vector>

class StringArtSolver
{
private:
    const Img target_img;
    const std::vector<Color> palette;
    const Color background_color;
    const double img_scale;
    const double nail_radius;
    const double string_radius;
    ThreadPool& thread_pool;
    const std::vector<Vec2<double>> nail_positions;
    std::vector<StringLine> sequence;
    Img output_img;

    StringArtSolver(Img&& target_img,
                    std::vector<Color>&& palette,
                    Color background_color,
                    double img_diameter_cm,
                    uint32_t nail_count,
                    double nail_diameter_cm,
                    double nail_img_dist_cm,
                    double string_diameter_cm,
                    ThreadPool& thread_pool);

public:
    class Builder;

    void solve();
    [[nodiscard]] std::vector<StringLine> get_sequence() const;
    [[nodiscard]] Img get_img() const;

    const std::vector<Vec2<double>>& get_nail_positions();
    double get_nail_radius_px() const;

private:
    static std::vector<Vec2<double>> make_nail_positions(Vec2<double> center, double radius, uint32_t n);
};

class StringArtSolver::Builder
{
private:
    Img target_img;
    std::vector<Color> palette;
    Color background_color;
    double img_diameter_cm;
    uint32_t nail_count;
    double nail_diameter_cm;
    double nail_img_dist_cm;
    double string_diameter_cm;
    std::optional<std::reference_wrapper<ThreadPool>> thread_pool;

public:
    Builder();
    StringArtSolver build();
    Builder& set_target_img(Img&& target_img);
    Builder& set_palette(std::vector<Color>&& palette);
    Builder& set_background_color(Color background_color);
    Builder& set_img_diameter_cm(double diameter);
    Builder& set_nail_count(uint32_t count);
    Builder& set_nail_diameter_cm(double diameter);
    Builder& set_nail_img_dist_cm(double distance);
    Builder& set_string_diameter_cm(double diameter);
    Builder& set_thread_pool(ThreadPool& thread_pool);
};