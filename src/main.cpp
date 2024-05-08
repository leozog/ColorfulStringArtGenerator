#include "img.h"
#include "line.h"

#include <cmath>
#include <iostream>

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
                    throw "arg error";
                pic_filename = std::string(argv[i]);
            }
        }

        if (pic_filename == "")
            throw "supply pic file name after -S";

        img pic(pic_filename);

        // crop pic into circle
        pic->for_each(
            [&pic](color &a, size_t _x, size_t _y)
            {
                double x = _x;
                double y = _y;
                x -= pic->get_w() / 2.0;
                y -= pic->get_w() / 2.0;
                double r = std::sqrt(x * x + y * y);
                if (r > pic->get_w() / 2.0)
                    a = color(0, 0, 0);
            });

        double D = 1.;
        for (double a = 0; a < 2 * M_PI; a += 2 * M_PI / 32)
        {
            line(pic->get_w() / 2, pic->get_w() / 2 + 140 * std::cos(a), pic->get_h() / 2, pic->get_h() / 2 + 140 * std::sin(a), D,
                 [&pic, D](int32_t x, int32_t y, double d)
                 {
                     (*pic)(x, y) += color(1., 0., 0., std::max(D / 2. - std::fabs(d), 0.) / (D / 2.));
                 });
            D += 1.;
        }

        pic.save("test.png");
    }
    catch (const char *err)
    {
        std::cout << err << '\n';
    }

    return 0;
}