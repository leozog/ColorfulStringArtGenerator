#include "array2d.h"
#include "color.h"
#include "img.h"
#include "img_color_quantizer.h"
#include "logger.h"
#include "string_art_solver.h"
#include "thread_pool.h"
#include "vec.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <memory>
#include <string>

void draw_full_circle(Img& img, Vec2<double> center, double radius, Color color)
{
    if (radius > 0) {
        double abs_radius{ std::fabs(radius) };
        auto img_region{ img.get_region(static_cast<size_t>(floor(std::max(center[0] - abs_radius, 0.0))),
                                        static_cast<size_t>(floor(std::max(center[1] - abs_radius, 0.0))),
                                        std::min(static_cast<size_t>(ceil(center[0] + abs_radius)), img.get_w()),
                                        std::min(static_cast<size_t>(ceil(center[1] + abs_radius)), img.get_h())) };
        std::for_each(img_region.begin(), img_region.end(), [&img, &center, &radius, &color](auto c) {
            double dist{ Vec2<double>(c.get_x(), c.get_y()).dist(center) };
            img(c.get_x(), c.get_y()) +=
                Color(color.r(), color.g(), color.b(), color.a() * std::clamp(radius - dist, 0.0, 1.0));
        });
    } else {
        std::for_each(img.begin(), img.end(), [&img, &center, &radius, &color](auto c) {
            double dist{ Vec2<double>(c.get_x(), c.get_y()).dist(center) };
            img(c.get_x(), c.get_y()) +=
                Color(color.r(), color.g(), color.b(), color.a() * std::clamp(dist + radius, 0.0, 1.0));
        });
    }
}

int main(int argc, char* argv[])
{
    try {
        std::string pic_filename;

        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "-S") {
                if (++i == argc) {
                    throw std::runtime_error("-S arg error");
                }
                pic_filename = std::string(argv[i]);
            }
        }

        if (pic_filename.empty()) {
            throw std::runtime_error("supply target img file name after -S");
        }

        Logger::info("Loading image: {}", pic_filename);
        Img pic{ Img::load(pic_filename) };

        ThreadPool tp;

        Logger::info("Creating color palette");
        ImageColorQuantizer img_color_quantizer{ pic, { Color(1.0, 1.0, 1.0) }, tp };
        std::vector<Color> palette{ img_color_quantizer.get_pallete(32, 32, 0.01) };

        std::string palette_str{ "{ " };
        for (int i = 0; i < palette.size(); i++) {
            auto color = palette[i];
            palette_str += std::format("( {:.0f}, {:.0f}, {:.0f} )", 255 * color.r(), 255 * color.g(), 255 * color.b());
            if (i != palette.size() - 1) {
                palette_str += ", ";
            }
        }
        palette_str += " }";
        Logger::info("Created color palette: {}", palette_str);

        StringArtSolver string_art_solver = StringArtSolver::Builder()
                                                .set_target_img(std::move(pic))
                                                .set_background_color(Color(1.0, 1.0, 1.0))
                                                // .set_palette({ Color(0.0, 0.0, 0.1),
                                                //                Color(0.8, 0.1, 0.1),
                                                //                Color(0.8, 0.8, 0.1),
                                                //                Color(0.4, 0.0, 0.0),
                                                //                Color(1.0, 1.0, 1.0) })
                                                // .set_palette({ Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.8), Color(0.8,
                                                // 0.8, 0.0), Color(1.0, 1.0, 1.0) })
                                                // .set_palette({ Color(0.0, 0.0, 0.0) })
                                                .set_palette(std::move(palette))
                                                .set_img_diameter_cm(20.0)
                                                .set_nail_count(200)
                                                .set_nail_diameter_cm(0.1)
                                                .set_nail_img_dist_cm(0.1)
                                                .set_string_diameter_cm(0.05)
                                                .set_thread_pool(tp)
                                                .build();

        Logger::info("Starting string art solver");
        string_art_solver.solve();

        Logger::info("Saving output sequence: {}", "test.txt");
        std::unique_ptr<StringSequence> seq = string_art_solver.get_sequence();
        // save sequence as txt file
        std::ofstream seq_file("test.txt");
        if (!seq_file) {
            throw std::runtime_error("Failed to open file for writing sequence");
        }
        seq_file << seq->get_str();
        seq_file.close();

        Logger::info("Saving output image: {}", "test.png");
        Img output = *string_art_solver.get_img();
        output(output.get_w() / 2, output.get_h() / 2) += Color(0., 1., 0., 1.0);
        // Vec2<double> center{ Vec2<double>(output.get_w(), pic->get_h()) / 2.0 };
        // double radius{ static_cast<double>(output.get_w()) / 2.0 };
        // double nail_radius{ 5.0 };
        // draw_full_circle(output, center, -radius, Color(0., 0., 0., 1.0));
        // for (const auto& nail_pos : string_art_solver.get_nail_positions()) {
        //     draw_full_circle(output, nail_pos, nail_radius, Color(0.8, 0.8, 0.9, 0.9));
        // }
        output.save("test.png");

        Logger::info("Done");
    } catch (const char* err) {
        Logger::error("{}", err);
        return 1;
    } catch (const std::exception& err) {
        Logger::error("{}", err.what());
        return 1;
    } catch (...) {
        Logger::error("Unknown error");
        return 1;
    }

    return 0;
}