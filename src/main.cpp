#include "img.h"
#include "line_chen.h"

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
                    return color(0, 0, 0);
                return a;
            });

        line_chen(10, 100, 10, 50,
                  [&pic](int32_t x, int32_t y, double d)
                  {
                      (*pic)(x, y) += color(1., 0., 0., std::max(1. - std::fabs(d), 0.));
                  });

        line_chen(10, 100, 10, 10,
                  [&pic](int32_t x, int32_t y, double d)
                  {
                      (*pic)(x, y) += color(0., 1., 0., std::max(1. - std::fabs(d), 0.));
                  });

        line_chen(10, 80, 10, 100,
                  [&pic](int32_t x, int32_t y, double d)
                  {
                      (*pic)(x, y) += color(0., 0., 1., std::max(1. - std::fabs(d), 0.));
                  });

        pic.save("test.png");
    }
    catch (const char *err)
    {
        std::cout << err << '\n';
    }

    return 0;
}