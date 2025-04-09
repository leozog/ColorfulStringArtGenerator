#include "array2d.h"
#include "img.h"
#include "line.h"
#include "string_art_solver.h"
#include "thread_pool.h"
#include "vec.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <vector>

double string_fn(double d)
{
    return std::max(std::min(1., 1. - d * 1.), 0.);
}

void draw_full_circle(Img& img, Vec2<double> center, double radius, Color color)
{
    if (radius > 0) {
        double abs_radius{ std::fabs(radius) };
        auto img_region{ img->get_region(static_cast<size_t>(floor(std::max(center[0] - abs_radius, 0.0))),
                                         static_cast<size_t>(floor(std::max(center[1] - abs_radius, 0.0))),
                                         std::min(static_cast<size_t>(ceil(center[0] + abs_radius)), img->get_w()),
                                         std::min(static_cast<size_t>(ceil(center[1] + abs_radius)), img->get_h())) };
        std::for_each(img_region.begin(), img_region.end(), [&img, &center, &radius, &color](auto c) {
            double dist{ Vec2<double>(c.get_x(), c.get_y()).dist(center) };
            (*img)(c.get_x(), c.get_y()) +=
                Color(color.r(), color.g(), color.b(), color.a() * std::clamp(radius - dist, 0.0, 1.0));
        });
    } else {
        std::for_each(img->begin(), img->end(), [&img, &center, &radius, &color](auto c) {
            double dist{ Vec2<double>(c.get_x(), c.get_y()).dist(center) };
            (*img)(c.get_x(), c.get_y()) +=
                Color(color.r(), color.g(), color.b(), color.a() * std::clamp(dist + radius, 0.0, 1.0));
        });
    }
}

std::vector<Vec2<double>> get_nail_positions(Vec2<double> center, double radius, uint32_t n)
{
    std::vector<Vec2<double>> nail_positions;
    for (uint32_t i = 0; i < n; ++i) {
        double a{ i * 2 * std::numbers::pi / n };
        nail_positions.push_back(center + Vec2<double>(radius, 0.0).rotate(a));
    }
    return nail_positions;
}

int main(int argc, char* argv[])
{
    try {
        std::string pic_filename;

        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "-S") {
                if (++i == argc) {
                    throw "-S arg error";
                }
                pic_filename = std::string(argv[i]);
            }
        }

        if (pic_filename.empty())
            throw "supply pic file name after -S";

        Img pic(pic_filename);

        Vec2<double> center{ Vec2<double>(pic->get_w(), pic->get_h()) / 2.0 };
        double radius{ static_cast<double>(pic->get_w()) / 2.0 };
        double nail_radius{ 5.0 };

        std::vector<Vec2<double>> nail_positions = get_nail_positions(center, radius, 32);

        // double D{ 1.0 };
        // for (double a = 0; a < 2 * std::numbers::pi; a += 2 * std::numbers::pi / 32) {
        //     line(static_cast<double>(pic->get_w()) / 2.0,
        //          static_cast<double>(pic->get_w()) / 2.0 + 140 * std::cos(a),
        //          static_cast<double>(pic->get_h()) / 2.0,
        //          static_cast<double>(pic->get_h()) / 2.0 + 140 * std::sin(a),
        //          D,
        //          [&pic, D](int32_t x, int32_t y, double d) {
        //              (*pic)(x, y) += Color(1., 0., 0., string_fn(std::fabs(d) / (D / 2)));
        //          });
        //     D += 1.0;
        // }

        (*pic)(pic->get_w() / 2, pic->get_h() / 2) += Color(0., 1., 0., 1.0);

        draw_full_circle(pic, center, -radius, Color(0., 0., 0., 1.0));
        for (const auto& nail_pos : nail_positions) {
            draw_full_circle(pic, nail_pos, nail_radius, Color(0.8, 0.8, 0.9, 0.9));
        }

        pic.save("test.png");

        // ThreadPool tp;

        // StringArtSolver string_art_solver =
        //     StringArtSolver::Builder()
        //         .set_input_img(std::move(pic))
        //         .set_palette({ Color(1.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), Color(0.0, 0.0, 1.0) })
        //         .set_output_name("test2.png")
        //         .set_thread_pool(tp)
        //         .build();

        // string_art_solver.solve();
    } catch (const char* err) {
        std::cerr << err << '\n';
        return 1;
    }

    return 0;
}