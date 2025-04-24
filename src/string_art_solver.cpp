#include "string_art_solver.h"
#include "logger.h"
#include "string_color_solver.h"
#include "string_sequence.h"
#include "vec.h"

#include <cstdint>
#include <memory>
#include <numbers>
#include <utility>
#include <vector>

StringArtSolver::StringArtSolver(Img&& target_img,
                                 std::vector<Color>&& palette,
                                 Color background_color,
                                 double img_diameter_cm,
                                 uint32_t nail_count,
                                 double nail_diameter_cm,
                                 double nail_img_dist_cm,
                                 double string_diameter_cm,
                                 ThreadPool& thread_pool)
    : target_img{ std::move(target_img) }
    , palette{ std::move(palette) }
    , background_color{ background_color }
    , img_scale{ static_cast<double>(target_img.get_w()) / img_diameter_cm }
    , nail_radius{ nail_diameter_cm / 2.0 * img_scale }
    , string_radius{ string_diameter_cm / 2.0 * img_scale }
    , thread_pool{ thread_pool }
    , nail_positions{ make_nail_positions(Vec2<double>(this->target_img.get_w(), this->target_img.get_h()) / 2.0,
                                          (img_diameter_cm / 2.0 + nail_img_dist_cm) * img_scale,
                                          nail_count) }
{
}

void StringArtSolver::solve()
{
    sequence = std::make_unique<StringSequence>();
    output_img = std::make_unique<Img>(target_img.get_w(), target_img.get_h(), background_color);
    for (const Color& color : palette) {
        Logger::info(
            "Solving for color: ( {:.0f}, {:.0f}, {:.0f} )", 255 * color.r(), 255 * color.g(), 255 * color.b());
        StringColorSolver solver{ target_img, background_color, nail_positions, nail_radius, string_radius,
                                  color,      thread_pool };
        solver.solve();
        sequence->add(color, std::move(solver.get_sequence()));
        *output_img += solver.get_img();
    }
}

std::unique_ptr<StringSequence> StringArtSolver::get_sequence()
{
    return std::move(sequence);
}

std::unique_ptr<Img> StringArtSolver::get_img()
{
    return std::move(output_img);
}

const std::vector<Vec2<double>>& StringArtSolver::get_nail_positions()
{
    return nail_positions;
}

double StringArtSolver::get_nail_radius_px() const
{
    return nail_radius;
}

std::vector<Vec2<double>> StringArtSolver::make_nail_positions(Vec2<double> center, double radius, uint32_t n)
{
    std::vector<Vec2<double>> nail_positions;
    for (uint32_t i = 0; i < n; ++i) {
        double a{ i * 2 * std::numbers::pi / n };
        nail_positions.push_back(center + Vec2<double>(radius, 0.0).rotate(a));
    }
    return nail_positions;
}
