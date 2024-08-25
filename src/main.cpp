#include "img.h"
#include "line.h"
#include "thread_pool.h"

#include <cmath>
#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>

double string_fn(double d)
{
    return std::max(std::min(1., 2. - d * 2.), 0.);
}

int main(int argc, char *argv[])
{
    try
    {
        std::string pic_filename;

        for (int i = 1; i < argc; i++)
        {
            if (std::string(argv[i]) == "-S")
            {
                if (++i == argc)
                    throw "-S arg error";
                pic_filename = std::string(argv[i]);
            }
        }

        if (pic_filename == "")
            throw "supply pic file name after -S";

        Img pic(pic_filename);

        // crop pic into circle
        std::for_each(pic->begin(), pic->end(),
                      [&pic](Array2d<Color>::Element a)
                      {
                          double x = a.get_x() - pic->get_w() / 2.0;
                          double y = a.get_y() - pic->get_h() / 2.0;
                          double r = std::sqrt(x * x + y * y);
                          if (r > pic->get_w() / 2.0)
                              *a = Color(0, 0, 0);
                      });

        double D = 1.;
        for (double a = 0; a < 2 * M_PI; a += 2 * M_PI / 32)
        {
            line(pic->get_w() / 2, pic->get_w() / 2 + 140 * std::cos(a), pic->get_h() / 2, pic->get_h() / 2 + 140 * std::sin(a), D,
                 [&pic, D](int32_t x, int32_t y, double d)
                 {
                     (*pic)(x, y) += Color(1., 0., 0., string_fn(std::fabs(d) / (D / 2)));
                 });
            D += 1.;
        }

        pic.save("test.png");

        // ThreadPool tp(-1);

        // std::osyncstream sout(std::cout);
        // std::vector<std::future<int>> futures;

        // for (int i = 0; i < 10000; i++)
        // {
        //     std::function<int(int)> f = [&sout](int i)
        //     {
        //         std::cout << "calculating: " << i << std::endl;
        //         using namespace std::chrono_literals;
        //         std::this_thread::sleep_for(1ms);
        //         return i;
        //     };
        //     futures.push_back(tp.submit(
        //         0, f,
        //         std::move(i)));
        // }

        // for (auto &f : futures)
        // {
        //     std::cout << f.get() << std::endl;
        // }
    }
    catch (const char *err)
    {
        std::cerr << err << '\n';
    }

    return 0;
}