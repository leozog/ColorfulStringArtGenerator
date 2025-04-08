#include "array2d.h"
#include "img.h"
#include "line.h"
#include "string_art_solver.h"
#include "thread_pool.h"

#include <cmath>
#include <iostream>
#include <numbers>

double string_fn(double d)
{
    return std::max(std::min(1., 1. - d * 1.), 0.);
}

void crop_img_into_circle(Img& img)
{
    std::for_each(img->begin(), img->end(), [&img](auto a) {
        Vec2<double> center{ static_cast<double>(img->get_w()) / 2.0, static_cast<double>(img->get_h()) / 2.0 };
        Vec2<double> p{ static_cast<double>(a.get_x()), static_cast<double>(a.get_y()) };
        double r{ p.dist(center) };
        *a += Color(0.0, 0.0, 0.0, std::clamp(r - (static_cast<double>(img->get_w()) / 2.0), 0.0, 1.0));
    });
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

        crop_img_into_circle(pic);

        double D{ 1.0 };
        for (double a = 0; a < 2 * std::numbers::pi; a += 2 * std::numbers::pi / 32) {
            line(static_cast<double>(pic->get_w()) / 2.0,
                 static_cast<double>(pic->get_w()) / 2.0 + 140 * std::cos(a),
                 static_cast<double>(pic->get_h()) / 2.0,
                 static_cast<double>(pic->get_h()) / 2.0 + 140 * std::sin(a),
                 D,
                 [&pic, D](int32_t x, int32_t y, double d) {
                     (*pic)(x, y) += Color(1., 0., 0., string_fn(std::fabs(d) / (D / 2)));
                 });
            D += 1.0;
        }

        (*pic)(pic->get_w() / 2, pic->get_h() / 2) += Color(0., 1., 0., 1.0);

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