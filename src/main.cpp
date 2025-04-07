#include "img.h"
#include "line.h"
#include "string_art_solver.h"
#include "thread_pool.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>
#include <syncstream>
#include <thread>

double string_fn(double d)
{
    return std::max(std::min(1., 2. - d * 2.), 0.);
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

        // crop pic into circle
        std::for_each(pic->begin(), pic->end(), [&pic](Array2d<Color>::Element a) {
            double x{ a.get_x() - pic->get_w() / 2.0 };
            double y{ a.get_y() - pic->get_h() / 2.0 };
            double r{ std::sqrt(x * x + y * y) };
            if (r > pic->get_w() / 2.0) {
                *a = Color(0, 0, 0);
            }
        });

        double D{ 1.0 };
        for (double a = 0; a < 2 * std::numbers::pi; a += 2 * std::numbers::pi / 32) {
            line(pic->get_w() / 2,
                 pic->get_w() / 2 + 140 * std::cos(a),
                 pic->get_h() / 2,
                 pic->get_h() / 2 + 140 * std::sin(a),
                 D,
                 [&pic, D](int32_t x, int32_t y, double d) {
                     (*pic)(x, y) += Color(1., 0., 0., string_fn(std::fabs(d) / (D / 2)));
                 });
            D += 1.0;
        }

        pic.save("test.png");

        ThreadPool tp;

        StringArtSolver string_art_solver =
            StringArtSolver::Builder()
                .set_input_img(std::move(pic))
                .set_palette({ Color(1.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), Color(0.0, 0.0, 1.0) })
                .set_output_name("test2.png")
                .set_thread_pool(tp)
                .build();

        string_art_solver.solve();
    } catch (const char* err) {
        std::cerr << err << std::endl;
        return 1;
    }

    return 0;
}